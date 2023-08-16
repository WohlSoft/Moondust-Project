#include "frame_tune_scene.h"
#include <QPainter>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QtDebug>
#include <cmath>
#include <pge_qt_compat.h>


class DrawTool : public QObject
{
protected:
    QImage *m_image;
    FrameTuneScene *m_self;

public:
    explicit DrawTool(QImage *image, FrameTuneScene *parent = 0) :
        QObject(parent),
        m_image(image),
        m_self(parent)
    {}

    virtual ~DrawTool() {}

    virtual bool mousePress(const QPoint &p) = 0;
    virtual bool mouseMove(const QPoint &p, const QPoint &delta) = 0;
    virtual bool mouseRelease(const QPoint &p) = 0;

    virtual bool mouseDoubleClick(const QPoint &)
    {
        return false;
    }

    virtual void drawPreview(QPainter &, const QRect&, double, const QPoint &)
    {}

    void setImage(QImage &img)
    {
        m_image = &img;
    }
};


class DrawToolPencil : public DrawTool
{
    QPoint startAt;
public:
    explicit DrawToolPencil(QImage *image, FrameTuneScene *parent = 0) :
        DrawTool(image, parent)
    {}

    virtual bool mousePress(const QPoint &p)
    {
        startAt = p;
        m_image->setPixel(p.x(), p.y(), m_self->m_drawColor.toRgb().rgba());
        return true;
    }

    virtual bool mouseMove(const QPoint &p, const QPoint &)
    {
        QPainter pa(m_image);
        pa.setPen(m_self->m_drawColor);
        pa.drawLine(startAt, p);
        pa.end();
        startAt = p;
        return true;
    }

    virtual bool mouseRelease(const QPoint &)
    {
        return false;
    }

    virtual void drawPreview(QPainter &p, const QRect& dst, double zoom, const QPoint &pos)
    {
        p.setPen(QPen(Qt::gray, 1));
        p.setBrush(m_self->m_drawColor);
        if(zoom <= 1.0)
            p.drawPoint(pos);
        else
        {
            p.drawRect(QRectF(dst.x() + pos.x() * zoom,
                              dst.y() + pos.y() * zoom,
                              zoom,
                              zoom
                       ));
        }
    }
};

class DrawToolPicker : public DrawTool
{
public:
    explicit DrawToolPicker(QImage *image, FrameTuneScene *parent = 0) :
        DrawTool(image, parent)
    {};

    virtual bool mousePress(const QPoint &)
    {
        return false;
    }

    virtual bool mouseMove(const QPoint &, const QPoint &)
    {
        return false;
    }

    virtual bool mouseRelease(const QPoint &p)
    {
        m_self->m_drawColor.setRgba(m_image->pixel(p.x(), p.y()));
        return true;
    }

    virtual void drawPreview(QPainter &p, const QRect& dst, double zoom, const QPoint &pos)
    {
        p.setPen(QPen(Qt::gray, 1));
        p.setBrush(m_self->m_drawColor);
        if(zoom <= 1.0)
            p.drawPoint(pos);
        else
        {
            p.drawRect(QRectF(dst.x() + pos.x() * zoom,
                              dst.y() + pos.y() * zoom,
                              zoom,
                              zoom
                       ));
        }
    }
};

class DrawToolRubber : public DrawTool
{
    QPoint startAt;
public:
    explicit DrawToolRubber(QImage *image, FrameTuneScene *parent = 0) :
        DrawTool(image, parent)
    {};

    virtual bool mousePress(const QPoint &p)
    {
        startAt = p;
        m_image->setPixel(p.x(), p.y(), 0x00000000);
        return true;
    }

    virtual bool mouseMove(const QPoint &p, const QPoint &)
    {
        QPainter pa(m_image);
        pa.setCompositionMode (QPainter::CompositionMode_Source);
        pa.setPen(Qt::transparent);
        pa.drawLine(startAt, p);
        pa.end();
        startAt = p;
        return true;
    }

