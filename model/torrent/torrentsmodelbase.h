#ifndef TORRENTSMODELBASE_H
#define TORRENTSMODELBASE_H

#include <map>

#include <Qt>
#include <QAbstractListModel>
#include <QList>

class Torrent;


class TorrentsModelBase : public QAbstractListModel
{
	Q_OBJECT
	Q_PROPERTY(int length READ length NOTIFY lengthChanged STORED false)

public:
	enum Role {
		TorrentRole = Qt::UserRole
	}; Q_ENUM(Role)

	explicit TorrentsModelBase(QObject *parent = 0);

	int rowFromTorrent(const Torrent *torrent) const;
	Torrent *torrentFromRow(int row) const;
	//const QVector<Torrent*> &asVector() const {return mTorrentList;}

	int length() const {return mTorrentList.length();}

	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	QVariant data(const QModelIndex &index, int role) const override;
	QHash<int,QByteArray> roleNames() const override;

signals:
	void lengthChanged();
	void torrentUpdated(Torrent *torrent);

protected:
	enum ValidationResult {
		RemoveTorrent,
		KeepTorrent,
		AcceptTorrent
	};

	virtual int compareTorrents(Torrent *t1, Torrent *t2) const = 0;
	virtual int validateTorrent(Torrent *torrent) const = 0;

protected slots:
	void trackTorrent(Torrent *torrent);
	void untrackTorrent(Torrent *torrent);

private slots:
	void onTorrentMetadataUpdated();
	void onTorrentStatusUpdated();

private:
	void addTorrent(Torrent *torrent);
	void removeTorrent(Torrent *torrent);
	void handleTorrentUpdate(Torrent *torrent, bool metadata);
	void registerHandler(Torrent *torrent);
	void unregisterHandler(Torrent *torrent);
	void updateRowNumbers(int firstRow, int addition, int lastRow = -1);

	QList<Torrent*> mTorrentList;
	std::map<const Torrent*,int> mTorrentMap;

};

#endif // TORRENTSMODELBASE_H
