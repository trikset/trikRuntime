/* Copyright 2026 CyberTech Labs Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License. */

#pragma once

#include <VideoDeviceFileInterface.h>

#include <QtCore/QString>

namespace trikHal {
namespace stub {

/// No-op video capture device for tests and non-target builds.
class StubVideoDeviceFile : public VideoDeviceFileInterface
{
	Q_OBJECT
	Q_DISABLE_COPY(StubVideoDeviceFile)
public:
	explicit StubVideoDeviceFile(const QString &fileName);

	/// Pretends to open the device.
	bool open() override;

	/// Pretends to start the capture.
	bool startStreaming() override;

	/// No-op: there is no real webcam behind the stub.
	void fixExposure() override;

	/// Pretends to stop the capture.
	void stopStreaming() override;

	/// Pretends to close the device.
	void close() override;

	/// No-op buffer release.
	void release(uint32_t bufferIdx) override;

	uint32_t actualFourcc() const override { return 0; }
	uint32_t bytesPerLine() const override { return 0; }

private:
	QString mFileName;
};

} // namespace stub
} // namespace trikHal
