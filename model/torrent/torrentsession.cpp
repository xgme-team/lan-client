#include "torrentsession.h"

#include <cassert>
#include <cstdint>
#include <deque>
#include <map>
#include <utility>
#include <vector>

#include <QDir>
#include <QTimer>
#include <QUrl>

#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/alert.hpp>
#include <libtorrent/alert_types.hpp>
#include <libtorrent/error_code.hpp>
#include <libtorrent/magnet_uri.hpp>
#include <libtorrent/session.hpp>
#include <libtorrent/storage_defs.hpp>
#include <libtorrent/torrent_handle.hpp>
#include <libtorrent/torrent_info.hpp>

#include "torrent.h"
#include "torrentinfo.h"
#include "torrentsessionstatus.h"
#include "torrentsmodel.h"
#include "torrentstatus.h"

namespace lt = libtorrent;


TorrentSession::TorrentSession(QObject *parent) :
	QObject(parent),
	mSessionHandle(new lt::session(
			  lt::fingerprint("LC", 1, 0, 0, 0)
			, std::make_pair(6881, 6891)
			, "0.0.0.0"
			, lt::session::start_default_features
					| lt::session::add_default_plugins
			, lt::alert::error_notification
					| lt::alert::peer_notification
					| lt::alert::port_mapping_notification
					| lt::alert::storage_notification
					| lt::alert::tracker_notification
					| lt::alert::status_notification
					| lt::alert::ip_block_notification
					| lt::alert::performance_warning
					| lt::alert::ip_block_notification
					| lt::alert::storage_notification
			TORRENT_LOGPATH_ARG_DEFAULT)),
	mStatus(new TorrentSessionStatus(this)),
	mModel(new TorrentsModel(this, this))
{
	mSessionHandle->start_lsd();
	// TODO use prioritize partial pieces?
	// TODO use prefer whole pieces (or another threshold)?
	// TOOO block global IPs?
	// TODO set sequential download if the torrent has good availability

	mStatus->loadFromLibtorrent(mSessionHandle->status());

	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(update()));
	timer->start(100);
}

TorrentSession::~TorrentSession()
{
}

const TorrentSessionStatus *TorrentSession::status() const
{
	return mStatus;
}

TorrentsModel *TorrentSession::torrents() const
{
	return mModel;
}

/**
 * @brief Returns all torrents in this session.
 *
 * The order is undefined.
 *
 * @return All turrents in this session.
 */
QVector<Torrent *> TorrentSession::getTorrentsAsVector() const
{
	QVector<Torrent*> list(mTorrentMap.size());
	int i = 0;
	for (const auto &entry : mTorrentMap) {
		assert(i < list.size());
		list[i++] = entry.second.get();
	}
	return list;
}

/**
 * @brief Adds a new torrent to the session.
 *
 * If a torrent with the same info hash does already exists, this torrent is
 * returned. You can check what happend with Torrent::wasAdded(). It should be
 * <code>false</code> for newly added torrents.
 *
 * @param info Content of the torrent file.
 * @param savePath The directory where the file should be saved.
 * @param flags Flags which should be set for this torrent.
 * @return Returns the torrent handled by this session.
 */
Torrent *TorrentSession::addTorrent(const lt::torrent_info &info, const QDir &saveDir,
			uint64_t flags)
{
	assert(info.is_valid());
	std::unique_ptr<Torrent> &t = mTorrentMap[info.info_hash()];

	if (t) {
		// Torrent already added
		return t.get();
	} else {
		// Torrent will be added
		QString savePath = QDir::toNativeSeparators(saveDir.absolutePath());
		lt::add_torrent_params params;
		params.ti = new lt::torrent_info(info);
		params.save_path = savePath.toLocal8Bit().constData(); // TODO encoding?
		params.storage_mode = lt::storage_mode_allocate;
		params.flags = flags | lt::add_torrent_params::flag_update_subscribe; // TODO default flags?
		mSessionHandle->async_add_torrent(params);

		t.reset(new Torrent(this));
		t->mMetadata.reset(new TorrentInfo(info)); // TODO should I avoid copying lt::torrent_info?
		return t.get();
	}
}

