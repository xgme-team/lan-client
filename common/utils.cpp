#include "utils.h"

#include <cmath>

#include <QString>


QString Utils::makeSpeedStr(int bytesPerSecond)
{
	if (bytesPerSecond < 900) {
		return tr("%1 B/s").arg(bytesPerSecond, 4);
	}

	double rate;
	QString translation;
	if (bytesPerSecond < 1024 * 900) {
		rate = (double) bytesPerSecond / 1024;
		translation = tr("%1 KiB/s");
	} else if (bytesPerSecond < 1024 * 1024 * 900) {
		rate = (double) bytesPerSecond / (1024 * 1024);
		translation = tr("%1 MiB/s");
	} else {
		rate = (double) bytesPerSecond / (1024 * 1024 * 1024);
		translation = tr("%1 GiB/s");
	}

	if (rate < 10.0)
		return translation.arg(rate, 4, 'f', 2);
	else if (rate < 100.0)
		return translation.arg(rate, 4, 'f', 1);
	else
		return translation.arg(rate, 4, 'f', 0);
}
