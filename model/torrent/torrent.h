#ifndef TORRENT_H
#define TORRENT_H

#include <memory>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <utility>

#include <QObject>

#include <libtorrent/error_code.hpp>

namespace libtorrent {
class torrent_handle;
}
class TorrentInfo;
class TorrentSession;
class TorrentStatus;


class Torrent : public QObject
{
	Q_OBJECT
	Q_PROPERTY(const TorrentStatus* status READ status)
	Q_PROPERTY(const TorrentInfo* metadata READ metadata NOTIFY metadataReceived)
	Q_PROPERTY(bool               wasAdded READ wasAdded NOTIFY added)

	friend class TorrentSession;

public:
	const TorrentStatus *status() const {return mStatus.get();}
	const TorrentInfo *metadata() const {return mMetadata.get();}
	bool wasAdded() const {return mAdded;}

	template<class T>
	std::shared_ptr<T> &at();
	template<class T>
	std::shared_ptr<T> get();
	template<class T>
	bool has() const;

signals:
	void added();
	void addFailed(const libtorrent::error_code &error);
	void removed();
	void deleted();
	void deleteFailed(const libtorrent::error_code &error);
	void metadataReceived();
	void metadataFailed(const libtorrent::error_code &error);
	void statusUpdated();

public slots:
	void remove();
	void deleteFiles();

protected:
	explicit Torrent(TorrentSession *session);
public: // I would like to make it protected, but unique_ptr have to destroy it.
	virtual ~Torrent();

private:
	TorrentSession *mSession;
	std::unique_ptr<libtorrent::torrent_handle> mHandle;

	std::unique_ptr<TorrentStatus> mStatus;
	std::unique_ptr<TorrentInfo> mMetadata;

	std::unordered_map<std::type_index,std::shared_ptr<void*>> mUserdata;

	bool mAdded = false;
	bool mRemoving = false;
	bool mDeleting = false;

};

// -----------------------------------------------------------------------------

template<class T>
std::shared_ptr<T> &Torrent::at()
{
	return std::static_pointer_cast<T>(mUserdata[typeid(T)]);
}

template<class T>
std::shared_ptr<T> Torrent::get()
{
	auto it = mUserdata.find(typeid(T));
	if (it != mUserdata.end()) {
		return std::static_pointer_cast<T>(it->second);
	}
	return std::shared_ptr<T>();
}

template<class T>
bool Torrent::has() const
{
	auto it = mUserdata.find(typeid(T));
	return (it != mUserdata.end());
}

#endif // TORRENT_H
