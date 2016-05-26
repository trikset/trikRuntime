#include "tonePlayer.h"
#include <QObject>
#include <QTimer>
#include <QFile>
#include <QBuffer>
#include <QThread>
#include <QsLog.h>

namespace trikControl{

TonePlayer::TonePlayer()
    : mdevice(0)
    , mbdevice(0)
    , moutput(0)
{

    mtimer.setSingleShot(true);
    initializeAudio();
    mdevice = new AudioSynthDevice(m_format);
    mbdevice = new AudioSynthDeviceBuffered(this);
    moutput = new QAudioOutput(m_format, this);
}

void TonePlayer::initializeAudio(){

    m_format.setChannelCount(1);
    m_format.setSampleRate(44100);
    m_format.setSampleSize(8);
    m_format.setSampleType(QAudioFormat::SampleType::SignedInt);
    m_format.setCodec("audio/pcm");

    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    if (!info.isFormatSupported(m_format)) {
            //QLOG_WARN()<<format << " audio format not supported by backend.";
            m_format = info.nearestFormat(m_format);
    }
}

void TonePlayer::play(float hzFreq, int msDuration) {

    moutput->reset();
    switch (moutput->state()) {
        case QAudio::IdleState: break;
        default:break;
    }

    mtimer.setInterval(msDuration);
    mdevice->HzFreq = hzFreq;
    mbdevice->start(m_format, hzFreq);
    connect(&mtimer, SIGNAL(timeout()), this, SLOT(stop()));
    mtimer.start();
    moutput->start(mbdevice);
}


void TonePlayer::stop() {
    mtimer.stop();
    moutput->stop();
    mdevice->resetState();
}
}