    virtual bool mouseRelease(const QPoint &)
    {
        return false;
    }

    virtual void drawPreview(QPainter &p, const QRect& dst, double zoom, const QPoint &pos)
    {
        p.setPen(QPen(Qt::gray, 1));
        p.setBrush(Qt::transparent);
        if(zoom <= 1.0)
            p.drawPoint(pos);
        else
        {
            p.drawRect(QRectF(dst.x() + pos.x() * zoom,
                              dst.y() + pos.y() * zoom,
                              zoom,
                              zoom
                       ));
        }
    }
};


class DrawToolLine : public DrawTool
{
    QPoint startAt;
    QImage demo;

public:
    explicit DrawToolLine(QImage *image, FrameTuneScene *parent = 0) :
        DrawTool(image, parent)
    {};

    virtual bool mousePress(const QPoint &p)
    {
        startAt = p;
        demo = QImage(m_image->size(), QImage::Format_ARGB32);
        demo.fill(Qt::transparent);
        demo.setPixel(p.x(), p.y(), m_self->m_drawColor.toRgb().rgba());
        return true;
    }

    virtual bool mouseMove(const QPoint &p, const QPoint &)
    {
        demo = QImage(m_image->size(), QImage::Format_ARGB32);
        demo.fill(Qt::transparent);
        QPainter pa(&demo);
        pa.setPen(m_self->m_drawColor);
        pa.drawLine(startAt, p);
        pa.end();
        return true;
    }

    virtual bool mouseRelease(const QPoint &)
    {
        QPainter pa(m_image);
        pa.setPen(m_self->m_drawColor);
        pa.drawImage(0, 0, demo);
        demo = QImage();
        return true;
    }

    virtual void drawPreview(QPainter &p, const QRect& dst, double zoom, const QPoint &pos)
    {
        p.setPen(QPen(Qt::gray, 1));
        p.setBrush(m_self->m_drawColor);
        if(zoom <= 1.0)
            p.drawPoint(pos);
        else
        {
            p.drawRect(QRectF(dst.x() + pos.x() * zoom,
                              dst.y() + pos.y() * zoom,
                              zoom,
                              zoom
                       ));
        }

        if(demo.isNull())
            return;
        p.drawImage(dst, demo);
    }
};

class DrawToolRect : public DrawTool
{
    QPoint startAt;
    QImage demo;

public:
    explicit DrawToolRect(QImage *image, FrameTuneScene *parent = 0) :
        DrawTool(image, parent)
    {};

    virtual bool mousePress(const QPoint &p)
    {
        startAt = p;
        demo = QImage(m_image->size(), QImage::Format_ARGB32);
        demo.fill(Qt::transparent);
        demo.setPixel(p.x(), p.y(), m_self->m_drawColor.toRgb().rgba());
        return true;
    }

    virtual bool mouseMove(const QPoint &p, const QPoint &)
    {
        demo = QImage(m_image->size(), QImage::Format_ARGB32);
        demo.fill(Qt::transparent);
        QPainter pa(&demo);
        pa.setPen(m_self->m_drawColor);
        pa.drawRect(QRect(startAt, p));
        pa.end();
        return true;
    }

    virtual bool mouseRelease(const QPoint &)
    {
        QPainter pa(m_image);
        pa.drawImage(0, 0, demo);
        pa.end();
        demo = QImage();
        return true;
    }

    virtual void drawPreview(QPainter &p, const QRect& dst, double zoom, const QPoint &pos)
    {
        p.setPen(QPen(Qt::gray, 1));
        p.setBrush(m_self->m_drawColor);
        if(zoom <= 1.0)
            p.drawPoint(pos);
        else
        {
            p.drawRect(QRectF(dst.x() + pos.x() * zoom,
                              dst.y() + pos.y() * zoom,
                              zoom,
                              zoom
                       ));
        }

        if(demo.isNull())
            return;
        p.drawImage(dst, demo);
    }
};

