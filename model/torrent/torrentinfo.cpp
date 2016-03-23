#include "torrentinfo.h"

#include <libtorrent/torrent_info.hpp>

namespace lt = libtorrent;


TorrentInfo::TorrentInfo(const libtorrent::torrent_info &data, QObject *parent) :
	QObject(parent),
	mData(new lt::torrent_info(data))
{
}

TorrentInfo::~TorrentInfo()
{
}

const QString TorrentInfo::name() const
{
	return QString::fromLocal8Bit(mData->name().c_str());
}

const QString TorrentInfo::comment() const
{
	return QString::fromLocal8Bit(mData->comment().c_str());
}

const QString TorrentInfo::creator() const
{
	return QString::fromLocal8Bit(mData->creator().c_str());
}
