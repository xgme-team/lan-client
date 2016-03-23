#ifndef TORRENTSESSIONSTATUS_H
#define TORRENTSESSIONSTATUS_H

#include <QObject>

namespace libtorrent {
struct session_status;
}


class TorrentSessionStatus : public QObject
{
	Q_OBJECT
	Q_PROPERTY(int    numPeers               READ numPeers
	           WRITE setNumPeers             NOTIFY numPeersChanged)
	Q_PROPERTY(int    downloadRate           READ downloadRate
	           WRITE setDownloadRate         NOTIFY downloadRateChanged)
	Q_PROPERTY(int    uploadRate             READ uploadRate
	           WRITE setUploadRate           NOTIFY uploadRateChanged)
	Q_PROPERTY(int    payloadDownloadRate    READ payloadDownloadRate
	           WRITE setPayloadDownloadRate  NOTIFY payloadDownloadRateChanged)
	Q_PROPERTY(int    payloadUploadRate      READ payloadUploadRate
	           WRITE setPayloadUploadRate    NOTIFY payloadUploadRateChanged)
	Q_PROPERTY(double totalDownload          READ totalDownload
	           WRITE setTotalDownload        NOTIFY totalDownloadChanged)
	Q_PROPERTY(double totalUpload            READ totalUpload
	           WRITE setTotalUpload          NOTIFY totalUploadChanged)
	Q_PROPERTY(double totalPayloadDownload   READ totalPayloadDownload
	           WRITE setTotalPayloadDownload NOTIFY totalPayloadDownloadChanged)
	Q_PROPERTY(double totalPayloadUpload     READ totalPayloadUpload
	           WRITE setTotalPayloadUpload   NOTIFY totalPayloadUploadChanged)

public:
	explicit TorrentSessionStatus(QObject *parent = 0);
	virtual ~TorrentSessionStatus();

	int numPeers() const;
	int downloadRate() const;
	int uploadRate() const;
	int payloadDownloadRate() const;
	int payloadUploadRate() const;
	double totalDownload() const;
	double totalUpload() const;
	double totalPayloadDownload() const;
	double totalPayloadUpload() const;

	void setNumPeers(int numPeers);
	void setDownloadRate(int downloadRate);
	void setUploadRate(int uploadRate);
	void setPayloadDownloadRate(int payloadDownloadRate);
	void setPayloadUploadRate(int payloadUploadRate);
	void setTotalDownload(double totalDownload);
	void setTotalUpload(double totalUpload);
	void setTotalPayloadDownload(double totalPayloadDownload);
	void setTotalPayloadUpload(double totalPayloadUpload);

	void loadFromLibtorrent(const libtorrent::session_status &status);

signals:
	void numPeersChanged();
	void downloadRateChanged();
	void uploadRateChanged();
	void payloadDownloadRateChanged();
	void payloadUploadRateChanged();
	void totalDownloadChanged();
	void totalUploadChanged();
	void totalPayloadDownloadChanged();
	void totalPayloadUploadChanged();

private:
	int mNumPeers;
	int mDownloadRate;
	int mUploadRate;
	int mPayloadDownloadRate;
	int mPayloadUploadRate;
	double mTotalDownload;
	double mTotalUpload;
	double mTotalPayloadDownload;
	double mTotalPayloadUpload;

};

#endif // TORRENTSESSIONSTATUS_H
