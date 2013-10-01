#include "device.h"

#include <QtCore/QDebug>

#include <cmath>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <linux/ioctl.h>
#include <linux/input.h>

using namespace trikControl;

Device::Device()
{
    tilts << 0 << 0 << 0;
}

void Device::init(int min, int max, const QString &controlFile)
{
    mMax = max;
    mMin = min;

    mDeviceFd = open(controlFile.toAscii().data(), O_SYNC, O_RDONLY);
    if (mDeviceFd == -1)
        qDebug() <<"cannot open input file";

    mSocketNotifier = QSharedPointer<QSocketNotifier>
                        (new QSocketNotifier(mDeviceFd, QSocketNotifier::Read, this));
    connect(mSocketNotifier.data(), SIGNAL(activated(int)), this, SLOT(readFile()));
    mSocketNotifier->setEnabled(true);
}

void Device::readFile()
{
    struct input_event event;

    if (read(mDeviceFd, reinterpret_cast<char*>(&event), sizeof(event)) != sizeof(event))
    {
        qDebug() << "incomplete data read";
    } else
    {
        switch (event.type)
        {
        case EV_ABS:
            switch (event.code)
            {
                case ABS_X: tilts[0] = event.value; break;
                case ABS_Y: tilts[1] = event.value; break;
                case ABS_Z: tilts[2] = event.value; break;
            }
            break;
        case EV_SYN:
            return;
            break;
        }
    }
}

QVector<int> Device::readTilts()
{
    return tilts;
}
