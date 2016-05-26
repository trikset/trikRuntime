#pragma once

#include "audioSynthDevices.h"
#include <QtCore/QObject>
#include <QAudioOutput>
#include <QTimer>


namespace trikControl {


class TonePlayer : public QObject
{
    Q_OBJECT

public:

    TonePlayer();
    void play(float freqHz,int durationMs);

private:
    QAudioFormat m_format;
    AudioSynthDevice *mdevice;
    AudioSynthDeviceBuffered *mbdevice;
    QAudioOutput *moutput;
    QTimer mtimer;

    void initializeAudio();


public slots:
    void stop();

};
}
