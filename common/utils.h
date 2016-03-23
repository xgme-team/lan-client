#ifndef UTILS_H
#define UTILS_H

#include <QCoreApplication>

QT_BEGIN_NAMESPACE
class QString;
QT_END_NAMESPACE


class Utils
{
	Q_DECLARE_TR_FUNCTIONS(Utils)

public:
	static QString makeSpeedStr(int bytesPerSecond);

private:
	Utils() = delete;

};

#endif // UTILS_H
