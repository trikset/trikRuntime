#include "brickInterface.h"

trikControl::BrickInterface::BrickInterface()
{
	qRegisterMetaType<QVector<uint8_t>>("QVector<uint8_t>");
	qRegisterMetaType<QVector<int>>("QVector<int>");
	qRegisterMetaType<trikKernel::TimeVal>("trikKernel::TimeVal");
}
