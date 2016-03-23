#include "messagelistmodel.h"

#include <cassert>

#include "message.h"


MessageListModel::MessageListModel(QObject *parent)
	: QAbstractListModel(parent)
{
}

Message *MessageListModel::message(int row) const
{
	if (row < 0 || row >= mMessages.length())
		return nullptr;
	else
		return mMessages[row];
}

Message *MessageListModel::message(const QModelIndex &index) const
{
	if (!index.isValid())
		return nullptr;
	else
		return message(index.row());
}

int MessageListModel::rowCount(const QModelIndex &) const
{
	return mMessages.length();
}

QVariant MessageListModel::data(const QModelIndex &index, int role) const
{
	Message *msg = message(index);
	if (msg == nullptr)
		return QVariant();

	switch (role) {
	case MessageRole:     return QVariant::fromValue(msg);
	case Qt::DisplayRole: return msg->title();
	default:              return QVariant();
	}
}

QHash<int, QByteArray> MessageListModel::roleNames() const
{
	auto roles = QAbstractListModel::roleNames();
	roles[MessageRole] = "message";
	return roles;
}

void MessageListModel::addMessage(Message *message)
{
	// Check argument and take ownership.
	assert(message);
	message->setParent(this);
	// Insert the message.
	beginInsertRows(QModelIndex(), mMessages.size(), mMessages.size());
	mMessages.push_back(message);
	endInsertRows();
	// Register handler if someone deletes the message with `delete`.
	connect(message, &Message::destroyed,
	        this, &MessageListModel::onMessageDestroyed);
}

void MessageListModel::removeMessage(Message *message)
{
	// Get the row of the message.
	int row = 0;
	while (row < mMessages.length() && mMessages[row])
		++row;
	// Check whether the message was found.
	assert(row < mMessages.length());
	// Remove the message.
	beginRemoveRows(QModelIndex(), row, row);
	mMessages.remove(row);
	endRemoveRows();
	message->deleteLater();
}

void MessageListModel::onMessageDestroyed()
{
	assert(dynamic_cast<Message*>(sender()));
	Message *msg = static_cast<Message*>(sender());
	removeMessage(msg);
}
