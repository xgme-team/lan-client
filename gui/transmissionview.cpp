#include "transmissionview.h"

#include <QApplication>
#include <QIdentityProxyModel>
#include <QMetaEnum>
#include <QStyledItemDelegate>

#include "torrent.h"
#include "torrentinfo.h"
#include "torrentsmodelbase.h"
#include "torrentstatus.h"
#include "utils.h"


// Custom data roles. Default roles are described
// [here](http://doc.qt.io/qt-5/qt.html#ItemDataRole-enum).
enum CustomRole {
	ProgressRole = Qt::UserRole, // Progress as PPM
	TransferSpeedRole,           // Bytes per second of payload transfer.
	TransferSpeedEffectivityRole // Payload ratio as double in [0,1]
};

// TorrentViewDelegate is used to draw the progress bars.
class TransmissionViewDelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:
	TransmissionViewDelegate(TransmissionView *transmissionView) : QStyledItemDelegate(transmissionView) {}

	void paint(QPainter *painter, const QStyleOptionViewItem &option,
	           const QModelIndex &index) const override
	{
		QVariant data;
		if ((data = index.data(ProgressRole)).canConvert<int>()) {
			// Set up a QStyleOptionProgressBar to precisely mimic the
			// environment of a progress bar.
			QStyleOptionProgressBar progressBarOption;
			progressBarOption.state = QStyle::State_Enabled;
			progressBarOption.direction = QApplication::layoutDirection();
			progressBarOption.rect = option.rect;
			progressBarOption.fontMetrics = QApplication::fontMetrics();
			progressBarOption.minimum = 0;
			progressBarOption.maximum = 1000000; // we are using ppm
			progressBarOption.textAlignment = Qt::AlignCenter;
			progressBarOption.textVisible = true;

			// Set the progress and text values.
			int progress = data.toInt();
			progressBarOption.progress = progress;
			progressBarOption.text = tr("%L1%").arg((double) progress / 10000, 0, 'f', 2);

			// Draw the progress bar.
			// TODO Fix missing animation.
			QApplication::style()->drawControl(QStyle::CE_ProgressBar,
						&progressBarOption, painter);
		} else {
			// Not a progress. Use the default implementation.
			QStyledItemDelegate::paint(painter, option, index);
		}
	}

//	QSize sizeHint(const QStyleOptionViewItem &option,
//	               const QModelIndex &index) const override
//	{
//		if (index.data(TorrentsModelBase::PROGRESS).canConvert<int>()) {
//			// TODO
//		}
//		return QStyledItemDelegate::sizeHint(option, index);
//	}
};

// TransmissionViewProxy is used to define the columns.
class TransmissionViewProxy : public QIdentityProxyModel
{
	Q_OBJECT
public:
	TransmissionViewProxy(TransmissionView *transmissionView)
		: QIdentityProxyModel(transmissionView)
	{
		connect(this, &TransmissionViewProxy::rowsInserted,
		        this, &TransmissionViewProxy::onRowsInserted);
		connect(this, &TransmissionViewProxy::rowsAboutToBeRemoved,
		        this, &TransmissionViewProxy::onRowsAboutToBeRemoved);
		connect(this, &TransmissionViewProxy::modelReset,
		        this, &TransmissionViewProxy::onModelReset);
		connect(this, &TransmissionViewProxy::modelAboutToBeReset,
		        this, &TransmissionViewProxy::onModelAboutToBeReset);
	}

	int columnCount(const QModelIndex & = QModelIndex()) const override
	{
		// Return the amount of columns in the table.
		return 8;
	}

	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const
	{
		if (parent.isValid() || row<0 || column <0 || row>=rowCount() || column>=columnCount())
			return QModelIndex();
		return createIndex(row, column);
	}

