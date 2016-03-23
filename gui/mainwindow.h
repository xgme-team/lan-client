#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "model.h"
#include "torrentlogdialog.h"

QT_BEGIN_NAMESPACE
class QFile;
class QSessionManager;
class QSettings;
class QUrl;
QT_END_NAMESPACE
class Application;
class Model;
class TorrentLogDialog;

namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	virtual ~MainWindow();

public slots:
	void openTorrent();
	void openTorrent(const QUrl &url);
	void createTorrentFile();
	void createTorrentDirectory();
	void createTorrent(const QString &fileOrDirName);
	void openOrCreateTorrent(const QString &fileOrDirName);

protected:
	virtual void closeEvent(QCloseEvent *event) override;
	virtual void dragEnterEvent(QDragEnterEvent *event) override;
	virtual void dropEvent(QDropEvent *event) override;

private slots:
	void onSessionUpdate();
	void onShutdown();

// TODO Should I use them?
//	void commitData(QSessionManager &);

private:
	void readSettings();
	void writeSettings();

	Ui::MainWindow * const ui;
	QSettings * const settings;

	TorrentLogDialog * const logDialog;

};

#endif // MAINWINDOW_H
