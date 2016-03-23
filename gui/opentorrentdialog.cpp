#include "opentorrentdialog.h"
#include "ui_opentorrentdialog.h"

#include <QFileDialog>
#include <QMessageBox>


OpenTorrentDialog::OpenTorrentDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::OpenTorrentDialog)
{
	ui->setupUi(this);
}

OpenTorrentDialog::~OpenTorrentDialog()
{
	delete ui;
}

QString OpenTorrentDialog::getSavePath()
{
	return ui->lineEdit->text();
}

void OpenTorrentDialog::accept()
{
	QDir dir (ui->lineEdit->text());
	if (dir.isAbsolute() && (dir.exists() || dir.mkpath("."))) {
		ui->lineEdit->setText(dir.absolutePath());
		QDialog::accept();
	} else {
		QMessageBox::critical(
					this,
					tr("Directory dose not exist"),
					tr("Directory does not exist and could not be created:\n%1").arg(ui->lineEdit->text()));
	}
}

void OpenTorrentDialog::on_pushButton_clicked()
{
	QFileDialog dialog(this, tr("Select directory to download files"), QString());
	dialog.setFileMode(QFileDialog::Directory);
	if (dialog.exec()) {
		QString dirName;
		dirName = dialog.selectedFiles()[0];
		ui->lineEdit->setText(dirName);
	}
}