class DrawToolSelect : public DrawTool
{
    QPoint startAt;
    QImage floating;
    QRect  selection;
    bool selecting = true;
    bool moving = true;
    bool resetting = false;
    bool m_copyMode = false;

public:
    explicit DrawToolSelect(bool copyMode, QImage *image, FrameTuneScene *parent = 0) :
        DrawTool(image, parent),
        m_copyMode(copyMode)
    {};

    virtual ~DrawToolSelect()
    {
        mouseDoubleClick(QPoint());
    }

    virtual bool mousePress(const QPoint &p)
    {
        startAt = p;
        if(floating.isNull())
        {
            selecting = true;
            selection = QRect(startAt, p);
        }
        else
        {
            moving = true;
        }

        return true;
    }

    virtual bool mouseMove(const QPoint &p, const QPoint &delta)
    {
        if(resetting)
            return false;

        if(selecting)
        {
            selection.setLeft(startAt.x() > p.x() ? p.x() : startAt.x());
            selection.setTop(startAt.y() > p.y() ? p.y() : startAt.y());
            selection.setRight((startAt.x() <= p.x() ? p.x() : startAt.x()) - 1);
            selection.setBottom((startAt.y() <= p.y() ? p.y() : startAt.y()) - 1);
        }
        else if(moving)
        {
            selection.translate(delta);
            startAt = p;
        }
        return true;
    }

    virtual bool mouseDoubleClick(const QPoint &)
    {
        if(!floating.isNull())
        {
            resetting = true;
            QPainter p(m_image);
            p.drawImage(selection.topLeft(), floating);
            p.end();
            floating = QImage();
            return true;
        }
        return false;
    }

    virtual bool mouseRelease(const QPoint &)
    {
        if(selecting && !resetting)
        {
            if(selection.intersects(m_image->rect()))
            {
                floating = m_image->copy(selection);
                if(!floating.isNull() && !m_copyMode)
                {
                    QPainter pa(m_image);
                    pa.setCompositionMode(QPainter::CompositionMode_Source);
                    pa.setPen(Qt::NoPen);
                    pa.setBrush(Qt::transparent);
                    pa.drawRect(selection);
                    pa.end();
                }
            }
        }

        selecting = false;
        moving = false;
        resetting = false;
        return true;
    }

    virtual void drawPreview(QPainter &p, const QRect &dst, double zoom, const QPoint &)
    {
        auto r = QRectF(dst.x() + selection.x() * zoom,
                        dst.y() + selection.y() * zoom,
                        selection.width() * zoom,
                        selection.height() * zoom
                     );
        if(selecting)
        {
            p.setPen(QPen(Qt::red, 2, Qt::DotLine));
            p.drawRect(r);
        }
        else if(!floating.isNull())
        {
            p.setPen(QPen(Qt::cyan, 4, Qt::DotLine));
            p.drawRect(r);
            p.drawImage(r, floating);
        }
    }
};


bool FrameTuneScene::scrollPossible()
{
    return m_scrollAllowed;
}

QPoint &FrameTuneScene::curScrollOffset()
{
    return m_focusHitBox ? m_scrollOffsetHitbox : m_scrollOffsetImage;
}

QPoint FrameTuneScene::mapToImg(const QPointF &p)
{
    auto canvas = rect();
    auto c = canvas.center() + (curScrollOffset() * m_zoom);
    QRect dst;
    dst.moveTo(c);

    // Simulate SMBX' constant mount offset
    int mountOffset = ((m_ducking ? 64 : 72) - m_hitbox.height());

    if(!m_image.isNull())
    {
        QSize is = m_image.size();
        if(m_focusHitBox)
        {
            dst.setX(c.x() - qRound((m_hitbox.x() + m_hitbox.width() / 2) * m_zoom));
            dst.setY(c.y() - qRound((m_hitbox.bottom() + (m_showMount ? mountOffset : 0)) * m_zoom));
        }
        else
        {
            dst.setX(c.x() - qRound((is.width() / 2) * m_zoom));
            dst.setY(c.y() - qRound((is.height() / 2) * m_zoom));
        }
        dst.setWidth(qRound(is.width() * m_zoom));
        dst.setHeight(qRound(is.width() * m_zoom));
    }

    QPoint o = QPoint(qRound((p.x() - dst.x()) / m_zoom), qRound((p.y() - dst.y()) / m_zoom));
    qDebug() << "x=" << o.x() << "y=" << o.y();

    return o;
}

