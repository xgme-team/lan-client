#include "trayicon.h"

#include <cassert>

#include <QAction>
#include <QIcon>
#include <QMenu>

#include "application.h"
#include "message.h"
#include "messagelistmodel.h"
#include "model.h"
#include "torrentsession.h"
#include "torrentsessionstatus.h"
#include "torrentsmodel.h"
#include "utils.h"


TrayIcon::TrayIcon(QObject *parent)
	: QSystemTrayIcon(myApp->iconDefault(), parent)
	, mMessage(nullptr)
{
	// Get instance of Application.
	Application *app = myApp;
	// Set properties.
	updateState();

	// Setup context menu.
	mMenu.reset(new QMenu(QStringLiteral("XGME LAN-Client")));
	mMenu->addAction(app->openTorrentAction());
	mMenu->addAction(app->newTorrentFromFileAction());
	mMenu->addAction(app->newTorrentFromDirAction());
	mMenu->addSeparator();
	mStatusAction1 = mMenu->addAction(QStringLiteral(""));
	mStatusAction2 = mMenu->addAction(QStringLiteral(""));
	mStatusAction3 = mMenu->addAction(QStringLiteral(""));
	mMenu->addSeparator();
	mMenu->addAction(app->exitAction());
	setContextMenu(mMenu.get());

	mStatusAction1->setEnabled(false);
	mStatusAction2->setEnabled(false);
	mStatusAction3->setEnabled(false);

	// Register event handler.
	connect(this, &TrayIcon::activated,
	        this, &TrayIcon::onActivated);
	connect(app->model()->messages(), &MessageListModel::messageAdded,
	        this, &TrayIcon::onMessageAdded);
	connect(this, &TrayIcon::messageClicked,
	        this, &TrayIcon::onMessageClicked);
	// Update information in context menu when the user opens it.
	connect(mMenu.get(), &QMenu::aboutToShow,
	        this, &TrayIcon::updateSssionInfo);

	// Show tray icon.
	this->show();
}

TrayIcon::~TrayIcon()
{
}

void TrayIcon::onActivated(QSystemTrayIcon::ActivationReason reason)
{
	switch (reason) {
	case QSystemTrayIcon::DoubleClick:
	case QSystemTrayIcon::Trigger:
		// Open the window if the user clicks on the icon.
		myApp->showWindowAction()->trigger();
		break;
	default:
		// Do nothing for all other activation reasons.
		break;
	}
}

void TrayIcon::onMessageAdded(Message *message)
{
	assert(!message->noticed());
	// Send notification.
	// What about games? Can I delay it if there is a fullscreen application?
	QSystemTrayIcon::MessageIcon msgIcon;
	switch (message->level()) {
	case Message::Success: msgIcon = QSystemTrayIcon::Information; break;
	case Message::Failure: msgIcon = QSystemTrayIcon::Warning;     break;
	case Message::Severe:  msgIcon = QSystemTrayIcon::Critical;    break;
	default: msgIcon = QSystemTrayIcon::NoIcon;
	}
	showMessage(message->title(), message->text(), msgIcon);

	// Do not change the state just because a single task is successful.
	// TODO add special case if all downloads are complete.
	if (message->level() == Message::Success)
		return;

	// Update state to consider the message.
	mMessage = message;
	updateState();
	// Connect handler to get updates about the message.
	connect(message, &Message::destroyed,
	        this, &TrayIcon::onMessageDestroyed);
	connect(message, &Message::noticedChanged,
	        this, &TrayIcon::onMessageNoticedChanged);
}

void TrayIcon::onMessageClicked()
{
	myApp->showWindowAction()->trigger();
}

void TrayIcon::onMessageDestroyed()
{
	assert(dynamic_cast<Message*>(sender()));
	Message *msg = static_cast<Message*>(sender());
	// Update state.
	if (mMessage == msg) {
		mMessage = nullptr;
		updateState();
	}
}

void TrayIcon::onMessageNoticedChanged()
{
	assert(dynamic_cast<Message*>(sender()));
	Message *msg = static_cast<Message*>(sender());
	// Update state.
	if (msg->noticed() && mMessage == msg) {
		mMessage = nullptr;
		updateState();
	}
	// Remove handler since it is not needed anymore.
	disconnect(msg, &Message::noticedChanged,
	           this, &TrayIcon::onMessageNoticedChanged);
}

void TrayIcon::onSessionUpdate()
{
	if (mMenu->isVisible())
		updateSssionInfo();
}

void TrayIcon::updateState()
{
	if (mMessage == nullptr) {
		// Setup default state.
		setToolTip(QStringLiteral("XGME LAN-Client"));
		setIcon(myApp->iconDefault());
	} else {
		// Setup state for the given message.
		setToolTip(mMessage->title());
		switch (mMessage->level()) {
		case Message::Success: setIcon(myApp->iconSuccess()); break;
		case Message::Failure: setIcon(myApp->iconFailure()); break;
		case Message::Severe:  setIcon(myApp->iconSevere());  break;
		default: setIcon(myApp->iconDefault());
		}
	}
}

void TrayIcon::updateSssionInfo()
{
	const TorrentSessionStatus *s = myApp->model()->session()->status();
	const TorrentsModel *torrents = myApp->model()->session()->torrents();

	// TODO While initialization: tr("Initializing ...");

	if (torrents->downloads()->length() == 0) {
		int uploads = torrents->uploads()->length();
		mStatusAction1->setText(tr("No download queued."));
		if (uploads == 0) {
			mStatusAction2->setText(tr("No upload running."));
		} else {
			mStatusAction2->setText(tr("%n upload(s) with %1.", "", uploads).arg(
						Utils::makeSpeedStr(s->payloadUploadRate())));
		}

		mStatusAction2->setVisible(true);
		mStatusAction3->setVisible(false);
	} else {
		int running = torrents->downloads()->length();
		int finished = torrents->downloads()->finished();
		// Add progress (percent) to first line.
		mStatusAction1->setText(tr("%n of %1 download(s) finished.", "", finished).arg(running));
		// TODO Required time for next download.
		mStatusAction3->setText(tr("DL: %1, UL: %2").arg(
					Utils::makeSpeedStr(s->payloadDownloadRate()),
					Utils::makeSpeedStr(s->payloadUploadRate())));

		mStatusAction2->setVisible(false); // TODO make it visible
		mStatusAction3->setVisible(true);
	}

	mMenu->update();
}
