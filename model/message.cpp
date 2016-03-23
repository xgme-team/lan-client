#include "message.h"


Message::Message(Level level, QString title, QString message)
	: QObject()
	, mLevel(level)
	, mTitle(title)
	, mText(text())
	, mNoticed(false)
{
}
