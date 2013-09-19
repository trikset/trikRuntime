#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QStringList>

#include <trikScriptRunner/trikScriptRunner.h>

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);
	QStringList const args = app.arguments();

	if (args.count() != 2) {
		qDebug() << "Usage: trikRun <QtScript file name>";
		return 1;
	}

	QString const scriptFileName = args[1];

	QFile file(scriptFileName);
	file.open(QIODevice::ReadOnly | QIODevice::Text);
	if (!file.isOpen()) {
		qDebug() << "Can not open file " << scriptFileName << " for reading";
		return 1;
	}

	QTextStream input;
	input.setDevice(&file);
	input.setCodec("UTF-8");
	QString const script = input.readAll();
	file.close();

	trikScriptRunner::TrikScriptRunner runner;

	// TODO: Run synchronously.
	runner.run(script);

	return 0;
}
