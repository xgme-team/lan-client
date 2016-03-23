#include "torrentsessionstatus.h"

#include <libtorrent/session_status.hpp>


TorrentSessionStatus::TorrentSessionStatus(QObject *parent) :
	QObject(parent)
{
}

TorrentSessionStatus::~TorrentSessionStatus()
{
}

int TorrentSessionStatus::numPeers() const
{
	return mNumPeers;
}

int TorrentSessionStatus::downloadRate() const
{
	return mDownloadRate;
}

int TorrentSessionStatus::uploadRate() const
{
	return mUploadRate;
}

int TorrentSessionStatus::payloadDownloadRate() const
{
	return mPayloadDownloadRate;
}

int TorrentSessionStatus::payloadUploadRate() const
{
	return mPayloadUploadRate;
}

double TorrentSessionStatus::totalDownload() const
{
	return mTotalDownload;
}

double TorrentSessionStatus::totalUpload() const
{
	return mTotalUpload;
}

double TorrentSessionStatus::totalPayloadDownload() const
{
	return mTotalPayloadDownload;
}

double TorrentSessionStatus::totalPayloadUpload() const
{
	return mTotalPayloadUpload;
}

void TorrentSessionStatus::setNumPeers(int numPeers)
{
	if (mNumPeers != numPeers) {
		mNumPeers = numPeers;
		numPeersChanged();
	}
}

void TorrentSessionStatus::setDownloadRate(int downloadRate)
{
	if (mDownloadRate != downloadRate) {
		mDownloadRate = downloadRate;
		downloadRateChanged();
	}
}

void TorrentSessionStatus::setUploadRate(int uploadRate)
{
	if (mUploadRate != uploadRate) {
		mUploadRate = uploadRate;
		uploadRateChanged();
	}
}

void TorrentSessionStatus::setPayloadDownloadRate(int payloadDownloadRate)
{
	if (mPayloadDownloadRate != payloadDownloadRate) {
		mPayloadDownloadRate = payloadDownloadRate;
		payloadDownloadRateChanged();
	}
}

void TorrentSessionStatus::setPayloadUploadRate(int payloadUploadRate)
{
	if (mPayloadUploadRate != payloadUploadRate) {
		mPayloadUploadRate = payloadUploadRate;
		payloadUploadRateChanged();
	}
}

void TorrentSessionStatus::setTotalDownload(double totalDownload)
{
	if (mTotalDownload != totalDownload) {
		mTotalDownload = totalDownload;
		totalDownloadChanged();
	}
}

void TorrentSessionStatus::setTotalUpload(double totalUpload)
{
	if (mTotalUpload != totalUpload) {
		mTotalUpload = totalUpload;
		totalUploadChanged();
	}
}

void TorrentSessionStatus::setTotalPayloadDownload(double totalPayloadDownload)
{
	if (mTotalPayloadDownload != totalPayloadDownload) {
		mTotalPayloadDownload = totalPayloadDownload;
		totalPayloadDownloadChanged();
	}
}

void TorrentSessionStatus::setTotalPayloadUpload(double totalPayloadUpload)
{
	if (mTotalPayloadUpload != totalPayloadUpload) {
		mTotalPayloadUpload = totalPayloadUpload;
		totalPayloadUploadChanged();
	}
}

void TorrentSessionStatus::loadFromLibtorrent(const libtorrent::session_status &status)
{
	setNumPeers(status.num_peers);
	setDownloadRate(status.download_rate);
	setUploadRate(status.upload_rate);
	setPayloadDownloadRate(status.payload_download_rate);
	setPayloadUploadRate(status.payload_upload_rate);
	setTotalDownload(status.total_download);
	setTotalUpload(status.total_upload);
	setTotalPayloadDownload(status.total_payload_download);
	setTotalPayloadUpload(status.total_payload_upload);
}
