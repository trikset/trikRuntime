#pragma once

#include <QString>
#include <QObject>

namespace trikKernel {
class TimeVal;
}

namespace trikHal {

/// IIO file abstraction.
class IIOFileInterface : public QObject
{
	Q_OBJECT

public:
	/// Opens IIO file and starts listening for events.
	virtual bool open() = 0;

	/// Closes IIO file and stops listening for events.
	virtual bool close() = 0;

	/// Returns name of an event file.
	virtual QString fileName() const = 0;

	/// Returns true if a file is opened.
	virtual bool isOpened() const = 0;

signals:
	void newData(QVector<int>, const trikKernel::TimeVal &eventTime);
};
}