FrameTuneScene::FrameTuneScene(QWidget *parent) : QFrame(parent)
{
    m_bgColor = QColor(Qt::white);
    m_mount[0] = QPixmap(":/images/mount.png");
    m_mountDuck[0] = QPixmap(":/images/mount_duck.png");
    m_mount[1] = QPixmap(":/images/mount2.png");
    m_mountDuck[1] = QPixmap(":/images/mount2_duck.png");
    m_wall = QPixmap(":/images/wall.png");
    m_tool = QPixmap(":/images/tool.png");
}

FrameTuneScene::~FrameTuneScene()
{
    m_curTool.reset();
}

void FrameTuneScene::setImage(const QPixmap &image)
{
    m_image = image.toImage();
    if(!m_blockRepaint)
        repaint();
}

QPixmap FrameTuneScene::getImage()
{
    return QPixmap::fromImage(m_image);
}

void FrameTuneScene::setRefImage(const QPixmap &image)
{
    m_ref = image;
    if(!m_blockRepaint)
        repaint();
}

void FrameTuneScene::setRefOpacity(int percents)
{
    m_refOpacity = percents / 100.0;
    if(!m_blockRepaint)
        repaint();
}

void FrameTuneScene::clearRef()
{
    m_ref = QPixmap();
    if(!m_blockRepaint)
        repaint();
}

void FrameTuneScene::setOffset(const QPoint &offset)
{
    curScrollOffset() = offset;
    if(!m_blockRepaint)
        repaint();
}

QPoint FrameTuneScene::getOffset() const
{
    return m_focusHitBox ? m_scrollOffsetHitbox : m_scrollOffsetImage;
}

double FrameTuneScene::getZoom() const
{
    return m_zoom;
}

void FrameTuneScene::setZoom(double zoom)
{
    m_zoom = zoom;
    if(m_zoom >= 20.0)
        m_zoom = 20.0;
    else if(m_zoom <= 1.0)
        m_zoom = 1.0;
    if(!m_blockRepaint)
        repaint();
}

void FrameTuneScene::setGlobalSetup(const Calibration &calibration)
{
    m_hitboxWidth = calibration.frameWidth;
    m_hitboxHeight = calibration.frameHeight;
    m_hitboxHeightDuck = calibration.frameHeightDuck;
    m_grabTop = calibration.grabOverTop;
    m_grabOffset = QPoint(calibration.grabOffsetX, calibration.grabOffsetY);
}

void FrameTuneScene::setFrameSetup(const CalibrationFrame &frame)
{
    m_ducking = frame.isDuck;
    m_showGrabOffset = frame.showGrabItem;
    m_showMount = frame.isMountRiding;
    m_isRightDir = frame.isRightDir;
    m_hitbox.setRect(frame.offsetX,
                     frame.offsetY,
                     m_hitboxWidth,
                     frame.isDuck ? m_hitboxHeightDuck : m_hitboxHeight);
    if(!m_blockRepaint)
        repaint();
}

void FrameTuneScene::setAllowScroll(bool allowScroll)
{
    m_scrollAllowed = allowScroll;
    if(!m_scrollAllowed)
        curScrollOffset() = QPoint(0, 0);
}

void FrameTuneScene::setDrawMetaData(bool en)
{
    m_drawMetaData = en;
    if(!m_blockRepaint)
        repaint();
}