	QVariant data(const QModelIndex &index, int role) const override
	{
		if (!index.isValid())
			return QVariant();

		// Get data form source model.
		Torrent *t = getTorrent(index.row());
		const TorrentStatus *s = t->status();

		// Return the right value for the given column and role.
		int column = index.column();
		if (t->metadata()) {
			switch (column) {
			case 0:
				switch (role) {
				case Qt::DisplayRole: return t->metadata()->name();
				case Qt::ToolTipRole: return t->metadata()->comment();
				default: return QVariant();
				}
			}
		}
		switch (column) {
		case 1:
			switch (role) {
			case Qt::DisplayRole: return QMetaEnum::fromType<TorrentStatus::State>().valueToKey(t->status()->state());
			// TODO symbols? t->status()->error()?
			default: return QVariant();
			}
		case 2:
			switch (role) {
			case Qt::DisplayRole: return tr("%L1%").arg((double) s->progressPPM() / 10000, 0, 'f', 2);
			case ProgressRole: return s->progressPPM();
			default: return QVariant();
			}
		case 3:
			switch (role) {
			case Qt::DisplayRole: return Utils::makeSpeedStr(s->downloadPayloadRate());
			case TransferSpeedRole: return s->uploadPayloadRate();
			case TransferSpeedEffectivityRole: return (double) s->uploadPayloadRate() / s->uploadRate();
			default: return QVariant();
			}
		case 4:
			switch (role) {
			case Qt::DisplayRole: return Utils::makeSpeedStr(s->uploadPayloadRate());
			case TransferSpeedRole: return s->uploadPayloadRate();
			case TransferSpeedEffectivityRole: return (double) s->uploadPayloadRate() / s->uploadRate();
			default: return QVariant();
			}
		case 5:
			switch (role) {
			case Qt::DisplayRole: return tr("%1 / %2").arg(s->peers()).arg(s->seeds());
			default: return QVariant();
			}
		case 6:
			switch (role) {
			case Qt::DisplayRole: return t->status()->savePath();
			default: return QVariant();
			}
		case 7:
			switch (role) {
			case Qt::DisplayRole: return t->status()->currentTracker();
			default: return QVariant();
			}
		}
		return QVariant();
	}

	QVariant headerData(int section, Qt::Orientation orientation, int role) const override
	{
		if (role != Qt::DisplayRole)
			return QVariant();

		if (orientation == Qt::Horizontal) {
			switch (section) {
			case 0: return tr("Name");
			case 1: return tr("State");
			case 2: return tr("Progress");
			case 3: return tr("DL speed");
			case 4: return tr("UL speed");
			case 5: return tr("Peers / Seeds");
			case 6: return tr("Save path");
			case 7: return tr("Active tracker");
			}
		}
		return QVariant();
	}

private slots:
	void onRowsInserted(const QModelIndex &parent, int first, int last)
	{
		if (parent.isValid())
			return;

		for (int row = first; row <= last; ++row) {
			Torrent *torrent = getTorrent(row);
			connect(torrent, &Torrent::statusUpdated,
			        this, &TransmissionViewProxy::onTorrentStatusUpdate);
		}
	}

	void onRowsAboutToBeRemoved(const QModelIndex &parent, int first, int last)
	{
		if (parent.isValid())
			return;

		for (int row = first; row <= last; ++row) {
			Torrent *torrent = getTorrent(row);
			disconnect(torrent, &Torrent::statusUpdated,
			           this, &TransmissionViewProxy::onTorrentStatusUpdate);
		}
	}

	void onModelReset()
	{
		// Act as if every torrent would be new.
		onRowsInserted(QModelIndex(), 0, rowCount() - 1);
	}

	void onModelAboutToBeReset()
	{
		// Act as if every torrent will be removed.
		onRowsAboutToBeRemoved(QModelIndex(), 0, rowCount() - 1);
	}

	void onTorrentStatusUpdate()
	{
		// Assertions.
		assert(dynamic_cast<Torrent*>(sender()));
		assert(dynamic_cast<TorrentsModelBase*>(sourceModel()));
		// Get torrent and underlying model.
		Torrent *t = static_cast<Torrent*>(sender());
		TorrentsModelBase *torrentsModel = static_cast<TorrentsModelBase*>(sourceModel());
		// Get the row from the underlying model and emit signal to propagate changes.
		int row = torrentsModel->rowFromTorrent(t);
		dataChanged(index(row, 1), index(row, 7));
	}

private:
	Torrent *getTorrent(int row) const
	{
		QModelIndex index = sourceModel()->index(row, 0);
		QVariant data = sourceModel()->data(index, TorrentsModelBase::TorrentRole);
		Torrent *t = data.value<Torrent*>();
		assert(t);
		return t;
	}
};

TransmissionView::TransmissionView(QWidget *parent)
	: QTableView(parent), delegate(new TransmissionViewDelegate(this))
	, proxyModel(new TransmissionViewProxy(this))
{
	// Set delegate and model for the base class.
	QTableView::setItemDelegate(delegate);
	QTableView::setModel(proxyModel);

	// Set some properties.
	setAlternatingRowColors(true);
	setSelectionBehavior(QAbstractItemView::SelectRows);
}

void TransmissionView::setModel(QAbstractItemModel *model)
{
	assert(dynamic_cast<TorrentsModelBase*>(model));
	proxyModel->setSourceModel(model);
}

#include "transmissionview.moc"
