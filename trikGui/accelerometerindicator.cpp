#include "accelerometerindicator.h"

#include <QtCore/QString>
#include <trikControl/vectorSensorInterface.h>
#include <QPainter>

#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QMouseEvent>
#include <math.h>
#include <QDebug>
#include <QPixmap>

using namespace trikGui;

AccelerometerIndicator::AccelerometerIndicator(trikControl::VectorSensorInterface &accelerometer
        , QWidget *parent)
    : AbstractIndicator(parent)
    , mAccelerometer(accelerometer)
{
    mText.setAlignment(Qt::AlignTop);
    mText1.setAlignment(Qt::AlignTop);
    mText2.setAlignment(Qt::AlignTop);

    mWid.setAlignment(Qt::AlignCenter);

    mLayout.addWidget(&mText);
    mLayout.addWidget(&mText1);
    mLayout.addWidget(&mText2);

    mLayout.addWidget(&mWid);

    setLayout(&mLayout);
}

template<typename T>
T constrain(T value1, T value2, T max)
{
    T norm = sqrtf(value1 * value1 + value2 * value2);
    return (norm > max) ? (value1 * max / norm) : value1;
}

void AccelerometerIndicator::renew()
{

    QVector<int> value = mAccelerometer.read();

    float norm = sqrtf(value[0] * value[0] + value[1] * value[1] + value[2] * value[2]);

    float x = value[0] / norm * m_bounds.width();
    float y = value[1] / norm * m_bounds.width();


    QPointF acc(y, x);

    mText.setText(QString::number(x));
    mText1.setText(QString::number(y));
    mText2.setText(QString::number(value[2]));

    qreal radius = (m_bounds.width() - m_knopBounds.width()) / 2;

    acc.setX(constrain(acc.x(), acc.y(), radius));
    acc.setY(constrain(acc.y(), acc.x(), radius));

    m_knopBounds.moveCenter(acc + m_bounds.center());
    update();
}


void AccelerometerIndicator::resizeEvent(QResizeEvent *)
{
    mPix = new QPixmap(mWid.width(), mWid.height());
    float size = qMin(rect().width(), rect().height());

    QPointF topleft;
    topleft.setY(((height()-size)/2));
    topleft.setX((width()-size)/2);

    m_bounds = QRectF(topleft, QSize(size, size));

    m_knopBounds.setWidth(size * 0.3);
    m_knopBounds.setHeight(size * 0.3);

    // adjust knob position
    m_knopBounds.moveCenter(QPointF(m_bounds.center().x(), m_bounds.center().y()));
}

void AccelerometerIndicator::paintEvent(QPaintEvent *)
{

    QPainter painter(mPix);

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::HighQualityAntialiasing);

    painter.setPen(QPen(QBrush(Qt::gray), m_bounds.width() * 0.005));
    painter.setBrush(QBrush(Qt::white));
    painter.drawEllipse(m_bounds);

    // draw crosshair
    painter.setPen(QPen(QBrush(Qt::gray), m_bounds.width() * 0.005));
    painter.drawLine(QPointF(m_bounds.left(), m_bounds.center().y()), QPointF(m_bounds.center().x() - m_bounds.width() * 0.35, m_bounds.center().y()));
    painter.drawLine(QPointF(m_bounds.center().x() + m_bounds.width() * 0.35, m_bounds.center().y()), QPointF(m_bounds.right(), m_bounds.center().y()));
    painter.drawLine(QPointF(m_bounds.center().x(), m_bounds.top()), QPointF(m_bounds.center().x(), m_bounds.center().y() - m_bounds.width() * 0.35));
    painter.drawLine(QPointF(m_bounds.center().x(), m_bounds.center().y() + m_bounds.width() * 0.35), QPointF(m_bounds.center().x(), m_bounds.bottom()));
    painter.drawPoint(m_bounds.center().x(), m_bounds.center().y());

    // draw knob
    painter.setPen(QPen(QBrush(Qt::darkGray), m_bounds.width() * 0.005));
    painter.setBrush(QBrush(Qt::darkGray));
    painter.drawEllipse(m_knopBounds);


    QPainter painter1(mPix);
    painter1.drawPixmap(0, 0, *mPix);
}

