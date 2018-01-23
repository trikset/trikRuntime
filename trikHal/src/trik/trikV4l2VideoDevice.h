#pragma once
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtCore/QSocketNotifier>
#include <linux/videodev2.h>

class TrikV4l2VideoDevice: QObject
{
	Q_OBJECT
public:
	explicit TrikV4l2VideoDevice(const QString &inputFile);
	~TrikV4l2VideoDevice();
	const QVector<uint8_t> & makeShot();
	const QVector<uint8_t> & getFrame() const { return mFrame; }

signals:
	void dataReady();
public slots:
	void readFrameData(int fd);
private:
	void closeDevice();
	void setFormat();
	void openDevice();
	int xioctl(long request, void *arg, const QString &possibleError);
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
	v4l2_format mFormat;
	QSocketNotifier *mNotifier {}; // Has ownership
};

