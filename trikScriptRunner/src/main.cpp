#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QStringList>

#include "runner.h"
#include "scriptEngineWorker.h"
#include "qRealCommunicator.h"

using namespace scriptRunner;

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);
	QStringList args = app.arguments();

	qDebug() << "Running " + args.join(" ");

	if (args.count() == 1) {
		QRealCommunicator communicator;
		communicator.listen(8888);
		return app.exec();
	}

	if (args.count() != 2) {
		qDebug() << "Usage: scriptRunner <QtScript file>";
		return 1;
	}

	QString const scriptFileName = args[1];

	QFile file(scriptFileName);
	file.open(QIODevice::ReadOnly | QIODevice::Text);
	if (!file.isOpen()) {
		throw "File open operation failed";
	}

	QTextStream input;
	input.setDevice(&file);
	input.setCodec("UTF-8");
	QString const script = input.readAll();
	file.close();

	ScriptEngineWorker engineWorker;
	engineWorker.run(script);

	return 0;
}
