#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QAction>
#include <QCloseEvent>
#include <QDesktopServices>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QMimeData>
#include <QSettings>

#include <libtorrent/torrent_info.hpp>

#include "application.h"
#include "model.h"
#include "opentorrentdialog.h"
#include "torrentlogdialog.h"
#include "torrentsession.h"
#include "torrentsessionstatus.h"
#include "torrentsmodel.h"
#include "trayicon.h"
#include "utils.h"

namespace lt = libtorrent;


MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
	, settings(new QSettings(this))
	, logDialog(new TorrentLogDialog(myApp->model(), this))
{
	// Get instances of other modules of the application.
	Application *app = myApp;
	Model *model = app->model();
	TorrentSession *session = model->session();
	// Set up window.
	ui->setupUi(this);
	setAcceptDrops(true);

	//: File menu
	QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(app->openTorrentAction());
	fileMenu->addAction(app->newTorrentFromFileAction());
	fileMenu->addAction(app->newTorrentFromDirAction());
	fileMenu->addSeparator();
	fileMenu->addAction(app->exitAction());

	// Set model as base for all views.
	ui->transmissions->setModel(session->torrents()->downloads());

	// Read and apply configuration.
	readSettings();

	// Register handlers.
	QDesktopServices::setUrlHandler("magnet", this, "openTorrent");

	connect(session, &TorrentSession::statusUpdated,
	        this, &MainWindow::onSessionUpdate);
	connect(app, &Application::shutdownStarted,
	        this, &MainWindow::onShutdown);
//	connect(app, &Application::commitDataRequest,
//	        this, &MainWindow::commitData);

	// Register action handlers.
	connect(app->showWindowAction(), &QAction::triggered,
	        this, &MainWindow::show);
	connect(app->openTorrentAction(), &QAction::triggered,
	        this, static_cast<void(MainWindow::*)()>(&MainWindow::openTorrent));
	connect(app->newTorrentFromFileAction(), &QAction::triggered,
	        this, &MainWindow::createTorrentFile);
	connect(app->newTorrentFromDirAction(), &QAction::triggered,
	        this, &MainWindow::createTorrentDirectory);
}

MainWindow::~MainWindow()
{
	writeSettings();
	QDesktopServices::unsetUrlHandler("magnet");
	delete ui;
}

void MainWindow::openTorrent()
{
	QFileDialog dialog(this, tr("Open Torrent"), QString(),
	                   tr("Torrent Files (*.torrent)"));
	dialog.setFileMode(QFileDialog::ExistingFiles);
	if (dialog.exec()) {
		QStringList fileNames;
		fileNames = dialog.selectedFiles();
		for (QString fileName : fileNames) {
			openTorrent(QUrl::fromLocalFile(fileName));
		}
	}
}

void MainWindow::openTorrent(const QUrl &url)
{
	std::unique_ptr<lt::torrent_info> info;
	if (url.isLocalFile()) {
		QFile file(url.toLocalFile());
		if (!file.open(QFile::ReadOnly)) {
			QMessageBox::critical(
						this,
						tr("Could not open file"),
						tr("Could not open file: %1").arg(file.fileName()));
			return;
		}
		QByteArray data = file.readAll();
		file.close();

		lt::error_code e;
		info.reset(new lt::torrent_info(data.constData(), data.size(), e));
	}

	OpenTorrentDialog *dialog = new OpenTorrentDialog(this);
	if (dialog->exec()) {
		QString savePath = dialog->getSavePath();
		if (info) {
			myApp->model()->session()->addTorrent(*info, savePath);
		} else {
			myApp->model()->session()->addTorrentMagnet(url, savePath);
		}
	}
}

void MainWindow::createTorrentFile()
{
	QFileDialog dialog(this, tr("Select file"), QString());
	dialog.setFileMode(QFileDialog::ExistingFile);
	if (dialog.exec()) {
		QString fileName;
		fileName = dialog.selectedFiles()[0];
		createTorrent(fileName);
	}
}

