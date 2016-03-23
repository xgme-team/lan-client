#ifndef TORRENTSMODEL_H
#define TORRENTSMODEL_H

#include <libtorrent/alert.hpp>

#include "torrentsmodelbase.h"

class Torrent;
class TorrentSession;


//! This model is a child of TorrentsModel and contains all downloads.
class DownloadsModel : public TorrentsModelBase
{
	Q_OBJECT
	friend class TorrentsModel;

public:
	int finished() const {return mFinished;}

protected:
	int compareTorrents(Torrent *, Torrent *) const override;
	int validateTorrent(Torrent *) const override;

private slots:
	void onTorrentUpdated();

private:
	explicit DownloadsModel(QObject *parent = 0);

	int mFinished;
};

//! This model is a child of TorrentsModel and contains all uploads.
class UploadsModel : public TorrentsModelBase
{
	Q_OBJECT
	friend class TorrentsModel;

public:

protected:
	int compareTorrents(Torrent *, Torrent *) const override;
	int validateTorrent(Torrent *) const override;

private:
	explicit UploadsModel(QObject *parent = 0);
};

//! A Model which contains all torrents which are handled by the session.
class TorrentsModel : public TorrentsModelBase
{
	Q_OBJECT
	Q_PROPERTY(DownloadsModel* downloads READ downloads)
	Q_PROPERTY(UploadsModel*   uploads   READ uploads)

public:
//	enum Role {
//		TorrentRole = TorrentsModelBase
//	}; Q_ENUM(Role)

	explicit TorrentsModel(TorrentSession *session, QObject *parent = 0);

	DownloadsModel *downloads() const {return mDownloads;}
	UploadsModel   *uploads() const   {return mUploads;}

protected:
	int compareTorrents(Torrent *, Torrent *) const override;
	int validateTorrent(Torrent *) const override;

private slots:
	void onAlert(const libtorrent::alert &alert, Torrent *torrent);

private:
	DownloadsModel *mDownloads;
	UploadsModel *mUploads;

};

#endif // TORRENTSMODEL_H
