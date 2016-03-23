#include "application.h"

#include <cassert>

#include <QAction>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QIcon>
#include <QTimer>

#include "localapplicationserver.h"
#include "mainwindow.h"
#include "model.h"
#include "trayicon.h"


// I use this class for easy cleanup. See last comment in constructor of
// Application for more information.
class ApplicationLauncher : public QObject
{
	Q_OBJECT
	friend class Application;
public:
	ApplicationLauncher(Application *application) : QObject(application) {}
	// I have to delete mMainWindow manually because the parent must be a widget :(
	~ApplicationLauncher() {delete mMainWindow;}

public slots:
	void launch();

private:
	LocalApplicationServer *mApplicationServer;
	Model *mModel;
	// Set to null to protect for SEGV on destruction if MainWindow is not created.
	MainWindow *mMainWindow = nullptr;
	TrayIcon *mTrayIcon;

};

void ApplicationLauncher::launch()
{
	// Get instance of Application.
	assert(dynamic_cast<Application*>(parent()));
	Application *app = static_cast<Application*>(parent());

	// Parse command line options.
	QCommandLineParser parser;
	parser.setApplicationDescription("XGME LAN-Client");
	parser.addHelpOption();
	parser.addVersionOption();

	QCommandLineOption hiddenOption("hidden", tr("Do not open the window."));
	parser.addOption(hiddenOption);

	parser.process(*app);

	// Ensure that there is no other instance running already.
	mApplicationServer = new LocalApplicationServer(this);
	if (mApplicationServer->isServer())
	{
		// This is the first instance of the application.
		// Initialize model and gui.
		mModel = new Model(this);
		mMainWindow = new MainWindow();
		mTrayIcon = new TrayIcon(this);

		if (!parser.isSet(hiddenOption))
			mMainWindow->show();
	}
	else if (mApplicationServer->isClient())
	{
		// There is another instance running already.
		mApplicationServer->sendShowWindowMessage();
		QApplication::quit();
	}
	else
	{
		// Error occurred in LocalApplicationServer.
		// Since LocalApplicationServer shows already a message box, we do
		// nothing beside exiting the application here.
		QApplication::exit(1);
	}
}

Application::Application(int &argc, char **argv)
	: QApplication(argc, argv)
{
	// Set global properties of the applications.
	// TODO setlocale(LC_NUMERIC, "C"); ?
	setOrganizationName("XGME");
	setOrganizationDomain("xgme.de");
	setApplicationName("LAN-Client");
	setApplicationVersion("1.0-dev");

	// Create common actions.
	mExitAction               = new QAction(QIcon(QStringLiteral(":/icons/exit")),    tr("Exit"), this);
	mShowWindowAction         = new QAction(QIcon(QStringLiteral(":/icons/show")),    tr("Show window"), this);
	mOpenTorrentAction        = new QAction(QIcon(QStringLiteral(":/icons/open")),    tr("&Open torrent"), this);
	mNewTorrentFromFileAction = new QAction(QIcon(QStringLiteral(":/icons/newFile")), tr("New torrent from &file"), this);
	mNewTorrentFromDirAction  = new QAction(QIcon(QStringLiteral(":/icons/newDir")),  tr("New torrent from &directory"), this);

	// Set some properties of created options.
	mExitAction->setToolTip(tr("Just quits the application."));
	mOpenTorrentAction->setToolTip(tr("Opens a dialog where you can choose a torrent to open."));
	mNewTorrentFromFileAction->setToolTip(tr("Opens a dialog to creates a new torrent from a file."));
	mNewTorrentFromDirAction->setToolTip(tr("Opens a dialog to creates a new torrent from a directory."));

	// Shutdown the application when the exit action is triggerd.
	connect(mExitAction, &QAction::triggered,
	        this, &Application::shutdown);

	// Start launcher. I use QTimer to launch the application when the event
	// loop is running. ApplicationLauncher::launch will no be executed at this
	// position. It will be executed after `exec` is called.
	mLauncher = new ApplicationLauncher(this);
	QTimer::singleShot(0, mLauncher, &ApplicationLauncher::launch);

	// Clean up (nearly) everything when the application quits.
	// TODO Should I do that? It might not be designed to be used like this.
	//      But it could be a easy way to exit the application cleanly since
	//      the destructorss of the classes may not run otherwise. See the
	//      documentation of QApplication::aboutToQuit for more information
	//      about this issue.
	connect(this, &Application::aboutToQuit, [this](){
		delete mLauncher;
		mLauncher = nullptr;
	});
}

Model *Application::model() const
{
	return mLauncher->mModel;
}

void Application::shutdown()
{
	shutdownStarted();
	// TODO Allow modules to delay the process.
	shutdownFinished();
	QApplication::quit();
}

#include "application.moc"