/**
 * @brief Adds a new torrent from a magnet link to the session.
 *
 * If a torrent with the same info hash does already exists, this torrent is
 * returned. You can check what happend with Torrent::wasAdded(). It should be
 * <code>false</code> for newly added torrents.
 *
 * @param uri The magnet link to add.
 * @param saveDir The directory where the file should be saved.
 * @param flags Flags which should be set for this torrent.
 * @return Returns the torrent handled by this session or <code>nullptr</code>
 *         if the link was not valid.
 */
Torrent *TorrentSession::addTorrentMagnet(const QUrl &uri, const QDir &saveDir,
			std::uint64_t flags)
{
	lt::error_code error;
	lt::add_torrent_params params;
	std::string uriStr = uri.toString().toLocal8Bit().constData(); // TODO encoding?
	lt::parse_magnet_uri(uriStr, params, error);
	if (error) {
		return nullptr;
	}

	std::unique_ptr<Torrent> &t = mTorrentMap[params.info_hash];

	if (t) {
		// Torrent already added
		return t.get();
	} else {
		// Torrent will be added
		QString savePath = QDir::toNativeSeparators(saveDir.absolutePath());
		params.save_path = savePath.toLocal8Bit().constData(); // TODO encoding?
		params.storage_mode = lt::storage_mode_allocate;
		params.flags = flags | lt::add_torrent_params::flag_update_subscribe;
		mSessionHandle->async_add_torrent(params);

		t.reset(new Torrent(this));
		return t.get();
	}
}

/**
 * @brief Removes torrent from the session.
 *
 * After removing, Torrent::removed() will be fired.
 *
 * If you call the function after TorrentSession::removeTorrent or
 * TorrentSession::deleteTorrentFiles was already called, the behavior is
 * undefined.
 *
 * @param torrent The torrent to remove.
 * @see TorrentSession::deleteTorrentFiles Deletes files from disk, too.
 */
void TorrentSession::removeTorrent(Torrent *torrent)
{
	assert(torrent->mSession == this);
	if (torrent->wasAdded() && !torrent->mRemoving) {
		mSessionHandle->remove_torrent(*torrent->mHandle);
	}
	torrent->mRemoving = true;
}

/**
 * @brief Removes torrent from the session and deletes all files from the disk.
 *
 * After removing, Torrent::removed() will be fired. After all data is deleted,
 * Torrent::deleted() will be fired. Torrent::deleteFailed will be fired
 * instead if the files could not be deleted.
 *
 * If you call the function after TorrentSession::removeTorrent or
 * TorrentSession::deleteTorrentFiles was already called, the behavior is
 * undefined.
 *
 * @param torrent The torrent to remove.
 * @see TorrentSession::removeTorrent Does not delete the files on the disk.
 */
void TorrentSession::deleteTorrentFiles(Torrent *torrent)
{
	assert(torrent->mSession == this);
	if (torrent->wasAdded() && !torrent->mRemoving) {
		mSessionHandle->remove_torrent(*torrent->mHandle, lt::session::delete_files);
	}
	torrent->mRemoving = true;
	torrent->mDeleting = true;
}

void TorrentSession::close()
{
	// TODO
	closed();
}

