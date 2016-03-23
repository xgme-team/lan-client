#ifndef MESSAGELISTMODEL_H
#define MESSAGELISTMODEL_H

#include <Qt>
#include <QAbstractListModel>
#include <QVector>

class Message;


class MessageListModel : public QAbstractListModel
{
	Q_OBJECT

public:
	enum Role {
		MessageRole = Qt::UserRole
	}; Q_ENUM(Role)

	explicit MessageListModel(QObject *parent = 0);

	Message *message(int row) const;
	Message *message(const QModelIndex &index) const;

	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	QVariant data(const QModelIndex &index, int role) const override;
	QHash<int,QByteArray> roleNames() const override;

signals:
	void messageAdded(Message *message);

public slots:
	void addMessage(Message *message); // Takes ownership of the object
	void removeMessage(Message *message);

private slots:
	void onMessageDestroyed();

private:
	QVector<Message*> mMessages;

};

#endif // MESSAGELISTMODEL_H
