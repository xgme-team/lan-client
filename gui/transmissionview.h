#ifndef TRANSMISSIONVIEW_H
#define TRANSMISSIONVIEW_H

#include <QTableView>

class TorrentsModel;
class TransmissionViewDelegate;
class TransmissionViewProxy;

class TransmissionView : public QTableView
{
	Q_OBJECT

public:
	explicit TransmissionView(QWidget *parent = 0);

	//! Sets the model for the view. You should only use instances of TorrentsModel here.
	void setModel(QAbstractItemModel *model) override;

private:
	TransmissionViewDelegate *delegate;
	TransmissionViewProxy *proxyModel;

};

#endif // TRANSMISSIONVIEW_H
