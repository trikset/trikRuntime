#include "sensor.h"

#include <QtCore/QTextStream>

#include <QtCore/QDebug>

using namespace trikControl;

Sensor::Sensor()
	: mMin(0)
	, mMax(0)
{
}

void Sensor::init(int min, int max, QString const &controlFile)
{
	mMin = min;
	mMax = max;
	mControlFile.setFileName(controlFile);
}

int Sensor::read()
{
	if (mMax == mMin) {
		return mMin;
	}

	if (mControlFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QTextStream stream(&mControlFile);
		int value = 0;
		stream >> value;
		mControlFile.close();

		qDebug() << "read, raw reading: " << value;

		value = qMin(value, mMax);
		value = qMax(value, mMin);

		double const scale = 100.0 / (static_cast<double>(mMax) - static_cast<double>(mMin));

		qDebug() << "read, scale: " << scale << "mMin" << mMin << "mMax" << mMax;

		value = (value - mMin) * scale;

		qDebug() << "read, normalized reading: " << value;

		return value;
	}

	qDebug() << "read, reading failed, file name: " << mControlFile.fileName();

	return 0;
}
