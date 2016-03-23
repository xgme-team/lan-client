#ifndef TRAYICON_H
#define TRAYICON_H

#include <memory>

#include <QIcon>
#include <QSystemTrayIcon>

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
QT_END_NAMESPACE
class Application;
class Message;


class TrayIcon : public QSystemTrayIcon
{
	Q_OBJECT

public:
	explicit TrayIcon(QObject *parent = 0);
	virtual ~TrayIcon();

private slots:
	void onActivated(QSystemTrayIcon::ActivationReason reason);
	void onMessageAdded(Message *message);
	void onMessageClicked();
	void onMessageDestroyed();
	void onMessageNoticedChanged();
	void onSessionUpdate();

	void updateState();
	void updateSssionInfo();

private:
	std::unique_ptr<QMenu> mMenu;
	Message *mMessage;

	QAction *mStatusAction1;
	QAction *mStatusAction2;
	QAction *mStatusAction3;

};

#endif // TRAYICON_H
