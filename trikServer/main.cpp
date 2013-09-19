#include <QtCore/QCoreApplication>

#include <trikCommunicator/trikCommunicator.h>

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);

	trikCommunicator::TrikCommunicator communicator;
	communicator.listen(8888);

	return app.exec();
}
