#include "trikGuiApplication.h"

#include <QtGui/QKeyEvent>

using namespace trikGui;

TrikGuiApplication::TrikGuiApplication(int &argc, char **argv)
	: QApplication(argc, argv)
{
}

bool TrikGuiApplication::notify(QObject *receiver, QEvent *event)
{
	if (event->type() == QEvent::KeyPress) {
		QKeyEvent * const keyEvent = dynamic_cast<QKeyEvent *>(event);
		if (keyEvent != NULL) {
			switch (keyEvent->key()) {
				case Qt::Key_F2: {
					*keyEvent = QKeyEvent(QEvent::KeyPress, Qt::Key_Meta, Qt::NoModifier);
					break;
				}
				case Qt::Key_F3: {
					*keyEvent = QKeyEvent(QEvent::KeyPress, Qt::Key_Left, Qt::NoModifier);
					break;
				}
				case Qt::Key_F4: {
					*keyEvent = QKeyEvent(QEvent::KeyPress, Qt::Key_Down, Qt::NoModifier);
					break;
				}
				case Qt::Key_F5: {
					*keyEvent = QKeyEvent(QEvent::KeyPress, Qt::Key_Up, Qt::NoModifier);
					break;
				}
				case Qt::Key_F6: {
					*keyEvent = QKeyEvent(QEvent::KeyPress, Qt::Key_Enter, Qt::NoModifier);
					break;
				}
				case Qt::Key_F7: {
					*keyEvent = QKeyEvent(QEvent::KeyPress, Qt::Key_Right, Qt::NoModifier);
					break;
				}
			}
		}
	}

	return QApplication::notify(receiver, event);
}
