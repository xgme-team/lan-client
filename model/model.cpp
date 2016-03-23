#include "model.h"

#include "librarymodel.h"
#include "messagelistmodel.h"
#include "torrentsession.h"


Model::Model(QObject *parent)
	: QObject(parent)
	, mLibrary(new LibraryModel(this))
	, mMessages(new MessageListModel(this))
	, mSession(new TorrentSession(this))
{
}
