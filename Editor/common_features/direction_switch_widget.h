#ifndef DIRECTION_SWITCH_WIDGET_H
#define DIRECTION_SWITCH_WIDGET_H

#include <QWidget>
#include <QPixmap>
#include <QHash>

class DirectionSwitchWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DirectionSwitchWidget(QWidget *parent = nullptr);
    virtual ~DirectionSwitchWidget();

    enum Sides
    {
        S_UNKNOWN = -1,
        S_CENTER = 0,
        S_LEFT,
        S_TOP,
        S_RIGHT,
        S_BOTTOM,
        S_TOP_LEFT,
        S_TOP_RIGHT,
        S_BOTTOM_LEFT,
        S_BOTTOM_RIGHT
    };

    void setUseDiagonals(bool diagonals);
    bool useDiagonals();

    void setDirection(int direction);
    int  direction();

    void setRotatingCenter(bool rc);
    bool rotatingCenter();

    void setPixmap(Sides side, const QPixmap &pixmap);
    void setPixmapOn(Sides side, const QPixmap &pixmap);
    void mapValue(Sides side, int value);
    void unMapValue(Sides side);
    void mapToolTip(Sides side, QString toolTip);

protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void leaveEvent(QEvent *event);
    void paintEvent(QPaintEvent *);
    bool event(QEvent *event);

signals:
    void directionChanged(int direction);
    void clicked(int direction);

private:
    Sides findSide(int x, int y);
    Sides findSide(const QPoint &pos);
    QRect getRect(Sides side);
    const QPixmap &getPixMap(Sides side);
    void drawButton(QPainter &painter,
                    int cellWidth, int cellHeight,
                    DirectionSwitchWidget::Sides side);
    void alignRect(QRect &r);

    int m_direction = 0;
    Sides m_currentSide = S_UNKNOWN;
    Sides m_hoverSide = S_UNKNOWN;
    Sides m_pressSide = S_UNKNOWN;
    bool m_hasDiagonals = false;
    bool m_rotatingCenter = false;
    QHash<Sides, QPixmap>   m_pixmaps;
    QHash<Sides, QPixmap>   m_pixmapsOn;
    QHash<Sides, int>       m_valuesMap;
    QHash<Sides, QString>   m_toolTipsMap;
};

#endif // DIRECTION_SWITCH_WIDGET_H
