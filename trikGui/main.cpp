#include <QtCore/QCoreApplication>

#include "trikGuiApplication.h"
#include "startWidget.h"

using namespace trikGui;

int main(int argc, char *argv[])
{
	TrikGuiApplication a(argc, argv);
	StartWidget w;
	w.show();

	return a.exec();
}
