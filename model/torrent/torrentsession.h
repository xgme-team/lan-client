#ifndef TORRENTSESSION_H
#define TORRENTSESSION_H

#include <cstdint>
#include <map>
#include <memory>

#include <QObject>
#include <QVector>

QT_BEGIN_NAMESPACE
class QDir;
class QUrl;
QT_END_NAMESPACE
namespace libtorrent {
class alert;
class session;
class sha1_hash;
class torrent_info;
}
class Torrent;
class TorrentSessionStatus;
class TorrentsModel;


class TorrentSession : public QObject
{
	Q_OBJECT
	Q_PROPERTY(const TorrentSessionStatus* status READ status)

public:
	explicit TorrentSession(QObject *parent = 0);
	virtual ~TorrentSession();

	const TorrentSessionStatus *status() const;
	TorrentsModel *torrents() const;
	QVector<Torrent*> getTorrentsAsVector() const;

signals:
	void alert(const libtorrent::alert &alert, Torrent *torrent);
	void statusUpdated();
	void closed();

public slots:
	Torrent *addTorrent(const libtorrent::torrent_info &info,
	                    const QDir &saveDir, std::uint64_t flags = 0);
	Torrent *addTorrentMagnet(const QUrl &uri, const QDir &saveDir,
	                          std::uint64_t flags = 0);
	void removeTorrent(Torrent *torrent);
	void deleteTorrentFiles(Torrent *torrent);
	void close();

private slots:
	void update();

private:
	std::unique_ptr<libtorrent::session> mSessionHandle;
	std::map<libtorrent::sha1_hash,std::unique_ptr<Torrent>> mTorrentMap;
	TorrentSessionStatus *mStatus;
	TorrentsModel *mModel;

	int mNoUpdateCounter = 0;

};

#endif // TORRENTSESSION_H