void FrameTuneScene::setDrawGrid(bool en)
{
    m_drawGrid = en;
    if(!m_blockRepaint)
        repaint();
}

void FrameTuneScene::setHitBoxFocus(bool en)
{
    m_focusHitBox = en;
    if(!m_blockRepaint)
        repaint();
}

void FrameTuneScene::resetScroll()
{
    curScrollOffset() = QPoint(0, 0);
    if(!m_blockRepaint)
        repaint();
}

void FrameTuneScene::setBgColor(QColor clr)
{
    m_bgColor = clr;
    if(!m_blockRepaint)
        repaint();
}

void FrameTuneScene::setBlockRepaint(bool en)
{
    m_blockRepaint = en;
    if(!m_blockRepaint)
        repaint();
}

QColor FrameTuneScene::getBgColor() const
{
    return m_bgColor;
}

void FrameTuneScene::setWall(Wall w)
{
    m_showWall = w;
    if(!m_blockRepaint)
        repaint();
}

void FrameTuneScene::setMode(Mode mode)
{
    m_mode = mode;

    switch(m_mode)
    {
    case MODE_NONE:
        m_curTool.reset();
        setMouseTracking(false);
        repaint();
        break;

    case MODE_SELECT:
        m_curTool.reset(new DrawToolSelect(false, &m_image, this));
        setMouseTracking(true);
        repaint();
        break;

    case MODE_SELECT_COPY:
        m_curTool.reset(new DrawToolSelect(true, &m_image, this));
        setMouseTracking(true);
        repaint();
        break;

    case MODE_DRAW:
        m_curTool.reset(new DrawToolPencil(&m_image, this));
        setMouseTracking(true);
        repaint();
        break;

    case MODE_PICK_COLOR:
        m_curTool.reset(new DrawToolPicker(&m_image, this));
        setMouseTracking(true);
        repaint();
        break;

    case MODE_RUBBER:
        m_curTool.reset(new DrawToolRubber(&m_image, this));
        setMouseTracking(true);
        repaint();
        break;

    case MODE_RECT:
        m_curTool.reset(new DrawToolRect(&m_image, this));
        setMouseTracking(true);
        repaint();
        break;

    case MODE_LINE:
        m_curTool.reset(new DrawToolLine(&m_image, this));
        setMouseTracking(true);
        repaint();
        break;
    }
}

FrameTuneScene::Mode FrameTuneScene::mode() const
{
    return (Mode)m_mode;
}

void FrameTuneScene::runAction(Actions action)
{
    Q_UNUSED(action)
    // TODO: Implement me
}

QSize FrameTuneScene::sizeHint() const
{
    return QSize(300, 300);
}

