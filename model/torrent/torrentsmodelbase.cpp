#include "torrentsmodelbase.h"

#include <cassert>

#include <QMetaEnum>

#include "torrent.h"
#include "torrentinfo.h"


TorrentsModelBase::TorrentsModelBase(QObject *parent)
	: QAbstractListModel(parent)
{
}

int TorrentsModelBase::rowFromTorrent(const Torrent *torrent) const
{
	auto it = mTorrentMap.find(torrent);
	assert(it != mTorrentMap.end());
	return it->second;
}

Torrent *TorrentsModelBase::torrentFromRow(int row) const
{
	if (row >= 0 && row < mTorrentList.length())
		return nullptr;
	else
		return mTorrentList[row];
}

int TorrentsModelBase::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid())
		return 0;
	else
		return mTorrentList.size();
}

QVariant TorrentsModelBase::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();
	if (index.row() >= mTorrentList.size())
		return QVariant();

	Torrent *t = mTorrentList[index.row()];
	switch (role) {
	case TorrentRole:     return QVariant::fromValue(t);
	case Qt::DisplayRole: return t->metadata() ? t->metadata()->name()    : QVariant();
	case Qt::ToolTipRole: return t->metadata() ? t->metadata()->comment() : QVariant();
	default:              return QVariant();
	}
}

QHash<int, QByteArray> TorrentsModelBase::roleNames() const
{
	auto roles = QAbstractListModel::roleNames();
	roles[TorrentRole] = "torrent";
	return roles;
}

void TorrentsModelBase::trackTorrent(Torrent *torrent)
{
	// Ensure that you are not adding torrents which are already in the model.
	assert(mTorrentMap.find(torrent) == mTorrentMap.end());
	// Register handlers to stay up to date. Even if we do not add the torrent.
	// Maybe we want add it later.
	registerHandler(torrent);
	// Check if we should add the torrent to the model.
	if (validateTorrent(torrent) == AcceptTorrent)
		addTorrent(torrent);
}

void TorrentsModelBase::untrackTorrent(Torrent *torrent)
{
	// Unregister handler.
	unregisterHandler(torrent);
	removeTorrent(torrent);
}

void TorrentsModelBase::onTorrentMetadataUpdated()
{
	// Get the updated torrent.
	assert(dynamic_cast<Torrent*>(sender()));
	Torrent *t = static_cast<Torrent*>(sender());
	// Handle update.
	handleTorrentUpdate(t, true);
}

void TorrentsModelBase::onTorrentStatusUpdated()
{
	// Get the updated torrent.
	assert(dynamic_cast<Torrent*>(sender()));
	Torrent *t = static_cast<Torrent*>(sender());
	// Handle update.
	handleTorrentUpdate(t, false);
}

void TorrentsModelBase::addTorrent(Torrent *torrent)
{
	// Find row where the torrent should be inserted.
	int row = mTorrentList.size();
	while (row > 0 && compareTorrents(mTorrentList[row - 1], torrent) > 0)
		--row;
	// Inform listeners that we will add a torrent.
	beginInsertRows(QModelIndex(), row, row);
	// Remove the torrent to the list.
	mTorrentList.insert(row, torrent);
	// Update the map.
	mTorrentMap.emplace(torrent, row);
	updateRowNumbers(row + 1, +1);
	// Inform listeners that we have finished the process.
	lengthChanged();
	endInsertRows();
}

void TorrentsModelBase::removeTorrent(Torrent *torrent)
{
	// Check whether the torrent is part of the model. Do nothing if not.
	auto it = mTorrentMap.find(torrent);
	if (it != mTorrentMap.end()) {
		// It is part of the model. We have to remove it.
		// Get the row of the torrent.
		const std::size_t i = it->second;
		// Inform listeners that we will delete a torrent.
		beginRemoveRows(QModelIndex(), i, i);
		// Remove the torrent from the list and map.
		mTorrentMap.erase(it);
		mTorrentList.erase(mTorrentList.begin() + i);
		// Update the map. Every torrent after the removed one has another row
		// number now.
		updateRowNumbers(i, -1);
		// Inform listeners that we have finished the process.
		lengthChanged();
		endRemoveRows();
	}
}

void TorrentsModelBase::handleTorrentUpdate(Torrent *torrent, bool metadata)
{
	const int lastRow = mTorrentList.length() - 1;
	// Get the position of the torrent in the map.
	const auto it = mTorrentMap.find(torrent);
	// Maybe we have not added the torrent yet. Check if we should add it now.
	if (it == mTorrentMap.end()) {
		if (validateTorrent(torrent) == AcceptTorrent)
			addTorrent(torrent);
		return;
	}
	// Check whether we should keep the torrent in the model.
	if (validateTorrent(torrent) == RemoveTorrent) {
		removeTorrent(torrent);
		return; // Nothing to do since the torrent was removed.
	}
	// Get the row of the torrent.
	int row = it->second;
	// Emit signal to propagate changes.
	if (metadata)
		dataChanged(index(row, 0), index(row, 0), {Qt::DisplayRole, Qt::ToolTipRole});
	torrentUpdated(torrent);
	// Search for new row position to keep the model sorted.
	int newRow = row;
	while (newRow > 0 && compareTorrents(mTorrentList[newRow - 1], torrent) > 0)
		--newRow;
	while (newRow < lastRow && compareTorrents(mTorrentList[newRow + 1], torrent) < 0)
		++newRow;
	// Move row if it is required.
	if (newRow != row) {
		// Inform listeners that we will move a torrent.
		beginMoveRows(QModelIndex(), row, row, QModelIndex(), newRow);
		// Move the torrent in the list.
		mTorrentList.move(row, newRow);
		// Update the map. Every torrent between row and newRow has another row
		// number now.
		if (newRow < row)
			updateRowNumbers(newRow, +1, row - 1);
		else
			updateRowNumbers(row + 1, -1, newRow);
		it->second = newRow;
		// Inform listeners that we have finished the process.
		endMoveRows();
	}
}

void TorrentsModelBase::registerHandler(Torrent *torrent)
{
	connect(torrent, &Torrent::metadataReceived,
	        this, &TorrentsModelBase::onTorrentMetadataUpdated);
	connect(torrent, &Torrent::statusUpdated,
	        this, &TorrentsModelBase::onTorrentStatusUpdated);
}

void TorrentsModelBase::unregisterHandler(Torrent *torrent)
{
	disconnect(torrent, &Torrent::metadataReceived,
	           this, &TorrentsModelBase::onTorrentStatusUpdated);
	disconnect(torrent, &Torrent::metadataReceived,
	           this, &TorrentsModelBase::onTorrentStatusUpdated);
}

void TorrentsModelBase::updateRowNumbers(int firstRow, int addition, int lastRow)
{
	lastRow = lastRow < 0 ? length() : lastRow;
	for (auto &entry : mTorrentMap) {
		if (entry.second >= firstRow && entry.second <= lastRow) {
			entry.second += addition;
		}
	}
}
