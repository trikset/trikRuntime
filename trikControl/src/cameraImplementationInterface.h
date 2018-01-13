#pragma once

#include <QtCore/QScopedPointer>

#include "declSpec.h"

namespace trikControl {

class TRIKCONTROL_EXPORT CameraImplementationInterface
{
public:
	virtual QVector<uint8_t> getPhoto() = 0;

	virtual ~CameraImplementationInterface() = default;

	QString getTempDir() const { return tempDir;}

	void setTempDir(const QString &newDir) {tempDir = newDir;}

private:
	QString tempDir;

};

}