void FrameTuneScene::paintEvent(QPaintEvent * /*event*/)
{
    QPainter painter(this);
    painter.fillRect(rect(), QBrush(m_bgColor));

    auto canvas = rect();
    auto c = canvas.center() + (curScrollOffset() * m_zoom);
    QRect dst;
    dst.moveTo(c);

    // Simulate SMBX' constant mount offset
    int mountOffset = ((m_ducking ? 64 : 72) - m_hitbox.height());

    if(!m_image.isNull())
    {
        QSize is = m_image.size();
        if(m_focusHitBox)
        {
            dst.setX(c.x() - qRound((m_hitbox.x() + m_hitbox.width() / 2) * m_zoom));
            dst.setY(c.y() - qRound((m_hitbox.bottom() + (m_showMount ? mountOffset : 0)) * m_zoom));
        }
        else
        {
            dst.setX(c.x() - qRound((is.width() / 2) * m_zoom));
            dst.setY(c.y() - qRound((is.height() / 2) * m_zoom));
        }
        dst.setWidth(qRound(is.width() * m_zoom));
        dst.setHeight(qRound(is.width() * m_zoom));
    }

    if(m_showWall != WALL_NONE)
    {
        int y, x_begin, x_end;
        int wall_x, wall_y_begin, wall_y_end;
        bool drawWall = false;
        QRect p;

        switch(m_showWall)
        {
        case WALL_FLOOR:
        case WALL_FLOOR_WL:
        case WALL_FLOOR_WR:
        {
            y = dst.y() + (m_hitbox.bottom() + 1) * m_zoom;
            x_begin = c.x() - 256 * m_zoom;
            x_end = c.x() + 256 * m_zoom;

            if(m_showMount)
                y += mountOffset * m_zoom;

            if(m_showWall == WALL_FLOOR_WL)
            {
                drawWall = true;
                wall_x = dst.x() + (m_hitbox.left() - m_wall.width()) * m_zoom;
                wall_y_begin = y - (m_wall.height() * 10) * m_zoom;
                wall_y_end = y;
            }
            else if(m_showWall == WALL_FLOOR_WR)
            {
                drawWall = true;
                wall_x = dst.x() + (m_hitbox.right() + 1) * m_zoom;
                wall_y_begin = y - (m_wall.height() * 10) * m_zoom;
                wall_y_end = y;
            }
            break;
        }
        case WALL_CEILING:
        case WALL_CEILING_WL:
        case WALL_CEILING_WR:
        {
            y = dst.y() + (m_hitbox.top() - m_wall.height()) * m_zoom;
            x_begin = c.x() - 256 * m_zoom;
            x_end = c.x() + 256 * m_zoom;

            if(m_showMount)
                y += mountOffset * m_zoom;

            if(m_showWall == WALL_CEILING_WL)
            {
                drawWall = true;
                wall_x = dst.x() + (m_hitbox.left() - m_wall.width()) * m_zoom;
                wall_y_begin = y + m_wall.height() * m_zoom;
                wall_y_end = y + (m_wall.height() * 10) * m_zoom;
            }
            else if(m_showWall == WALL_CEILING_WR)
            {
                drawWall = true;
                wall_x = dst.x() + (m_hitbox.right() + 1) * m_zoom;
                wall_y_begin = y + m_wall.height() * m_zoom;
                wall_y_end = y + (m_wall.height() * 10) * m_zoom;
            }
            break;
        }
        default:
            Q_ASSERT(false);
            break;
        }

        // Floor/Ceiling
        for(int dx = x_begin; dx < x_end;)
        {
            p.setRect(dx, y, m_wall.width() * m_zoom, m_wall.height() * m_zoom);
            painter.drawPixmap(p, m_wall);
            dx += m_wall.width() * m_zoom;
        }

        if(drawWall)
        {
            for(int dy = wall_y_begin; dy < wall_y_end;)
            {
                p.setRect(wall_x, dy, m_wall.width() * m_zoom, m_wall.height() * m_zoom);
                painter.drawPixmap(p, m_wall);
                dy += m_wall.height() * m_zoom;
            }
        }
    }

    if(m_showGrabOffset)
    {
        painter.save();
        if(m_grabTop)
        {
            double tx = m_hitbox.x() + m_hitbox.width() / 2 - m_tool.width() / 2;
            double ty = m_hitbox.y() - m_tool.height() + 6;

            painter.drawPixmap(
                dst.x() + tx * m_zoom,
                dst.y() + ty * m_zoom - 1,
                m_tool.width() * m_zoom,
                m_tool.height() * m_zoom,
                m_tool
            );
        }
        else
        {
            double tx, ty;

            if(m_isRightDir)
                tx = m_hitbox.x() + m_grabOffset.x();
            else
                tx = m_hitbox.x() + m_hitbox.width() - m_grabOffset.x() - m_tool.width();
            ty = m_hitbox.y() + m_grabOffset.y() + 32 - m_tool.height();

            painter.drawPixmap(
                dst.x() + tx * m_zoom,
                dst.y() + ty * m_zoom,
                m_tool.width() * m_zoom,
                m_tool.height() * m_zoom,
                m_tool
            );
        }
        painter.restore();
    }

    if(m_showMount)
    {
        painter.save();
        auto &pm = m_ducking ? m_mountDuck[0] : m_mount[0];
        int mw = pm.width();
        int mh = pm.height() / 2;
        double tx = (m_hitbox.x() + (m_hitbox.width() / 2) - (mw / 2));
        double ty = (m_hitbox.y() + m_hitbox.height() - mh + 2 + mountOffset);

        QRectF mr = QRectF(
                        dst.x() + tx * m_zoom,
                        dst.y() + ty * m_zoom,
                        mw * m_zoom,
                        mh * m_zoom
        );

        painter.drawPixmap(mr, pm, QRectF(0, m_isRightDir ? mh : 0, mw, mh));
        painter.restore();
    }

    if(!m_image.isNull())
    {
        painter.drawImage(dst, m_image);

        if(m_drawGrid && m_drawMetaData)
        {
            painter.save();
            painter.setPen(QPen(Qt::gray, 1));
            painter.setBrush(Qt::transparent);
            painter.drawRect(dst);
            painter.restore();

            painter.save();
            painter.setOpacity(5.0);
            painter.setPen(QPen(Qt::gray, 1, Qt::DotLine));

            if(m_zoom > 3.0)
            {
                for(int y = 0; y < m_image.height(); y += 2)
                {
                    painter.drawLine(
                        dst.x() + 0 * m_zoom,
                        dst.y() + y * m_zoom - 1,
                        dst.x() + m_image.width() * m_zoom,
                        dst.y() + y * m_zoom - 1
                    );
                }

                for(int x = 0; x < m_image.width(); x += 2)
                {
                    painter.drawLine(
                        dst.x() + x * m_zoom - 1,
                        dst.y() + 0 * m_zoom,
                        dst.x() + x * m_zoom - 1,
                        dst.y() + m_image.height() * m_zoom
                    );
                }
            }
            painter.restore();
        }
    }

    if(!m_ref.isNull())
    {
        painter.save();
        painter.setOpacity(m_refOpacity);
        painter.drawPixmap(dst, m_ref);
        painter.restore();
    }

    if(m_drawMetaData)
    {
        painter.save();
        {
            painter.setPen(QPen(Qt::green));
            painter.setBrush(Qt::transparent);
            QRect out;
            out.setX(dst.x() + (m_hitbox.x() * m_zoom));
            out.setY(dst.y() + (m_hitbox.y() * m_zoom));
            out.setWidth((m_hitbox.width() * m_zoom) - 1);
            out.setHeight((m_hitbox.height() * m_zoom) - 1);
            painter.drawRect(out);
        }
        painter.restore();
    }

    if(m_showGrabOffset && m_drawMetaData)
    {
        painter.save();
        painter.setPen(QPen(Qt::red));

        if(m_grabTop)
        {
            double tx = m_hitbox.x();
            double ty = m_hitbox.y() + 6;
            double txc = m_hitbox.x() + m_hitbox.width() / 2;
            double txe = m_hitbox.x() + m_hitbox.width();
            double tye = m_hitbox.y() + 6 - 21;

            painter.drawLine(
                dst.x() + tx * m_zoom,
                dst.y() + ty * m_zoom - 1,
                dst.x() + txe * m_zoom,
                dst.y() + ty * m_zoom - 1
            );
            painter.drawLine(
                dst.x() + txc * m_zoom,
                dst.y() + ty * m_zoom - 1,
                dst.x() + txc * m_zoom,
                dst.y() + tye * m_zoom - 1
            );
        }
        else
        {
            double tx, ty, txe, tye;

            if(m_isRightDir)
            {
                tx = m_hitbox.x() + m_grabOffset.x();
                txe = tx + m_tool.width();
            }
            else
            {
                tx = m_hitbox.x() + m_hitbox.width() - m_grabOffset.x();
                txe = tx - m_tool.width();
            }

            ty = m_hitbox.y() + m_grabOffset.y() + 32;
            tye = ty - m_tool.height();

            painter.drawLine(
                dst.x() + tx * m_zoom,
                dst.y() + ty * m_zoom,
                dst.x() + txe * m_zoom,
                dst.y() + ty * m_zoom
            );
            painter.drawLine(
                dst.x() + tx * m_zoom,
                dst.y() + ty * m_zoom,
                dst.x() + tx * m_zoom,
                dst.y() + tye * m_zoom
            );
        }
        painter.restore();
    }

    if(!m_curTool.isNull())
    {
        QPoint globalCursorPos = mapFromGlobal(QCursor::pos());
        painter.save();
        m_curTool->drawPreview(painter, dst, m_zoom, mapToImg(globalCursorPos));
        painter.restore();
    }
}

