#include "torrentstatus.h"

#include <cassert>

#include <libtorrent/torrent_handle.hpp>

namespace lt = libtorrent;
static TorrentStatus::State stateFromLibtorrent(lt::torrent_status::state_t state);


TorrentStatus::TorrentStatus(QObject *parent) :
	QObject(parent),
	mState(ADDING),
	mProgressPPM(0),
	mDownloadRate(0),
	mDownloadPayloadRate(0),
	mUploadRate(0),
	mUploadPayloadRate(0)
{
}

TorrentStatus::~TorrentStatus()
{
}

void TorrentStatus::setName(const QString &name)
{
	if (mName != name) {
		mName = name;
		nameChanged();
	}
}

void TorrentStatus::setSavePath(const QString &savePath)
{
	if (mSavePath != savePath) {
		mSavePath = savePath;
		savePathChanged();
	}
}

void TorrentStatus::setError(const QString &error)
{
	if (mError != error) {
		mError = error;
		errorChanged();
	}
}

void TorrentStatus::setState(TorrentStatus::State state)
{
	if (mState != state) {
		mState = state;
		stateChanged();
	}
}

void TorrentStatus::setProgressPPM(int progressPPM)
{
	if (mProgressPPM != progressPPM) {
		mProgressPPM = progressPPM;
		progressPPMChanged();
	}
}

void TorrentStatus::setAddedTime(const QDateTime &addedTime)
{
	if (mAddedTime != addedTime) {
		mAddedTime = addedTime;
		addedTimeChanged();
	}
}

void TorrentStatus::setCompletedTime(const QDateTime &completedTime)
{
	if (mCompletedTime != completedTime) {
		mCompletedTime = completedTime;
		completedTimeChanged();
	}
}

void TorrentStatus::setDownloadRate(int downloadRate)
{
	if (mDownloadRate != downloadRate) {
		mDownloadRate = downloadRate;
		downloadRateChanged();
	}
}

void TorrentStatus::setDownloadPayloadRate(int downloadPayloadRate)
{
	if (mDownloadPayloadRate != downloadPayloadRate) {
		mDownloadPayloadRate = downloadPayloadRate;
		downloadPayloadRateChanged();
	}
}

void TorrentStatus::setUploadRate(int uploadRate)
{
	if (mUploadRate != uploadRate) {
		mUploadRate = uploadRate;
		uploadRateChanged();
	}
}

void TorrentStatus::setUploadPayloadRate(int uploadPayloadRate)
{
	if (mUploadPayloadRate != uploadPayloadRate) {
		mUploadPayloadRate = uploadPayloadRate;
		uploadPayloadRateChanged();
	}
}

void TorrentStatus::setPeers(int peers)
{
	if (mPeers != peers) {
		mPeers = peers;
		peersChanged();
	}
}

void TorrentStatus::setSeeds(int seeds)
{
	if (mSeeds != seeds) {
		mSeeds = seeds;
		seedsChanged();
	}
}

void TorrentStatus::setUploads(int uploads)
{
	if (mUploads != uploads) {
		mUploads = uploads;
		uploadsChanged();
	}
}

void TorrentStatus::setQueuePosition(int queuePosition)
{
	if (mQueuePosition != queuePosition) {
		mQueuePosition = queuePosition;
		queuePositionChanged();
	}
}

void TorrentStatus::setCurrentTracker(const QString &currentTracker)
{
	if (mCurrentTracker != currentTracker) {
		mCurrentTracker = currentTracker;
		currentTrackerChanged();
	}
}

void TorrentStatus::loadFromLibtorrent(const lt::torrent_status &status)
{
	setName(QString::fromStdString(status.name));
	setSavePath(QString::fromStdString(status.save_path));
	setError(QString::fromStdString(status.error));
	setState(stateFromLibtorrent(status.state));
	setProgressPPM(status.progress_ppm);
	setAddedTime(QDateTime::fromTime_t(status.added_time));
	setCompletedTime(QDateTime::fromTime_t(status.completed_time));
	setDownloadRate(status.download_rate);
	setDownloadPayloadRate(status.download_payload_rate);
	setUploadRate(status.upload_rate);
	setUploadPayloadRate(status.upload_payload_rate);
	setPeers(status.num_peers);
	setSeeds(status.num_seeds);
	setUploads(status.num_uploads);
	setQueuePosition(status.queue_position);
	setCurrentTracker(QString::fromStdString(status.current_tracker));
}

TorrentStatus::State stateFromLibtorrent(lt::torrent_status::state_t state)
{
	switch (state) {
	case lt::torrent_status::queued_for_checking:
		return TorrentStatus::QUEUED_FOR_CHECKING;
	case lt::torrent_status::checking_files:
		return TorrentStatus::CHECKING_FILES;
	case lt::torrent_status::downloading_metadata:
		return TorrentStatus::DOWNLOADING_METADATA;
	case lt::torrent_status::downloading:
		return TorrentStatus::DOWNLOADING;
	case lt::torrent_status::finished:
		return TorrentStatus::FINISHED;
	case lt::torrent_status::seeding:
		return TorrentStatus::SEEDING;
	case lt::torrent_status::allocating:
		return TorrentStatus::ALLOCATING;
	case lt::torrent_status::checking_resume_data:
		return TorrentStatus::CHECKING_RESUME_DATA;
	default:
		assert(false);
		return TorrentStatus::FINISHED; // Omit warning
	}
}
