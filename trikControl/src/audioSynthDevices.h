#pragma once

#include <QIODevice>
#include <QAudioFormat>
#include <math.h>
#include <QAudioOutput>
#include <QByteArray>
#include <QObject>

class AudioSynthDevice : public QIODevice
{
    Q_OBJECT

public:
    //~AudioSynthDevice();
    AudioSynthDevice(QAudioFormat format);
    int HzFreq = 400;
    const int sampleSize;
    const int sampleRate;
    void resetState();
private:


public:
    qint64 readData (char *data, qint64 maxlen) override;

    qint64 writeData(const char *, qint64 ) override;
};




class AudioSynthDeviceBuffered : public QIODevice
{
    Q_OBJECT

public:
    AudioSynthDeviceBuffered(QObject *parent);
    ~AudioSynthDeviceBuffered();

    void start(QAudioFormat &format, int hzFreq);
    void stop();

    qint64 readData(char *data, qint64 maxlen);
    qint64 writeData(const char *data, qint64 len);
    qint64 bytesAvailable() const;

private:
    void generateData(const QAudioFormat &format, qint64 durationUs, int sampleRate);

private:
    qint64 m_pos;
    QByteArray m_buffer;
};

