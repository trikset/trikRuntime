#include <QtGui/QPainter>

#include "graphicsWidget.h"

GraphicsWidget::GraphicsWidget()
{
    lines = new QList<LineCoordinates>();
    points = new QList<PointCoordinates>();
    rects = new QList<RectCoordinates>();
    ellipses = new QList<EllipseCoordinates>();
    arcs = new QList<ArcCoordinates>();
}

GraphicsWidget::~GraphicsWidget()
{
    delete lines;
    delete points;
    delete rects;
    delete ellipses;
    delete arcs;
}

void GraphicsWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    for (int i = 0; i < lines->length(); i++)
    {
        painter.drawLine(lines->at(i).x1, lines->at(i).y1, lines->at(i).x2, lines->at(i).y2);
    }

    for (int i = 0; i < points->length(); i++)
    {
        painter.drawPoint(points->at(i).x, points->at(i).y);
    }

    for (int i = 0; i < rects->length(); i++)
    {
        painter.drawRect(rects->at(i).x, rects->at(i).y, rects->at(i).width, rects->at(i).height);
    }

    for (int i = 0; i < ellipses->length(); i++)
    {
        painter.drawEllipse(ellipses->at(i).x, ellipses->at(i).y, ellipses->at(i).width, ellipses->at(i).height);
    }

    for (int i = 0; i < arcs->length(); i++)
    {
        painter.drawArc(arcs->at(i).x, arcs->at(i).y, arcs->at(i).width, arcs->at(i).height
                        , arcs->at(i).startAngle, arcs->at(i).spanAngle);
    }
}

void GraphicsWidget::drawPoint(int x, int y)
{
    PointCoordinates coordinates(x, y);

    if (!isPointFind(coordinates)) {
        points->insert(points->length(), coordinates);
    }
}

void GraphicsWidget::deleteAllPoints()
{
    points->clear();
}

bool GraphicsWidget::isPointFind(PointCoordinates coordinates)
{
    for (int i = 0; i < points->length(); i++)
    {
        if (points->at(i).x == coordinates.x && points->at(i).y == coordinates.y) {
            return true;
        }
    }
    return false;
}

void GraphicsWidget::drawLine(int x1, int y1, int x2, int y2)
{
    LineCoordinates coordinates(x1, y1, x2, y2);

    if (!isLineFind(coordinates)) {
        lines->insert(lines->length(), coordinates);
    }
}

void GraphicsWidget::deleteAllLines()
{
    lines->clear();
}

bool GraphicsWidget::isLineFind(LineCoordinates coordinates)
{
    for (int i = 0; i < lines->length(); i++)
    {
        if (lines->at(i).x1 == coordinates.x1 && lines->at(i).y1 == coordinates.y1
            && lines->at(i).x2 == coordinates.x2 && lines->at(i).y2 == coordinates.y2) {
            return true;
        }
    }
    return false;
}

void GraphicsWidget::drawRect(int x, int y, int width, int height)
{
    RectCoordinates coordinates(x, y, width, height);

    if (!isRectFind(coordinates)) {
        rects->insert(rects->length(), coordinates);
    }
}

void GraphicsWidget::deleteAllRects()
{
    rects->clear();
}

bool GraphicsWidget::isRectFind(RectCoordinates coordinates)
{
    for (int i = 0; i < rects->length(); i++)
    {
        if (rects->at(i).x == coordinates.x && rects->at(i).y == coordinates.y
                && rects->at(i).width == coordinates.width && rects->at(i).height == coordinates.height) {
            return true;
        }
    }
    return false;
}

void GraphicsWidget::drawEllipse(int x, int y, int width, int height)
{
    EllipseCoordinates coordinates(x, y, width, height);

    if (!isEllipseFind(coordinates)) {
        ellipses->insert(ellipses->length(), coordinates);
    }
}

void GraphicsWidget::deleteAllEllipses()
{
    ellipses->clear();
}

bool GraphicsWidget::isEllipseFind(EllipseCoordinates coordinates)
{
    for (int i = 0; i < ellipses->length(); i++)
    {
        if (ellipses->at(i).x == coordinates.x && ellipses->at(i).y == coordinates.y
                && ellipses->at(i).width == coordinates.width && ellipses->at(i).height == coordinates.height) {
            return true;
        }
    }
    return false;
}

void GraphicsWidget::drawArc(int x, int y, int width, int height, int startAngle, int spanAngle)
{
    ArcCoordinates coordinates(x, y, width, height, startAngle, spanAngle);

    if (!isArcFind(coordinates)) {
        arcs->insert(arcs->length(), coordinates);
    }
}

void GraphicsWidget::deleteAllArcs()
{
    arcs->clear();
}

bool GraphicsWidget::isArcFind(ArcCoordinates coordinates)
{
    for (int i = 0; i < arcs->length(); i++)
    {
        if (arcs->at(i).x == coordinates.x && points->at(i).y == coordinates.y
            && arcs->at(i).width == coordinates.width && arcs->at(i).height == coordinates.height
            && arcs->at(i).startAngle == coordinates.startAngle
            && arcs->at(i).spanAngle == coordinates.spanAngle) {
        return true;
        }
    }
    return false;
}
