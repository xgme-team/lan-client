#include "torrent.h"

#include <libtorrent/torrent_handle.hpp>

#include "torrentinfo.h"
#include "torrentsession.h"
#include "torrentstatus.h"


void Torrent::remove()
{
	mSession->removeTorrent(this);
}

void Torrent::deleteFiles()
{
	mSession->deleteTorrentFiles(this);
}

Torrent::Torrent(TorrentSession *session) :
	QObject(session),
	mSession(session),
	mStatus(new TorrentStatus(this))
{
}

Torrent::~Torrent()
{
}
