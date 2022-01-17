#pragma once

#include "IIOFileInterface.h"

namespace trikHal {
namespace stub {

/// Empty implementation of event file, it only logs calls to its methods and doen't emit any signals.
class StubIIOFile : public IIOFileInterface
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param fileName - file name (with path, relative or absolute) of an IIO file.
	StubIIOFile(const QString &fileName);

	bool open() override;
	bool close() override;
	QString fileName() const override;
	bool isOpened() const override;

private:
	QString mFileName;
};

}
}