void TorrentSession::update()
{
	std::deque<lt::alert*> alerts;
	mSessionHandle->pop_alerts(&alerts);

	for (const lt::alert *alert : alerts) {

		// Set the handle if a torrent was added.
		Torrent *t = nullptr;
		if (alert->type() == lt::torrent_added_alert::alert_type
				|| alert->type() == lt::add_torrent_alert::alert_type) {

			const lt::torrent_alert *a =
					static_cast<const lt::torrent_alert*>(alert);
			t = mTorrentMap[a->handle.info_hash()].get();
			assert(t);
			t->mHandle.reset(new lt::torrent_handle(a->handle));
		}

		// Update info hash in map if it changed
		if (alert->type() == lt::torrent_update_alert::alert_type) {
			const lt::torrent_update_alert *a =
					static_cast<const lt::torrent_update_alert*>(alert);
			auto &newPos = mTorrentMap[a->new_ih];
			auto it = mTorrentMap.find(a->old_ih);
			assert(newPos == nullptr);
			assert(it != mTorrentMap.end());
			newPos = std::move(it->second);
			mTorrentMap.erase(it);
		}

		// Check whether it is an torrent alert and set `t`.
		if (t == nullptr) {
			const lt::torrent_alert *ta =
					dynamic_cast<const lt::torrent_alert*>(alert);
			if (ta) {
				t = mTorrentMap[ta->handle.info_hash()].get();
				assert(t);
			}
		}

		// Fire the alert signal.
		this->alert(*alert, t);

		// Do private handling
		switch (alert->type()) {
		case lt::add_torrent_alert::alert_type:
		{
			const lt::add_torrent_alert *a =
					static_cast<const lt::add_torrent_alert*>(alert);
			assert(t);
			if (a->error) {
				t->addFailed(a->error);
				t->removed(); // TODO should I call this signal ?
				int ret = mTorrentMap.erase(a->params.ti ? a->params.ti->info_hash() : a->params.info_hash);
				assert(ret);
			} else {
				t->mAdded = true;
				t->added();
				if (t->mDeleting) {
					mSessionHandle->remove_torrent(*t->mHandle, lt::session::delete_files);
				} else if (t->mRemoving) {
					mSessionHandle->remove_torrent(*t->mHandle);
				}
			}
			break;
		}
		case lt::torrent_removed_alert::alert_type:
		{
			const lt::torrent_removed_alert *a =
					static_cast<const lt::torrent_removed_alert*>(alert);
			assert(t);
			assert(t->mHandle->info_hash() == a->info_hash);
			t->removed();
			int ret = mTorrentMap.erase(a->info_hash);
			assert(ret);
			break;
		}
		case lt::torrent_deleted_alert::alert_type:
		{
			const lt::torrent_deleted_alert *a =
					static_cast<const lt::torrent_deleted_alert*>(alert);
			assert(t);
			assert(t->mHandle->info_hash() == a->info_hash);
			t->deleted();
			break;
		}
		case lt::torrent_delete_failed_alert::alert_type:
		{
			const lt::torrent_delete_failed_alert *a =
					static_cast<const lt::torrent_delete_failed_alert*>(alert);
			assert(t);
			assert(t->mHandle->info_hash() == a->info_hash);
			t->deleteFailed(a->error);
			break;
		}
		case lt::metadata_received_alert::alert_type:
		{
			const lt::metadata_received_alert *a =
					static_cast<const lt::metadata_received_alert*>(alert);
			assert(t);
			assert(*t->mHandle == a->handle);
			t->mMetadata.reset(new TorrentInfo(*t->mHandle->torrent_file()));
			t->metadataReceived();
			break;
		}
		case lt::metadata_failed_alert::alert_type:
		{
			const lt::metadata_failed_alert *a =
					static_cast<const lt::metadata_failed_alert*>(alert);
			assert(t);
			assert(*t->mHandle == a->handle);
			t->metadataFailed(a->error);
			break;
		}
		case lt::state_update_alert::alert_type:
		{
			const lt::state_update_alert *a =
					static_cast<const lt::state_update_alert*>(alert);
			mNoUpdateCounter = 0;

			for (const lt::torrent_status &nts : a->status) {
				Torrent *t = mTorrentMap[nts.info_hash].get();
				assert(t);
				assert(nts.info_hash == nts.handle.info_hash());
				assert(*t->mHandle == nts.handle);
				t->mStatus->loadFromLibtorrent(nts);
				t->statusUpdated();
			}

			mStatus->loadFromLibtorrent(mSessionHandle->status());
			statusUpdated();
			break;
		}
		}

		delete alert;
	}

	// get torrent states every 10 updates
	if (++mNoUpdateCounter == 10) {
		mSessionHandle->post_torrent_updates();
	}
}
