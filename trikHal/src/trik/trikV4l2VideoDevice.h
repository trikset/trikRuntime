/* Copyright 2018 Ivan Tyulyandin and CyberTech Labs Ltd.
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

#include <QObject>
#include <QSocketNotifier>
#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtCore/QSocketNotifier>
#include <linux/videodev2.h>

using convertFunctionPtr = QVector<uint8_t> (*)(const QVector<uint8_t> &realCameraShot, int height, int width);

/// Class for working with a camera on a TRIK controller
class TrikV4l2VideoDevice: public QObject
{
	Q_OBJECT
public:

	/// TRIK v4l2 video device constructor
	/// @param inputFile - camera device name
	explicit TrikV4l2VideoDevice(const QString &inputFile);

	~TrikV4l2VideoDevice();

	/// Make photo using TRIK camera
	const QVector<uint8_t> & makeShot();

	/// Get last frame
	const QVector<uint8_t> & getFrame() const { return mFrame; }

signals:
	/// Signal when photo was made
	void dataReady();

public slots:
	/// Read data from v4l2 buffers
	/// @param fd - file descriptor
	void readFrameData(int fd);

private:
	void closeDevice();
	void setFormat();
	void openDevice();
	int xioctl(unsigned long request, void *arg, const QString &possibleError);
	void initMMAP();
	void startCapturing();
	int readFrame();
	void stopCapturing();
	void freeMMAP();

	int mFileDescriptor = -1;
	const QString fileDevicePath;

	struct buffer {
		uint8_t *start;
		size_t  length;
	};
	QVector<uint8_t> mFrame;
	QVector<buffer> buffers;
	v4l2_format mFormat {};
	QScopedPointer<QSocketNotifier> mNotifier;
	convertFunctionPtr mConvertFunc; // convert real camera shot to RGB888
};

