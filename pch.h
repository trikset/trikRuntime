#include <QtCore/QString>
#include <QtCore/QObject>
#include <QtCore/QScopedPointer>
#include <QtCore/QHash>
#include <QtCore/QThread>
#include <QtCore/QStringList>
#include <QtCore/QVector>
#include <QtCore/QEventLoop>
#include <QtCore/QTimer>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QSocketNotifier>
#include <QtCore/QReadWriteLock>
#include <QtCore/qglobal.h>

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	#include <QtGui/QWidget>
	#include <QtGui/QLabel>
	#include <QtGui/QApplication>
	#include <QtGui/QVBoxLayout>
#else
	#include <QtWidgets/QWidget>
	#include <QtWidgets/QLabel>
	#include <QtWidgets/QApplication>
	#include <QtWidgets/QVBoxLayout>
#endif

#include <QtGui/QKeyEvent>
