#ifndef MESSAGE_H
#define MESSAGE_H

#include <QObject>


class Message : public QObject
{
	Q_OBJECT
	Q_PROPERTY(Level   level   READ level   CONSTANT)
	Q_PROPERTY(QString title   READ title   CONSTANT)
	Q_PROPERTY(QString text    READ text    CONSTANT)
	Q_PROPERTY(bool    noticed READ noticed WRITE setNoticed NOTIFY noticedChanged)

public:
	enum Level {
		Success,
		Failure,
		Severe
	}; Q_ENUM(Level)

	Message(Level level, QString title, QString message);

	Level level()          {return mLevel;}
	bool  noticed()        {return mNoticed;}
	const QString &title() {return mTitle;}
	const QString &text()  {return mText;}

	void setNoticed(bool noticed) {mNoticed = noticed; noticedChanged();}

signals:
	void noticedChanged();

private:
	Level mLevel;
	QString mTitle;
	QString mText;
	// TODO The GUI should have more possibilities to evaluate the message.

	bool mNoticed;

};

#endif // MESSAGE_H
