#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>

#include <trikCommunicator/trikCommunicator.h>

int main(int argc, char *argv[])
{
	int const port = 8888;

	QCoreApplication app(argc, argv);

	qDebug() << "Running TrikServer on port" << port;

	trikCommunicator::TrikCommunicator communicator;
	communicator.listen(port);

	return app.exec();
}
