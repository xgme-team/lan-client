#include "localapplicationserver.h"

#include <cassert>

#include <QAction>
#include <QByteArray>
#include <QDataStream>
#include <QLocalServer>
#include <QLocalSocket>
#include <QMessageBox>
#include <QMessageLogger>
#include <QSharedMemory>
#include <QString>

#include "application.h"


static const QString showWindowMessage = QStringLiteral("show");

// LocalApplicationServerClientHandler is used to handle connected clients.
class LocalApplicationServerClientHandler : public QObject
{
	Q_OBJECT
public:
	LocalApplicationServerClientHandler(QLocalSocket *socket, LocalApplicationServer *appServer)
		: QObject(appServer)
		, mSocket(socket)
	{
		// Change the parent of the socket to this.
		// TODO Is it legal? And why must I do it? I get SEGV if I delete it in
		//      the destructor and a connection exists while the app is exiting.
		//      QLocalServer (parent of mSocket) seems to be deleted first.
		socket->setParent(this);
		// Register handler to handle shutdown of the application.
		connect(myApp, &Application::shutdownStarted,
		        this, &LocalApplicationServerClientHandler::shutdown);
		// Process data whenever new data is available.
		connect(mSocket, &QLocalSocket::readyRead,
		        this, &LocalApplicationServerClientHandler::processData);
		// Destroy the handler if the client is disconnected.
		connect(mSocket, &QLocalSocket::disconnected,
		        this, &LocalApplicationServerClientHandler::onDisconnected);
	}

	~LocalApplicationServerClientHandler()
	{
		// I'm not sure about the implementation of QLocalSocket. Maybe it fires
		// the signal `disconnected` on destruction. It would cause another call
		// of `delete this;` if I would not disconnect the slot first.
		disconnect(mSocket, &QLocalSocket::disconnected,
		           this, &LocalApplicationServerClientHandler::onDisconnected);

		// Delete socken because it is not child of the handler and would live
		// util the application is closed.
		//delete mSocket; (See TODO in constructor to get information why it is commented out)
	}

signals:
	void messageReceived(QString message);

private slots:
	void shutdown()
	{
		// TODO Shutdown the handler properly.
	}

	void processData()
	{
		// Write new data to buffer.
		auto available = mSocket->bytesAvailable();
		QByteArray newData = mSocket->read(available);
		assert(newData.length() == available);
		mBuffer.append(newData);
		// Process data.
		int pos = 0, oldPos = 0;
		while ((pos = mBuffer.indexOf('\0', pos)) != -1) {
			assert(pos >= 0);
			QString msg = QString::fromUtf8(mBuffer.mid(oldPos, pos - oldPos));
			messageReceived(msg);
			oldPos = ++pos;
		}
		// Remove processed data from buffer.
		// Use `oldPos` because pos is set to -1 while the last check in the loop.
		mBuffer.remove(0, oldPos);
	}

	void onDisconnected()
	{
		// The client closes his side of the connection.
		if (!mBuffer.isEmpty()) {
			qWarning() << "Application client disconnected while last message is incomplete.";
		}
		qInfo() << "Application client disconnected.";
		// Close connection.
		mSocket->close();
		// Delete the handler itself since there is nothing to handle anymore.
		deleteLater();
	}

private:
	QByteArray mBuffer;
	QLocalSocket *mSocket;

};

LocalApplicationServer::LocalApplicationServer(QObject *parent)
	: QObject(parent)
	, mClientSocket(nullptr) // It is important to set it to null since isClient() uses it.
	, mServer(nullptr)       // It is important to set it to null since isServer() uses it.
{
	// Get instace of the application.
	Application *app = myApp;
	// Register event handlers.
	connect(app, &Application::shutdownStarted,
	        this, &LocalApplicationServer::onShutdown);

	// Create a (unique) key for the application.
	const QString key = app->organizationName()
	                    + "." + app->applicationName();

	// Create shared memory object. It can be used as a system wide mutex.
	mSharedMemory = new QSharedMemory(key, this);
	if (mSharedMemory->create(1, QSharedMemory::ReadOnly))
	{
		// This is the first instance of the application.
		// Create a local server to receive messages from other instances.
		mServer = new QLocalServer(this);
		QLocalServer::removeServer(key);
		if (!mServer->listen(key)) {
			// An error occurred.
			QMessageBox::critical(nullptr, tr("Could not start LAN-Client"),
			                      tr("Could not create a socket to listen for messages of other instances: %1")
			                      .arg(mServer->errorString()));
			// Delete mServer, so isServer returns false.
			delete mServer;
			mServer = nullptr;
		} else {
			// Register connection handler.
			connect(mServer, &QLocalServer::newConnection,
			        this, &LocalApplicationServer::onNewConnection);
		}
	}
	else
	{
		// Could not create the shared memory block.
		switch (mSharedMemory->error()) {
		case QSharedMemory::AlreadyExists:
			// There is another instance running already.
			mClientSocket = new QLocalSocket(this);
			mClientSocket->connectToServer(key);
			if (!mClientSocket->waitForConnected()) {
				// An error occurred.
				mClientSocket->abort();
				QMessageBox::critical(nullptr, tr("Could not start LAN-Client"),
				                      tr("There is another instance running but it does not react: %1")
				                      .arg(mSharedMemory->errorString()));
				// Delete mClientSocket, so isClient returns false.
				delete mClientSocket;
				mClientSocket = nullptr;
			}
			break;
		default:
			// An error occurred.
			QMessageBox::critical(nullptr, tr("Could not start LAN-Client"),
			                      tr("There is an error occurred while checking for another instance: %1")
			                      .arg(mSharedMemory->errorString()));
			break;
		}
	}
}

bool LocalApplicationServer::isServer() const
{
	return mServer != nullptr;
}

bool LocalApplicationServer::isClient() const
{
	return mClientSocket != nullptr;
}

void LocalApplicationServer::close()
{
	assert(isClient());
	mClientSocket->close();
}

void LocalApplicationServer::sendShowWindowMessage()
{
	sendMessage(showWindowMessage);
}

void LocalApplicationServer::onNewConnection()
{
	// Get an instance of the application.
	Application *app = myApp;
	// Create a client handler for every connection.
	while (QLocalSocket *socket = mServer->nextPendingConnection()) {
		qInfo() << "Application client connected.";
		LocalApplicationServerClientHandler *handler
				= new LocalApplicationServerClientHandler(socket, this);
		connect(handler, &LocalApplicationServerClientHandler::messageReceived,
		        this, &LocalApplicationServer::onMessageReceived);

		// Process already received data. (I don't know if it is required. My
		// tests suggest that it is not. I have commented it out.)
		//handler->processData();
	}
}

void LocalApplicationServer::onMessageReceived(QString message)
{
	// Get an instance of the application.
	Application *app = myApp;
	// Handle message.
	if (message == showWindowMessage) {
		qInfo() << "Application client send show-window message.";
		app->showWindowAction()->trigger();
	} else {
		// Message not known.
		qWarning() << "Application client sent unknown message: " << message;
	}
}

void LocalApplicationServer::onShutdown()
{
	if (isServer()) {
		// Do not accept incomming connections anymore.
		// TODO
	}
}

void LocalApplicationServer::sendMessage(const QString &message)
{
	assert(mClientSocket);
	assert(!message.contains('\n'));
	// Convert message and try to write the data to the socket.
	QByteArray data = message.toUtf8() + '\0';
	auto written = mClientSocket->write(data);
	// Check if everything was written.
	if (written != data.length()) {
		// Buffer the message.
		// TODO
	} else {
		mClientSocket->flush();
	}
}

#include "localapplicationserver.moc"
