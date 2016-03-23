#ifndef MODEL_H
#define MODEL_H

#include <QObject>

class LibraryModel;
class MessageListModel;
class TorrentSession;


class Model : public QObject
{
	Q_OBJECT
	Q_PROPERTY(LibraryModel*     library  READ library)
	Q_PROPERTY(MessageListModel* messages READ messages)
	Q_PROPERTY(TorrentSession*   session  READ session)

public:
	explicit Model(QObject *parent = 0);

	LibraryModel     *library()  {return mLibrary;}
	MessageListModel *messages() {return mMessages;}
	TorrentSession   *session()  {return mSession;}

private:
	Model(const Model &) = delete;
	Model &operator=(const Model &) = delete;

	LibraryModel *mLibrary;
	MessageListModel *mMessages;
	TorrentSession *mSession;

};

#endif // MODEL_H
