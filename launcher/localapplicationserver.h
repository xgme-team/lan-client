#ifndef LOCALAPPLICATIONSERVER_H
#define LOCALAPPLICATIONSERVER_H

#include <QObject>

QT_BEGIN_NAMESPACE
class QLocalServer;
class QLocalSocket;
class QSharedMemory;
class QString;
QT_END_NAMESPACE
class Application;


class LocalApplicationServer : public QObject
{
	Q_OBJECT

public:
	explicit LocalApplicationServer(QObject *parent = 0);

	bool isServer() const;
	bool isClient() const;

public slots:
	void close();
	void sendShowWindowMessage();

private slots:
	void onNewConnection();
	void onMessageReceived(QString message);
	void onShutdown();

private:
	void sendMessage(const QString &message);

	QLocalSocket *mClientSocket;
	QLocalServer *mServer;
	QSharedMemory *mSharedMemory;

};

#endif // LOCALAPPLICATIONSERVER_H
