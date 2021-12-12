#ifndef FRAMETUNESCENE_H
#define FRAMETUNESCENE_H

#include <QWidget>
#include <QFrame>
#include <QPixmap>
#include <QSharedPointer>
#include "calibration.h"

class DrawTool;
class DrawToolPencil;
class DrawToolPicker;
class DrawToolRubber;

class FrameTuneScene : public QFrame
{
    Q_OBJECT

    QImage  m_image;
    QPixmap m_ref;
    QPixmap m_mount[2];
    QPixmap m_mountDuck[2];
    QPixmap m_wall;
    QPixmap m_tool;

    QRect   m_hitbox;
    bool    m_scrollAllowed = false;
    QPoint  m_scrollOffsetImage;
    QPoint  m_scrollOffsetHitbox;
    double  m_zoom = 1.0;

    bool    m_drawGrid = true;
    bool    m_drawMetaData = true;

    friend class DrawTool;
    friend class DrawToolPencil;
    friend class DrawToolPicker;
    friend class DrawToolRubber;
    friend class DrawToolSelect;
    friend class DrawToolRect;
    friend class DrawToolLine;

    int     m_mode = MODE_NONE;
    QColor  m_drawColor = Qt::black;
    QSharedPointer<DrawTool> m_curTool;

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
    QPoint &curScrollOffset();

    QPoint mapToImg(const QPointF &p);

public:
    explicit FrameTuneScene(QWidget *parent = nullptr);
    virtual ~FrameTuneScene();

    void setImage(const QPixmap &image);
    QPixmap getImage();

    void setRefImage(const QPixmap &image);
    void setRefOpacity(int percents);
    void clearRef();

    void setOffset(const QPoint &offset);
    QPoint getOffset() const;

    double getZoom() const;
    void setZoom(double zoom);

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

    enum Mode
    {
        MODE_NONE = 0,
        MODE_SELECT,
        MODE_SELECT_COPY,
        MODE_DRAW,
        MODE_PICK_COLOR,
        MODE_RUBBER,
        MODE_RECT,
        MODE_LINE
    };

    void setMode(Mode mode);
    Mode mode() const;

    virtual QSize sizeHint() const;

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void wheelEvent(QWheelEvent *event);

    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseDoubleClickEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);

signals:
    void delta(Qt::MouseButton button, int deltaX, int deltaY);
    void mouseReleased();

private:
    Wall m_showWall = WALL_NONE;
};

#endif // FRAMETUNESCENE_H
