#pragma once

#include <QSocketNotifier>

#include "IIOFileInterface.h"

namespace trikHal {
namespace trik {

class TrikIIOFile : public IIOFileInterface {
	Q_OBJECT

public:
	TrikIIOFile(const QString &fileName);

	bool open() override;
	bool close() override;
	QString fileName() const override;
	bool isOpened() const override;

private slots:
	/// Called when there is a new event in a file.
	void readFile();

private:
	int mIIOFileDescriptor {-1};
	const QString mFileName;
	QScopedPointer<QSocketNotifier> mSocketNotifier;
};

}
}
