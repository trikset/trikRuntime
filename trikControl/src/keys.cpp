#include "keys.h"

#include <QDebug>
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

Keys::Keys():
    buttonCode(KEY_RESERVED),
    buttonValue(0)
{

}

void Keys::init(const QString &keysPath)
{
    mKeysFd = open(keysPath.toAscii().data(), O_SYNC, O_RDONLY);
    if (mKeysFd == -1)
        qDebug()<<"cannot open keys input file";

    mSocketNotifier = QSharedPointer<QSocketNotifier>
                        (new QSocketNotifier(mKeysFd, QSocketNotifier::Read, this));
    connect(mSocketNotifier.data(), SIGNAL(activated(int)), this, SLOT(readKeysEvent()));
    mSocketNotifier->setEnabled(true);
}

void Keys::readKeysEvent()
{
    struct input_event event;

    if (read(mKeysFd, reinterpret_cast<char*>(&event), sizeof(event)) != sizeof(event))
    {
        qDebug()<<"keys: incomplete data read";
        return;
    }
    switch (event.type)
    {
    case EV_KEY:
        buttonCode = static_cast<int>(event.code);
        buttonValue = static_cast<int>(event.value);
        break;
    case EV_SYN:
        emit buttonPressed(buttonCode, buttonValue);
        break;
    }
}