void FrameTuneScene::wheelEvent(QWheelEvent *event)
{
    if(pressed)
        return;

    QPoint numDegrees = event->angleDelta();
    if(numDegrees.y() > 0)
    {
        if(m_zoom >= 20.0)
            m_zoom = 20.0;
        else
            m_zoom += 0.5;
    }
    else if(numDegrees.y() < 0)
    {
        if(m_zoom <= 1.0)
            m_zoom = 1.0;
        else
            m_zoom -= 0.5;
    }

    if(!scrollPossible())
        curScrollOffset() = QPoint(0, 0);
    repaint();
}

void FrameTuneScene::mousePressEvent(QMouseEvent *event)
{
    if(pressed)
    {
        QWidget::mousePressEvent(event);
        return;
    }

    pressed = true;
    prevPos = event->Q_EventLocalPos() / m_zoom;
    button = event->button();

    if(button == Qt::LeftButton && m_mode != MODE_NONE)
    {
        if(m_curTool->mousePress(mapToImg(event->Q_EventLocalPos())))
            repaint();
    }

    QWidget::mousePressEvent(event);
}

void FrameTuneScene::mouseMoveEvent(QMouseEvent *event)
{
    if(!m_curTool.isNull())
        repaint();

    if(!pressed)
    {
        QWidget::mouseMoveEvent(event);
        return;
    }

    auto p = event->Q_EventLocalPos() / m_zoom;
    auto &so = curScrollOffset();
    int dX = 0, dY = 0;

    if(std::floor(std::abs(prevPos.x() - p.x())) >= 1)
    {
        dX = p.x() - prevPos.x();
        prevPos.setX(p.x());
    }

    if(std::floor(std::abs(prevPos.y() - p.y())) >= 1)
    {
        dY = p.y() - prevPos.y();
        prevPos.setY(p.y());
    }

    if(dX != 0 || dY != 0)
    {
        if(button == Qt::MiddleButton && scrollPossible())
        {
            so.setX(so.x() + dX);
            so.setY(so.y() + dY);
            repaint();
        }
        else if(button == Qt::LeftButton && m_mode != MODE_NONE)
        {
            if(m_curTool->mouseMove(mapToImg(event->Q_EventLocalPos()), QPoint(dX, dY)))
                repaint();
        }
        else
            emit delta(button, dX, dY);
    }

    QWidget::mouseMoveEvent(event);
}

void FrameTuneScene::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton && m_mode != MODE_NONE)
    {
        if(m_curTool->mouseDoubleClick(mapToImg(event->Q_EventLocalPos())))
            repaint();
    }

    QWidget::mouseDoubleClickEvent(event);
}

void FrameTuneScene::mouseReleaseEvent(QMouseEvent *event)
{
    if(!pressed || button != event->button())
    {
        QWidget::mouseReleaseEvent(event);
        return;
    }

    if(button == Qt::LeftButton && m_mode != MODE_NONE)
    {
        if(m_curTool->mouseRelease(mapToImg(event->Q_EventLocalPos())))
            repaint();
    }

    pressed = false;
    QWidget::mouseReleaseEvent(event);
    emit mouseReleased();
}