void MainWindow::createTorrentDirectory()
{
	QFileDialog dialog(this, tr("Select directory"), QString());
	dialog.setFileMode(QFileDialog::Directory);
	if (dialog.exec()) {
		QString dirName;
		dirName = dialog.selectedFiles()[0];
		createTorrent(dirName);
	}
}

void MainWindow::createTorrent(const QString &fileOrDirName)
{
	// TODO
}

void MainWindow::openOrCreateTorrent(const QString &fileOrDirName)
{
	if (fileOrDirName.endsWith(".torrent", Qt::CaseInsensitive)) {
		openTorrent(QUrl::fromLocalFile(fileOrDirName));
	} else {
		createTorrent(fileOrDirName);
	}
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	//writeSettings();
	event->ignore();
	hide();
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
	if (event->possibleActions() & Qt::CopyAction) {
		if (event->mimeData()->hasUrls()) {
			event->setDropAction(Qt::CopyAction);
			event->accept();
		}
	}
}

void MainWindow::dropEvent(QDropEvent *event)
{
	if (event->dropAction() == Qt::CopyAction && event->mimeData()->hasUrls()) {
		event->accept();
		// TODO is it a good idea to make a function call for every element?
		for (QUrl url : event->mimeData()->urls()) {
			if (url.isLocalFile()) {
				openOrCreateTorrent(url.toLocalFile());
			} else {
				openTorrent(url);
			}
		}
	}
}

void MainWindow::onSessionUpdate()
{
	const TorrentSessionStatus *s = myApp->model()->session()->status();
	ui->downloadRate->setText(Utils::makeSpeedStr(s->payloadDownloadRate()));
	ui->uploadRate->setText(Utils::makeSpeedStr(s->payloadUploadRate()));
}

void MainWindow::onShutdown()
{
	// TODO show some overlay.
}

//void MainWindow::commitData(QSessionManager &)
//{
//	writeSettings();
//}

//void MainWindow::torrentAlert(const lt::alert *alert)
//{
//	switch (alert->type()) {
//	case lt::torrent_error_alert::alert_type:
//	{
//		const lt::torrent_error_alert *a =
//				static_cast<const lt::torrent_error_alert*>(alert);
//		statusBar()->showMessage(tr(a->message().c_str()), 5000);
//		break;
//	}
//	case lt::torrent_added_alert::alert_type:
//	{
//		const lt::torrent_added_alert *a =
//				static_cast<const lt::torrent_added_alert*>(alert);
//		statusBar()->showMessage(tr(a->message().c_str()), 5000);
//		break;
//	}
//	case lt::listen_failed_alert::alert_type:
//	{
//		const lt::listen_failed_alert *a =
//				static_cast<const lt::listen_failed_alert*>(alert);
//		statusBar()->showMessage(tr(a->message().c_str()), 5000);
//		break;
//	}
//	case lt::listen_succeeded_alert::alert_type:
//	{
//		const lt::listen_succeeded_alert *a =
//				static_cast<const lt::listen_succeeded_alert*>(alert);
//		statusBar()->showMessage(tr(a->message().c_str()), 5000);
//		break;
//	}
//	case lt::file_completed_alert::alert_type:
//	{
//		const lt::file_completed_alert *a =
//				static_cast<const lt::file_completed_alert*>(alert);
//		statusBar()->showMessage(tr(a->message().c_str()), 5000);
//		break;
//	}
//	case lt::performance_alert::alert_type:
//	{
//		const lt::performance_alert *a =
//				static_cast<const lt::performance_alert*>(alert);
//		statusBar()->showMessage(tr(a->message().c_str()), 5000);
//		break;
//	}
//	}
//}

void MainWindow::readSettings()
{
	settings->beginGroup("MainWindow");
	resize(settings->value("size", size()).toSize());
	move(settings->value("pos", pos()).toPoint());
	settings->endGroup();
}

void MainWindow::writeSettings()
{
	settings->beginGroup("MainWindow");
	settings->setValue("size", size());
	settings->setValue("pos", pos());
	settings->endGroup();
}
