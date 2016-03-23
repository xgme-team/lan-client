#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include <QIcon>

QT_BEGIN_NAMESPACE
class QAction;
QT_END_NAMESPACE
class ApplicationLauncher;
class LocalApplicationServer;
class MainWindow;
class Model;
class TrayIcon;


//! Gets the instance of Application.
#define myApp static_cast<Application*>(qApp)

class Application : public QApplication
{
	Q_OBJECT
	Q_PROPERTY(QAction* exitAction               READ exitAction)
	Q_PROPERTY(QAction* showWindowAction         READ showWindowAction)
	Q_PROPERTY(QAction* openTorrentAction        READ openTorrentAction)
	Q_PROPERTY(QAction* newTorrentFromFileAction READ newTorrentFromFileAction)
	Q_PROPERTY(QAction* newTorrentFromDirAction  READ newTorrentFromDirAction)

public:
	explicit Application(int &argc, char **argv);

	Model *model() const;

	QAction *exitAction() const               {return mExitAction;}
	QAction *showWindowAction() const         {return mShowWindowAction;}
	QAction *openTorrentAction() const        {return mOpenTorrentAction;}
	QAction *newTorrentFromFileAction() const {return mNewTorrentFromFileAction;}
	QAction *newTorrentFromDirAction() const  {return mNewTorrentFromDirAction;}

	const QIcon &iconDefault() const {return mIconDefault;}
	const QIcon &iconFailure() const {return mIconFailure;}
	const QIcon &iconSevere() const  {return mIconSevere;}
	const QIcon &iconSuccess() const {return mIconSuccess;}

signals:
	void shutdownStarted();
	void shutdownFinished();

public slots:
	void shutdown();

private:
	ApplicationLauncher *mLauncher;

	QAction *mExitAction;
	QAction *mShowWindowAction;
	QAction *mOpenTorrentAction;
	QAction *mNewTorrentFromFileAction;
	QAction *mNewTorrentFromDirAction;

	const QIcon mIconDefault = QIcon(QStringLiteral(":/icons/app-default"));
	const QIcon mIconFailure = QIcon(QStringLiteral(":/icons/app-default"));
	const QIcon mIconSevere  = QIcon(QStringLiteral(":/icons/app-error"));
	const QIcon mIconSuccess = QIcon(QStringLiteral(":/icons/app-success"));

};

#endif // APPLICATION_H
