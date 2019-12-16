#pragma once

#include <QObject>
#include <QImage>

class Utilites : public QObject
{
	Q_OBJECT
public:
	static QImage *show(const QVector<int32_t> &array, int width, int height, const QString &format);
};
