#include "stubIIOFile.h"

#include <QsLog.h>

trikHal::stub::StubIIOFile::StubIIOFile(const QString &fileName)
	: mFileName(fileName)
{
}

bool trikHal::stub::StubIIOFile::open()
{
	QLOG_INFO() << "Opening stub:" << mFileName;
	return true;
}

bool trikHal::stub::StubIIOFile::close()
{
	QLOG_INFO() << "Opening stub:" << mFileName;
	return true;
}

QString trikHal::stub::StubIIOFile::fileName() const
{
	return mFileName;
}

bool trikHal::stub::StubIIOFile::isOpened() const
{
	return true;
}
