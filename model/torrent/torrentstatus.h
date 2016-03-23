#ifndef TORRENTSTATUS_H
#define TORRENTSTATUS_H

#include <QDateTime>
#include <QObject>
#include <QString>

namespace libtorrent {
struct torrent_status;
}


class TorrentStatus : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString   name                READ name
	           WRITE  setName                NOTIFY nameChanged)
	Q_PROPERTY(QString   savePath            READ savePath
	           WRITE  setSavePath            NOTIFY savePathChanged)
	Q_PROPERTY(QString   error               READ error
	           WRITE  setError               NOTIFY errorChanged)
	Q_PROPERTY(State     state               READ state
	           WRITE  setState               NOTIFY stateChanged)
	Q_PROPERTY(int       progressPPM         READ progressPPM
	           WRITE  setProgressPPM         NOTIFY progressPPMChanged)
	Q_PROPERTY(QDateTime addedTime           READ addedTime
	           WRITE  setAddedTime           NOTIFY addedTimeChanged)
	Q_PROPERTY(QDateTime completedTime       READ completedTime
	           WRITE  setCompletedTime       NOTIFY completedTimeChanged)
	Q_PROPERTY(int       downloadRate        READ downloadRate
	           WRITE  setDownloadRate        NOTIFY downloadRateChanged)
	Q_PROPERTY(int       downloadPayloadRate READ downloadPayloadRate
	           WRITE  setDownloadPayloadRate NOTIFY downloadPayloadRateChanged)
	Q_PROPERTY(int       uploadRate          READ uploadRate
	           WRITE  setUploadRate          NOTIFY uploadRateChanged)
	Q_PROPERTY(int       uploadPayloadRate   READ uploadPayloadRate
	           WRITE  setUploadPayloadRate   NOTIFY uploadPayloadRateChanged)
	Q_PROPERTY(int       peers               READ peers
	           WRITE  setPeers               NOTIFY peersChanged)
	Q_PROPERTY(int       seeds               READ seeds
	           WRITE  setSeeds               NOTIFY seedsChanged)
	Q_PROPERTY(int       uploads             READ uploads
	           WRITE setUploads              NOTIFY uploadsChanged)
	Q_PROPERTY(int       queuePosition       READ queuePosition
	           WRITE setQueuePosition        NOTIFY queuePositionChanged)
	Q_PROPERTY(QString   currentTracker      READ currentTracker
	           WRITE  setCurrentTracker      NOTIFY currentTrackerChanged)

public:
	enum State {
		ADDING,
		QUEUED_FOR_CHECKING,
		CHECKING_FILES,
		DOWNLOADING_METADATA,
		DOWNLOADING,
		FINISHED,
		SEEDING,
		ALLOCATING,
		CHECKING_RESUME_DATA
	}; Q_ENUM(State)

	explicit TorrentStatus(QObject *parent = 0);
	virtual ~TorrentStatus();

	const QString &name() const {return mName;}
	const QString &savePath() const {return mSavePath;}
	const QString &error() const {return mError;}
	State state() const {return mState;}
	int progressPPM() const {return mProgressPPM;}
	const QDateTime &addedTime() const {return mAddedTime;}
	const QDateTime &completedTime() const {return mCompletedTime;}
	int downloadRate() const {return mDownloadRate;}
	int downloadPayloadRate() const {return mDownloadPayloadRate;}
	int uploadRate() const {return mUploadRate;}
	int uploadPayloadRate() const {return mUploadPayloadRate;}
	int peers() const {return mPeers;}
	int seeds() const {return mSeeds;}
	int uploads() const {return mUploads;}
	int queuePosition() const {return mQueuePosition;}
	const QString &currentTracker() const {return mCurrentTracker;}

	void setName(const QString &name);
	void setSavePath(const QString &savePath);
	void setError(const QString &error);
	void setState(State state);
	void setProgressPPM(int progressPPM);
	void setAddedTime(const QDateTime &addedTime);
	void setCompletedTime(const QDateTime &completedTime);
	void setDownloadRate(int downloadRate);
	void setDownloadPayloadRate(int downloadPayloadRate);
	void setUploadRate(int uploadRate);
	void setUploadPayloadRate(int uploadPayloadRate);
	void setPeers(int peers);
	void setSeeds(int seeds);
	void setUploads(int uploads);
	void setQueuePosition(int queuePosition);
	void setCurrentTracker(const QString &currentTracker);

	void loadFromLibtorrent(const libtorrent::torrent_status &status);

signals:
	void nameChanged();
	void savePathChanged();
	void errorChanged();
	void stateChanged();
	void progressPPMChanged();
	void addedTimeChanged();
	void completedTimeChanged();
	void downloadRateChanged();
	void downloadPayloadRateChanged();
	void uploadRateChanged();
	void uploadPayloadRateChanged();
	void peersChanged();
	void seedsChanged();
	void uploadsChanged();
	void queuePositionChanged();
	void currentTrackerChanged();

private:
	QString mName;
	QString mSavePath;
	QString mError;
	State mState;
	int mProgressPPM;
	QDateTime mAddedTime;
	QDateTime mCompletedTime;
	int mDownloadRate;
	int mDownloadPayloadRate;
	int mUploadRate;
	int mUploadPayloadRate;
	int mPeers;
	int mSeeds;
	int mUploads;
	int mQueuePosition;
	QString mCurrentTracker;

};

#endif // TORRENTSTATUS_H
