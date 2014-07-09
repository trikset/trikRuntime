#pragma once
#include <QtCore/QList>

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    #include <QtGui/QWidget>
#else
    #include <QtWidgets/QWidget>
#endif

/// Class of graphic widget.
class GraphicsWidget : public QWidget
{
public:
    GraphicsWidget();
    ~GraphicsWidget();

    /// Draw point on the widget.
    /// @param x - x coordinate.
    /// @param y - y coordinate.
    void drawPoint(int x, int y);

    /// Delete all points.
    void deleteAllPoints();

    /// Draw line on the widget.
    /// @param x1 - first point's x coordinate.
    /// @param y1 - first point's y coordinate.
    /// @param x1 - second point's x coordinate.
    /// @param y1 - second point's y coordinate.
    void drawLine(int x1, int y1, int x2, int y2);

    /// Delete all lines.
    void deleteAllLines();

    /// Draw rect on the widget.
    /// @param x - x coordinate.
    /// @param y - y coordinate.
    /// @param width - rect's width.
    /// @param height - rect's height.
    void drawRect(int x, int y, int width, int height);

    /// Delete all rects.
    void deleteAllRects();

    /// Draw ellipse.
    /// @param x - x coordinate.
    /// @param y - y coordinate.
    /// @param width - width of ellipse.
    /// @param height - height of ellipse.
    void drawEllipse(int x, int y, int width, int height);

    /// Delete all ellipses.
    void deleteAllEllipses();

    /// Draw arc on the widget.
    /// @param x - x coordinate.
    /// @param y - y coordinate.
    /// @param width - width rect forming an arc.
    /// @param height - height rect forming an arc.
    /// @param startAngle - start angle.
    /// @param spanAngle - end angle.
    void drawArc(int x, int y, int width, int height, int startAngle, int spanAngle);

    /// Delete all arcs.
    void deleteAllArcs();

private:
    /// Struct of point coordinates.
    struct PointCoordinates
    {
        PointCoordinates(int x, int y)
        {
            this->x = x;
            this->y = y;
        }
        int x;
        int y;
    };

    /// Struct of rect coordinates.
    struct RectCoordinates
    {
        RectCoordinates(int x, int y, int width, int height)
        {
            this->x = x;
            this->y = y;
            this->width = width;
            this->height = height;
        }
        int x;
        int y;
        int width;
        int height;
    };

    /// Struct of line coordinates.
    struct LineCoordinates
    {
        LineCoordinates(int x1, int y1, int x2, int y2)
        {
            this->x1 = x1;
            this->y1 = y1;
            this->x2 = x2;
            this->y2 = y2;
        }
        int x1;
        int y1;
        int x2;
        int y2;
    };

    /// Struct of ellipse coordinates.
    struct EllipseCoordinates
    {
        EllipseCoordinates(int x, int y, int width, int height)
        {
            this->x = x;
            this->y = y;
            this->width = width;
            this->height = height;
        }
        int x;
        int y;
        int width;
        int height;
    };

    /// Struct of arc coordinates.
    struct ArcCoordinates
    {
        ArcCoordinates(int x, int y, int width, int height, int startAngle, int spanAngle)
        {
            this->x = x;
            this->y = y;
            this->width = width;
            this->height = height;
            this->startAngle = startAngle;
            this->spanAngle = spanAngle;
        }
        int x;
        int y;
        int width;
        int height;
        int startAngle;
        int spanAngle;
    };

    /// Draw all elements.
    void paintEvent(QPaintEvent *);

    /// Check list contains point.
    /// @param coordinates - point that occurence we are looking for.
    /// @return - true if list contains this point.
    bool isPointFind(PointCoordinates coordinates);

    /// Check list contains line.
    /// @param coordinates - line that occurence we are looking for.
    /// @return - true if list contains this line.
    bool isLineFind(LineCoordinates coordinates);

    /// Check list contains rect.
    /// @param coordinates - rect that occurence we are looking for.
    /// @return - true if list contains this rect.
    bool isRectFind(RectCoordinates coordinates);

    /// Check list contains ellipse.
    /// @param coordinates - ellipse that occurence we are looking for.
    /// @return - true if list contains this ellipse.
    bool isEllipseFind(EllipseCoordinates coordinates);

    /// Check list contains arc.
    /// @param coordinates - arc that occurence we are looking for.
    /// @return - true if list contains this arc.
    bool isArcFind(ArcCoordinates coordinates);

    /// List of all lines.
    QList<LineCoordinates> *lines;

    /// List of all points.
    QList<PointCoordinates> *points;

    /// List of all rectangles.
    QList<RectCoordinates> *rects;

    /// List of all ellipses.
    QList<EllipseCoordinates> *ellipses;

    /// List of all arcs.
    QList<ArcCoordinates> *arcs;
};
