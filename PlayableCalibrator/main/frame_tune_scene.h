#ifndef FRAMETUNESCENE_H
#define FRAMETUNESCENE_H

#include <QWidget>
#include <QFrame>
#include <QPixmap>
#include "calibration.h"


class FrameTuneScene : public QFrame
{
    Q_OBJECT

    QPixmap m_image;
    QPixmap m_ref;
    QPixmap m_mount[2];
    QPixmap m_mountDuck[2];
    QPixmap m_wall;
    QPixmap m_tool;

    QRect   m_hitbox;
    bool    m_scrollAllowed = false;
    QPoint  m_scrollOffset;
    double  m_zoom = 1.0;

    bool    m_drawGrid = true;
    bool    m_drawMetaData = true;

    //! Focus on hitbox instead of the left-top frame corder
    bool    m_focusHitBox = false;

    double  m_refOpacity = 0.5;

    QColor  m_bgColor;

    int     m_hitboxWidth = 0;
    int     m_hitboxHeight = 0;
    int     m_hitboxHeightDuck = 0;
    bool    m_ducking = false;
    bool    m_showMount = false;
    bool    m_isRightDir = false;

    bool    m_blockRepaint = false;

    bool    m_showGrabOffset = false;
    bool    m_grabTop = false;
    QPoint  m_grabOffset;

    // Mouse
    Qt::MouseButton button = Qt::NoButton;
    bool pressed = false;
    QPointF prevPos;

    bool scrollPossible();

public:
    explicit FrameTuneScene(QWidget *parent = nullptr);

    void setImage(const QPixmap &image);

    void setRefImage(const QPixmap &image);
    void setRefOpacity(int percents);
    void clearRef();

    void setGlobalSetup(const Calibration &calibration);
    void setFrameSetup(const CalibrationFrame &frame);
    void setAllowScroll(bool allowScroll);
    void setDrawMetaData(bool en);
    void setDrawGrid(bool en);
    void setHitBoxFocus(bool en);
    void resetScroll();
    void setBgColor(QColor clr);

    void setBlockRepaint(bool en);

    QColor getBgColor() const;

    enum Wall
    {
        WALL_NONE,
        WALL_FLOOR,
        WALL_FLOOR_WL,
        WALL_FLOOR_WR,
        WALL_CEILING,
        WALL_CEILING_WL,
        WALL_CEILING_WR,
        WALL_BOTH_H,
        WALL_BOTH_V
    };

    void setWall(Wall w);

    virtual QSize sizeHint() const;

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void wheelEvent(QWheelEvent *event);

    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);

signals:
    void deltaX(Qt::MouseButton button, int delta);
    void deltaY(Qt::MouseButton button, int delta);
    void mouseReleased();

private:
    Wall m_showWall = WALL_NONE;
};

#endif // FRAMETUNESCENE_H
