/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "trikDEApplication.h"
#include <QKeyEvent>

TrikDEApplication::TrikDEApplication(int &argc, char **argv) :
    QApplication(argc, argv)
{
}

bool TrikDEApplication::notify(QObject *receiver, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = dynamic_cast<QKeyEvent *>(event);
        if (keyEvent != NULL)
            switch (keyEvent->key())
            {
                case Qt::Key_F2:
                {
                    *keyEvent = QKeyEvent(QEvent::KeyPress, Qt::Key_Meta, Qt::NoModifier);
                    break;
                }
                case Qt::Key_F3:
                {
                    *keyEvent = QKeyEvent(QEvent::KeyPress, Qt::Key_Left, Qt::NoModifier);
                    break;
                }
                case Qt::Key_F4:
                {
                    *keyEvent = QKeyEvent(QEvent::KeyPress, Qt::Key_Down, Qt::NoModifier);
                    break;
                }
                case Qt::Key_F5:
                {
                    *keyEvent = QKeyEvent(QEvent::KeyPress, Qt::Key_Up, Qt::NoModifier);
                    break;
                }
                case Qt::Key_F6:
                {
                    *keyEvent = QKeyEvent(QEvent::KeyPress, Qt::Key_Enter, Qt::NoModifier);
                    break;
                }
                case Qt::Key_F7:
                {
                    *keyEvent = QKeyEvent(QEvent::KeyPress, Qt::Key_Right, Qt::NoModifier);
                    break;
                }
            }
    }
    return QApplication::notify(receiver, event);
}
