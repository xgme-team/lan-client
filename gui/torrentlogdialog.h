#ifndef TORRENTLOGDIALOG_H
#define TORRENTLOGDIALOG_H

#include <QWidget>

#include "model.h"


class TorrentLogDialog : public QWidget
{
	Q_OBJECT
public:
	explicit TorrentLogDialog(Model *model, QWidget *parent = 0);
	virtual ~TorrentLogDialog();

private:
	Model * const model;

};

#endif // TORRENTLOGDIALOG_H
