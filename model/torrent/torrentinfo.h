#ifndef TORRENTINFO_H
#define TORRENTINFO_H

#include <boost/intrusive_ptr.hpp>

#include <QObject>
#include <QString>

namespace libtorrent {
class torrent_info;
}


class TorrentInfo : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString name    READ name)
	Q_PROPERTY(QString comment READ comment)
	Q_PROPERTY(QString creator READ creator)

public:
	explicit TorrentInfo(const libtorrent::torrent_info &data, QObject *parent = 0);
	virtual ~TorrentInfo();

	const QString name() const;
	const QString comment() const;
	const QString creator() const;

private:
	boost::intrusive_ptr<libtorrent::torrent_info> mData;

};

#endif // TORRENTINFO_H
