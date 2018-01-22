#pragma once

#include <QtCore/QString>
#include <QtCore/QVector>

class TrikV4l2VideoDevice
{
public:
	explicit TrikV4l2VideoDevice(const QString &inputFile);
	~TrikV4l2VideoDevice();
	void openDevice();
	QVector<uint8_t> getFrame();
	void closeDevice();
	void setFormat();

private:
	int xioctl(long request, void *arg, const QString &possibleError);
	void initMMAP();
	void startCapturing();
	int readFrame();
	void stopCapturing();
	void freeMMAP();

	int fd;
	const QString fileDevicePath;

	struct buffer {
		uint8_t *start;
		size_t  length;
	};

	QVector<buffer> buffers;
};

