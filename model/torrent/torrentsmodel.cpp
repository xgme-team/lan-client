#include "torrentsmodel.h"

#include <cassert>
//#include <cstdint>
//#include <memory>
//#include <utility>

#include <QVector>

#include <libtorrent/alert.hpp>
#include <libtorrent/alert_types.hpp>

#include "torrent.h"
#include "torrentinfo.h"
#include "torrentsession.h"
#include "torrentstatus.h"

namespace lt = libtorrent;


TorrentsModel::TorrentsModel(TorrentSession *session, QObject *parent)
	: TorrentsModelBase(parent)
	, mDownloads(new DownloadsModel(this))
	, mUploads(new UploadsModel(this))
{
	// Add added torrents of the session to the model.
	for (Torrent *torrent : session->getTorrentsAsVector()) {
		if (torrent->wasAdded()) {
			trackTorrent(torrent);
			mDownloads->trackTorrent(torrent);
			mUploads->trackTorrent(torrent);
		}
	}
	// Connect to session stay up to date.
	connect(session, &TorrentSession::alert,
	        this, &TorrentsModel::onAlert);
}

int TorrentsModel::compareTorrents(Torrent *, Torrent *) const
{
	return 0;
}

int TorrentsModel::validateTorrent(Torrent *) const
{
	return AcceptTorrent;
}

void TorrentsModel::onAlert(const libtorrent::alert &alert, Torrent *torrent)
{
	switch (alert.type()) {
	case lt::torrent_added_alert::alert_type:
	{
		assert(torrent);
//		const lt::torrent_added_alert &a =
//				static_cast<const lt::torrent_added_alert&>(alert);

		trackTorrent(torrent);
		mDownloads->trackTorrent(torrent);
		mUploads->trackTorrent(torrent);
		break;
	}
	case lt::torrent_removed_alert::alert_type:
	{
		assert(torrent);
//		const lt::torrent_removed_alert &a =
//				static_cast<const lt::torrent_removed_alert&>(alert);

		mUploads->untrackTorrent(torrent);
		mDownloads->untrackTorrent(torrent);
		untrackTorrent(torrent);
		break;
	}
	}
}

DownloadsModel::DownloadsModel(QObject *parent)
	: TorrentsModelBase(parent)
	, mFinished(0)
{
	connect(this, &DownloadsModel::torrentUpdated,
	        this, &DownloadsModel::onTorrentUpdated);
}

int DownloadsModel::compareTorrents(Torrent *t1, Torrent *t2) const
{
	return t1->status()->queuePosition() - t2->status()->queuePosition();
}

int DownloadsModel::validateTorrent(Torrent *torrent) const
{
	switch (torrent->status()->state()) {
	case TorrentStatus::ADDING:
	case TorrentStatus::FINISHED:
	case TorrentStatus::SEEDING:
	case TorrentStatus::CHECKING_RESUME_DATA:
		return KeepTorrent;
	default:
		return AcceptTorrent;
	}
}

void DownloadsModel::onTorrentUpdated()
{
	// TODO Update mFinished.
}

UploadsModel::UploadsModel(QObject *parent)
	: TorrentsModelBase(parent)
{
}

int UploadsModel::compareTorrents(Torrent *, Torrent *) const
{
	return 0;
}

int UploadsModel::validateTorrent(Torrent *torrent) const
{
	if (torrent->status()->uploads() > 0)
		return AcceptTorrent;
	else
		return RemoveTorrent;
}
