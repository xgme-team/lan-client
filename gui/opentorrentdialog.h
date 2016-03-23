#ifndef OPENTORRENTDIALOG_H
#define OPENTORRENTDIALOG_H

#include <QDialog>


namespace Ui {
class OpenTorrentDialog;
}

class OpenTorrentDialog : public QDialog
{
	Q_OBJECT
public:
	explicit OpenTorrentDialog(QWidget *parent = 0);
	virtual ~OpenTorrentDialog();

	QString getSavePath();

public slots:
	void accept() override;

private slots:
	void on_pushButton_clicked();

private:
	Ui::OpenTorrentDialog *ui;

};

#endif // OPENTORRENTDIALOG_H
