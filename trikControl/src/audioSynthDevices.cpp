#include "audioSynthDevices.h"

#include <QAudioDeviceInfo>
#include <qmath.h>
#include <QsLog.h>

AudioSynthDevice::AudioSynthDevice(QAudioFormat format)
	:sampleSize(format.sampleSize())
	,sampleRate(format.sampleRate())
{
	setOpenMode(QIODevice::ReadOnly);
}


//still has problems, use AudioSynthDeviceBuffered
//pay attention to sample size
qint64 AudioSynthDevice::readData (char *data, qint64 maxlen)
{

	// based on http://www.musicdsp.org/showone.php?id=9
	// but with fixed point math
	const int channelBytes = sampleSize / 8;

	qint64 length = maxlen/sampleSize;

	static const int M = 1 << 16;
	const float w = HzFreq * M_PI / sampleRate;
	const long b1 = 2.0 * cos(w)*M;
	static const int AMPLITUDE = ((1<<sampleSize)-1)>>1;

	unsigned char *ptr = reinterpret_cast<unsigned char *>(data);

	int y1 = M * qSin(w);
	int y2 = M * qSin(2*w);
	int y0;

	int i = 0;

	while(length){

		y0 = b1*y1 / M - y2;
		y2 = b1*y0 / M - y1;
		y1 = b1*y2 / M - y0;

		if(sampleSize == 8){
			const qint8 val = static_cast<qint8>(y0*AMPLITUDE/M);
			*reinterpret_cast<quint8*>(ptr) = val;
		}
		if(sampleSize == 16)
		{
			const qint16 val = static_cast<qint16>(y0*AMPLITUDE/M);
			*reinterpret_cast<quint16*>(ptr) = val;
		}

		ptr += sampleSize;
		length -= channelBytes;
		i+=sampleSize;
	}
	return i;
}

qint64 AudioSynthDevice::writeData(const char *, qint64 )
{

	return -1;
}


AudioSynthDeviceBuffered::AudioSynthDeviceBuffered(QObject *parent)
	:	QIODevice(parent)
	,	m_pos(0)
{

}

AudioSynthDeviceBuffered::~AudioSynthDeviceBuffered()
{

}

void AudioSynthDeviceBuffered::start(QAudioFormat &format, int hzFreq)
{
	open(QIODevice::ReadOnly);
	generateData(format, 1000000, hzFreq);

}

void AudioSynthDeviceBuffered::stop()
{
	m_pos = 0;
	close();
}

void AudioSynthDeviceBuffered::generateData(const QAudioFormat &format, qint64 durationUs, int sampleRate)
{
	const int channelBytes = format.sampleSize() / 8;
	const int sampleBytes = format.channelCount() * channelBytes;

	qint64 length = (format.sampleRate() * format.channelCount() * (format.sampleSize() / 8))
								* durationUs / 100000;

	Q_ASSERT(length % sampleBytes == 0);
	Q_UNUSED(sampleBytes) // suppress warning in release builds

	m_buffer.resize(length);
	unsigned char *ptr = reinterpret_cast<unsigned char *>(m_buffer.data());
	static const int M = 1 << 16;
	const float w = sampleRate * M_PI / format.sampleRate();
	const long b1 = 2.0 * cos(w)*M;
	static const int AMPLITUDE = ((1<<format.sampleSize())-1)>>1;

	int y1 = M * qSin(w);
	int y2 = M * qSin(2*w);
	int y0;

	while(length){

		y0 = b1*y1 / M - y2;
		y2 = b1*y0 / M - y1;
		y1 = b1*y2 / M - y0;


		if(format.sampleSize() == 8){
		const qint8 val = static_cast<qint8>(y0*AMPLITUDE/M);
		*reinterpret_cast<quint8*>(ptr) = val;
		}
		if(format.sampleSize() == 16)
		{
			const qint16 val = static_cast<qint16>(y0*AMPLITUDE/M);
			*reinterpret_cast<quint16*>(ptr) = val;
		}

		ptr += channelBytes;
		length -= channelBytes;
	}
}

qint64 AudioSynthDeviceBuffered::readData(char *data, qint64 len)
{
	qint64 total = 0;
	while (len - total > 0) {
		const qint64 chunk = qMin((m_buffer.size() - m_pos), len - total);
		memcpy(data + total, m_buffer.constData() + m_pos, chunk);
		m_pos = (m_pos + chunk) % m_buffer.size();
		total += chunk;
	}
	return total;
}

qint64 AudioSynthDeviceBuffered::writeData(const char *data, qint64 len)
{
	Q_UNUSED(data);
	Q_UNUSED(len);

	return 0;
}

qint64 AudioSynthDeviceBuffered::bytesAvailable() const
{
	return m_buffer.size() + QIODevice::bytesAvailable();
}
