#include "frame_tune_scene.h"
#include <QApplication>
#include <QPainter>
#include <QWheelEvent>
#include <QClipboard>
#include <QKeyEvent>
#include <QtDebug>
#include <cmath>
#include <pge_qt_compat.h>


class DrawTool : public QObject
{
protected:
    QImage *m_image;
    FrameTuneScene *m_self;

    static QPoint to2pix(const QPoint p)
    {
        return (p / 2) - QPoint(1, 1);
    }

    void drawCursor(QPainter &p, const QRect& dst, double zoom, const QPoint &pos)
    {
        if(zoom <= 1.0 && !m_self->m_2pixDrawMode)
            p.drawPoint(pos);
        else
        {
            if(m_self->m_2pixDrawMode)
            {
                QPoint p2 = to2pix(pos);
                p.drawRect(QRectF(dst.x() + p2.x() * zoom * 2,
                                  dst.y() + p2.y() * zoom * 2,
                                  zoom * 2,
                                  zoom * 2
                           ));
            }
            else
            {
                p.drawRect(QRectF(dst.x() + pos.x() * zoom,
                                  dst.y() + pos.y() * zoom,
                                  zoom,
                                  zoom
                           ));
            }
        }
    }

    QPoint roundPoint(QPoint s)
    {
        s.setX(static_cast<int>(std::round(s.x() / 2.0f)) * 2);
        s.setY(static_cast<int>(std::round(s.y() / 2.0f)) * 2);
        return s;
    }

public:
    explicit DrawTool(QImage *image, FrameTuneScene *parent = 0) :
        QObject(parent),
        m_image(image),
        m_self(parent)
    {}

    virtual ~DrawTool() {}

    virtual bool toolIsBusy() = 0;

    virtual void cancel() {}
    virtual bool apply() { return false; }

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
    QImage m_buf2x;
public:
    explicit DrawToolPencil(QImage *image, FrameTuneScene *parent = 0) :
        DrawTool(image, parent)
    {}

    bool toolIsBusy() override
    {
        return false;
    }

    virtual bool mousePress(const QPoint &p) override
    {
        startAt = p;
        if(m_self->m_2pixDrawMode)
        {
            QPoint p2 = to2pix(p);
            m_buf2x = QImage(m_image->size() / 2, QImage::Format_RGBA8888);
            m_buf2x.fill(QColor(0, 0, 0, 0));
            m_buf2x.setPixel(p2.x(), p2.y(), m_self->m_drawColor.toRgb().rgba());
            QPainter pa(m_image);
            pa.drawImage(0, 0, m_buf2x.scaled(m_image->size(), Qt::KeepAspectRatio, Qt::FastTransformation));
            pa.end();
        }
        else
            m_image->setPixel(p.x(), p.y(), m_self->m_drawColor.toRgb().rgba());
        return true;
    }

    virtual bool mouseMove(const QPoint &p, const QPoint &) override
    {
        if(m_self->m_2pixDrawMode)
        {
            QPainter pa(&m_buf2x);
            pa.setPen(m_self->m_drawColor);
            pa.drawLine(to2pix(startAt), to2pix(p));
            pa.end();
            QPainter pad(m_image);
            pad.drawImage(0, 0, m_buf2x.scaled(m_image->size(), Qt::KeepAspectRatio, Qt::FastTransformation));
            pad.end();
        }
        else
        {
            QPainter pa(m_image);
            pa.setPen(m_self->m_drawColor);
            pa.drawLine(startAt, p);
            pa.end();
        }

        startAt = p;
        return true;
    }

    virtual bool mouseRelease(const QPoint &) override
    {
        return false;
    }

    virtual void drawPreview(QPainter &p, const QRect& dst, double zoom, const QPoint &pos) override
    {
        p.setPen(QPen(Qt::gray, 1));
        p.setBrush(m_self->m_drawColor);
        drawCursor(p, dst, zoom, pos);
    }
};

class DrawToolPicker : public DrawTool
{
public:
    explicit DrawToolPicker(QImage *image, FrameTuneScene *parent = 0) :
        DrawTool(image, parent)
    {}

    bool toolIsBusy() override
    {
        return false;
    }

    bool mousePress(const QPoint &) override
    {
        return false;
    }

    bool mouseMove(const QPoint &, const QPoint &) override
    {
        return false;
    }

    bool mouseRelease(const QPoint &p) override
    {
        m_self->m_drawColor.setRgba(m_image->pixel(p.x(), p.y()));
        emit m_self->drawColourChanged(m_self->m_drawColor);
        return true;
    }

    void drawPreview(QPainter &p, const QRect& dst, double zoom, const QPoint &pos) override
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
    QImage m_buf2x;

public:
    explicit DrawToolRubber(QImage *image, FrameTuneScene *parent = 0) :
        DrawTool(image, parent)
    {}

    bool toolIsBusy() override
    {
        return false;
    }

    bool mousePress(const QPoint &p) override
    {
        startAt = p;
        if(m_self->m_2pixDrawMode)
        {
            QPoint p2 = to2pix(p);
            m_buf2x = m_image->scaled(m_image->size() / 2, Qt::KeepAspectRatio, Qt::FastTransformation);
            m_buf2x.setPixel(p2.x(), p2.y(), 0x00000000);
            QPainter pa(m_image);
            pa.setCompositionMode(QPainter::CompositionMode_Source);
            pa.drawImage(0, 0, m_buf2x.scaled(m_image->size(), Qt::KeepAspectRatio, Qt::FastTransformation));
            pa.end();
        }
        else
        {
            m_image->setPixel(p.x(), p.y(), 0x00000000);
        }
        return true;
    }

    bool mouseMove(const QPoint &p, const QPoint &) override
    {
        if(m_self->m_2pixDrawMode)
        {
            QPainter pa(&m_buf2x);
            pa.setCompositionMode(QPainter::CompositionMode_Source);
            pa.setPen(QColor(0, 0, 0, 0));
            pa.drawLine(to2pix(startAt), to2pix(p));
            pa.end();
            QPainter pad(m_image);
            pad.setCompositionMode(QPainter::CompositionMode_Source);
            pad.drawImage(0, 0, m_buf2x.scaled(m_image->size(), Qt::KeepAspectRatio, Qt::FastTransformation));
            pad.end();
        }
        else
        {
            QPainter pa(m_image);
            pa.setCompositionMode(QPainter::CompositionMode_Source);
            pa.setPen(Qt::transparent);
            pa.drawLine(startAt, p);
            pa.end();
        }

        startAt = p;

        return true;
    }

    bool mouseRelease(const QPoint &) override
    {
        return false;
    }

    void drawPreview(QPainter &p, const QRect& dst, double zoom, const QPoint &pos) override
    {
        p.setPen(QPen(Qt::gray, 1));
        p.setBrush(Qt::transparent);
        drawCursor(p, dst, zoom, pos);
    }
};


class DrawToolLine : public DrawTool
{
    QPoint startAt;
    QImage demo;
    bool m_drawing = false;

public:
    explicit DrawToolLine(QImage *image, FrameTuneScene *parent = 0) :
        DrawTool(image, parent)
    {}

    bool toolIsBusy() override
    {
        return false;
    }

    bool mousePress(const QPoint &p) override
    {
        startAt = p;

        if(m_self->m_2pixDrawMode)
        {
            demo = QImage(m_image->size() / 2, QImage::Format_ARGB32);
            demo.fill(Qt::transparent);
            demo.setPixel(to2pix(p), m_self->m_drawColor.toRgb().rgba());
        }
        else
        {
            demo = QImage(m_image->size(), QImage::Format_ARGB32);
            demo.fill(Qt::transparent);
            demo.setPixel(p.x(), p.y(), m_self->m_drawColor.toRgb().rgba());
        }

        m_drawing = true;

        return true;
    }

    bool mouseMove(const QPoint &p, const QPoint &) override
    {
        if(!m_drawing)
            return false;

        if(m_self->m_2pixDrawMode)
        {
            demo = QImage(m_image->size() / 2, QImage::Format_ARGB32);
            demo.fill(Qt::transparent);
            QPainter pa(&demo);
            pa.setPen(m_self->m_drawColor);
            pa.drawLine(to2pix(startAt), to2pix(p));
            pa.end();
        }
        else
        {
            demo = QImage(m_image->size(), QImage::Format_ARGB32);
            demo.fill(Qt::transparent);
            QPainter pa(&demo);
            pa.setPen(m_self->m_drawColor);
            pa.drawLine(startAt, p);
            pa.end();
        }
        return true;
    }

    bool mouseRelease(const QPoint &) override
    {
        if(!m_drawing)
            return false;

        QPainter pa(m_image);
        pa.setPen(m_self->m_drawColor);

        if(m_self->m_2pixDrawMode)
            pa.drawImage(0, 0, demo.scaled(demo.size() * 2, Qt::KeepAspectRatio, Qt::FastTransformation));
        else
            pa.drawImage(0, 0, demo);

        demo = QImage();
        m_drawing = false;

        return true;
    }

    void drawPreview(QPainter &p, const QRect& dst, double zoom, const QPoint &pos) override
    {
        p.save();
        p.setPen(QPen(Qt::gray, 1));
        p.setBrush(m_self->m_drawColor);

        if(!m_drawing)
            drawCursor(p, dst, zoom, pos);

        if(!m_drawing || demo.isNull())
        {
            p.restore();
            return;
        }

        if(m_self->m_2pixDrawMode)
            p.drawImage(dst, demo.scaled(demo.size() * 2, Qt::KeepAspectRatio, Qt::FastTransformation));
        else
            p.drawImage(dst, demo);

        p.restore();
    }
};

class DrawToolRect : public DrawTool
{
    QPoint startAt;
    QImage demo;
    bool m_drawing = false;

public:
    explicit DrawToolRect(QImage *image, FrameTuneScene *parent = 0) :
        DrawTool(image, parent)
    {}

    bool toolIsBusy() override
    {
        return false;
    }

    bool mousePress(const QPoint &p) override
    {
        startAt = p;

        if(m_self->m_2pixDrawMode)
        {
            demo = QImage(m_image->size() / 2, QImage::Format_ARGB32);
            demo.fill(Qt::transparent);
            demo.setPixel(to2pix(p), m_self->m_drawColor.toRgb().rgba());
        }
        else
        {
            demo = QImage(m_image->size(), QImage::Format_ARGB32);
            demo.fill(Qt::transparent);
            demo.setPixel(p.x(), p.y(), m_self->m_drawColor.toRgb().rgba());
        }

        m_drawing = true;

        return true;
    }

    bool mouseMove(const QPoint &p, const QPoint &) override
    {
        if(!m_drawing)
            return false;

        if(m_self->m_2pixDrawMode)
        {
            demo = QImage(m_image->size() / 2, QImage::Format_ARGB32);
            demo.fill(Qt::transparent);
            QPainter pa(&demo);
            pa.setPen(m_self->m_drawColor);
            pa.drawRect(QRect(to2pix(startAt), to2pix(p) - QPoint(1, 1)));
            pa.end();
        }
        else
        {
            demo = QImage(m_image->size(), QImage::Format_ARGB32);
            demo.fill(Qt::transparent);
            QPainter pa(&demo);
            pa.setPen(m_self->m_drawColor);
            pa.drawRect(QRect(startAt, p - QPoint(1, 1)));
            pa.end();
        }

        return true;
    }

    bool mouseRelease(const QPoint &) override
    {
        if(!m_drawing)
            return false;

        QPainter pa(m_image);
        pa.setPen(m_self->m_drawColor);

        if(m_self->m_2pixDrawMode)
            pa.drawImage(0, 0, demo.scaled(demo.size() * 2, Qt::KeepAspectRatio, Qt::FastTransformation));
        else
            pa.drawImage(0, 0, demo);

        pa.end();
        demo = QImage();
        m_drawing = false;

        return true;
    }

    void drawPreview(QPainter &p, const QRect& dst, double zoom, const QPoint &pos) override
    {
        p.save();
        p.setPen(QPen(Qt::gray, 1));
        p.setBrush(m_self->m_drawColor);

        if(!m_drawing)
            drawCursor(p, dst, zoom, pos);

        if(!m_drawing || demo.isNull())
        {
            p.restore();
            return;
        }

        if(m_self->m_2pixDrawMode)
            p.drawImage(dst, demo.scaled(demo.size() * 2, Qt::KeepAspectRatio, Qt::FastTransformation));
        else
            p.drawImage(dst, demo);

        p.restore();
    }
};

class DrawToolSelect : public DrawTool
{
    QPoint startAt;
    QImage floating;
    QRect  selection;
    QRect  origRect;
    const bool m_copyMode = false;

    enum State
    {
        STATE_SELECT = 0,
        STATE_DRAW,
        STATE_MOVE,
        STATE_FLOAT,
        STATE_RESET
    } m_state = STATE_SELECT;

public:
    explicit DrawToolSelect(bool copyMode, QImage *image, FrameTuneScene *parent = 0) :
        DrawTool(image, parent),
        m_copyMode(copyMode)
    {}

    bool toolIsBusy() override
    {
        return m_state == STATE_FLOAT || m_state == STATE_MOVE;
    }

    void cancel() override
    {
        m_state = STATE_SELECT;
        floating = QImage();
    }

    bool apply()  override
    {
        if(!floating.isNull())
        {
            m_state = STATE_RESET;
            QPainter p(m_image);

            if(!m_copyMode) // Erase original area
            {
                p.setCompositionMode(QPainter::CompositionMode_Source);
                p.setPen(Qt::NoPen);
                p.setBrush(Qt::transparent);
                p.drawRect(origRect);
            }

            p.setCompositionMode(QPainter::CompositionMode_SourceOver);
            p.drawImage(selection.topLeft(), floating);
            p.end();
            floating = QImage();
            return true;
        }

        return false;
    }

    virtual ~DrawToolSelect()
    {
        DrawToolSelect::cancel();
    }

    bool mousePress(const QPoint &p) override
    {
        startAt = m_self->m_2pixDrawMode ? roundPoint(p) : p;

        if(m_state == STATE_SELECT)
        {
            selection = origRect = QRect(startAt, m_self->m_2pixDrawMode ? roundPoint(p) : p);
            m_state = STATE_DRAW;
        }
        else if(m_state == STATE_FLOAT)
            m_state = STATE_MOVE;

        return true;
    }

    bool mouseMove(const QPoint &p, const QPoint &delta) override
    {
        if(m_state == STATE_RESET || m_state == STATE_SELECT)
            return false;

        QPoint pd = (m_self->m_2pixDrawMode) ? roundPoint(p) : p;
        QPoint pdelta = (m_self->m_2pixDrawMode) ? roundPoint(delta) : delta;

        if(m_state == STATE_DRAW)
        {
            selection.setLeft(startAt.x() > pd.x() ? pd.x() : startAt.x());
            selection.setTop(startAt.y() > pd.y() ? pd.y() : startAt.y());
            selection.setRight((startAt.x() <= pd.x() ? pd.x() : startAt.x()) - 1);
            selection.setBottom((startAt.y() <= pd.y() ? pd.y() : startAt.y()) - 1);
            origRect = selection;
        }
        else if(m_state == STATE_MOVE)
        {
            selection.translate(pdelta);
            startAt = pd;
        }

        return true;
    }

    bool mouseDoubleClick(const QPoint &) override
    {
        return DrawToolSelect::apply();
    }

    bool mouseRelease(const QPoint &) override
    {
        if(m_state == STATE_DRAW)
        {
            if(selection.isNull() || selection.width() == 0 || selection.height() == 0)
            {
                m_state = STATE_SELECT;
                return true;
            }

            if(selection.intersects(m_image->rect()))
            {
                floating = m_image->copy(selection);
                m_state = STATE_FLOAT;
                return true;
            }

            m_state = STATE_SELECT;
        }
        else if(m_state == STATE_MOVE)
        {
            m_state = STATE_FLOAT;
        }
        else if(m_state == STATE_RESET)
        {
            m_state = STATE_SELECT;
        }

        return false;
    }

    void drawPreview(QPainter &p, const QRect &dst, double zoom, const QPoint &) override
    {
        auto r = QRectF(dst.x() + selection.x() * zoom,
                        dst.y() + selection.y() * zoom,
                        selection.width() * zoom,
                        selection.height() * zoom);

        auto o = QRectF(dst.x() + origRect.x() * zoom,
                        dst.y() + origRect.y() * zoom,
                        origRect.width() * zoom,
                        origRect.height() * zoom);

        if(m_state == STATE_DRAW)
        {
            p.setPen(QPen(Qt::red, 2, Qt::DotLine));
            p.drawRect(r);
        }
        else if((m_state == STATE_FLOAT || m_state == STATE_MOVE) && !floating.isNull())
        {
            if(!m_copyMode)
            {
                p.setCompositionMode(QPainter::CompositionMode_Source);
                p.setPen(Qt::NoPen);

                if(m_self->m_backgroundChess)
                    m_self->drawChess(p, o.toRect());
                else
                {
                    p.setBrush(m_self->m_bgColor);
                    p.drawRect(o);
                }
            }

            p.setCompositionMode(QPainter::CompositionMode_SourceOver);
            p.setPen(QPen(Qt::cyan, 4, Qt::DotLine));
            p.setBrush(Qt::transparent);
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
#ifdef DEBUG_BUILD
    qDebug() << "x=" << o.x() << "y=" << o.y();
#endif

    return o;
}

QPointF FrameTuneScene::zoomedPos(const QPointF &p)
{
    return (p - QPointF(m_zoom / 2.0, m_zoom / 2.0)) / m_zoom;
}

void FrameTuneScene::drawChess(QPainter &painter, const QRect &r)
{
    int gridSize = 8;
    int w = r.width() / gridSize;
    int h = r.height() / gridSize;
    // Ensure drawn chess is matches the background one (ensure the accurate offset!)
    int xBeg = r.x() - std::abs(r.x() % (gridSize * 2));
    int yBeg = r.y() - std::abs(r.y() % (gridSize * 2));

    painter.save();
    painter.setClipRect(r);
    painter.setPen(Qt::NoPen);

    for(int y = 0; y <= h + 2; ++y)
    {
        for(int x = 0; x <= w + 2; ++x)
        {
            QColor colour = ((x + (y % 2)) % 2) ? Qt::darkGray : Qt::gray;
            painter.fillRect(xBeg + x * gridSize, yBeg + y * gridSize, gridSize, gridSize, QBrush(colour));
        }
    }

    painter.restore();
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

void FrameTuneScene::set2pixDrawMode(bool en)
{
    m_2pixDrawMode = en;
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

void FrameTuneScene::setDrawColour(QColor clr)
{
    m_drawColor = clr;
    emit drawColourChanged(m_drawColor);
    if(!m_blockRepaint)
        repaint();
}

void FrameTuneScene::setBgChess(bool chess)
{
    m_backgroundChess = chess;
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

QColor FrameTuneScene::getDrawColour() const
{
    return m_drawColor;
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

bool FrameTuneScene::isToolBusy() const
{
    return m_mode != MODE_NONE && m_curTool->toolIsBusy();
}

void FrameTuneScene::cancelTool()
{
    if(!m_curTool.isNull() && m_curTool->toolIsBusy())
    {
        m_curTool->cancel();
        repaint();
    }
}

FrameTuneScene::Mode FrameTuneScene::mode() const
{
    return (Mode)m_mode;
}

void FrameTuneScene::runAction(Actions action)
{
    switch(action)
    {
    default:
    case ACTION_NOTHING:
        break;

    case ACTION_COPY_FRAME:
        qApp->clipboard()->setImage(m_image);
        break;

    case ACTION_COPY_FRAME_2X_SHRINK:
    {
        qApp->clipboard()->setImage(m_image.scaled(m_image.size() / 2, Qt::KeepAspectRatio, Qt::FastTransformation));
        break;
    }

    case ACTION_PASTE_FRAME:
        if(!qApp->clipboard()->image().isNull())
        {
            QImage i = qApp->clipboard()->image();
            QPainter p(&m_image);
            p.setCompositionMode(QPainter::CompositionMode_Source);
            int dstX = (i.width() / 2) - (i.width() / 2);
            int dstY = (i.height() / 2) - (i.height() / 2);
            p.drawImage(dstX, dstY, i);
            p.end();
            repaint();
            emit actionFramePasted();
        }
        break;

    case ACTION_PASTE_FRAME_2X_GROW:
        if(!qApp->clipboard()->image().isNull())
        {
            QImage i = qApp->clipboard()->image();
            QPainter p(&m_image);
            p.setCompositionMode(QPainter::CompositionMode_Source);
            int dstX = (i.width() / 2) - (i.width() / 2);
            int dstY = (i.height() / 2) - (i.height() / 2);
            p.drawImage(dstX, dstY, i.scaled(i.size() * 2, Qt::KeepAspectRatio, Qt::FastTransformation));
            p.end();
            repaint();
            emit actionFramePasted();
        }
        break;


    case ACTION_HFLIP_CUR_FRAME:
    {
        int w = m_image.width();
        int h = m_image.height();
        QRgb pix;

        for(int xl = 0, xr = w - 1; xl < xr; ++xl, --xr)
        {
            for(int y = 0; y < h; ++y)
            {
                pix = m_image.pixel(xr, y);
                m_image.setPixel(xr, y, m_image.pixel(xl, y));
                m_image.setPixel(xl, y, pix);
            }
        }

        repaint();
        emit actionFrameFlipedH();
        break;
    }

    case ACTION_VFLIP_CUR_FRAME:
    {
        int w = m_image.width();
        int h = m_image.height();
        QRgb pix;

        for(int yt = 0, yb = w - 1; yt < yb; ++yt, --yb)
        {
            for(int x = 0; x < h; ++x)
            {
                pix = m_image.pixel(x, yb);
                m_image.setPixel(x, yb, m_image.pixel(x, yt));
                m_image.setPixel(x, yt, pix);
            }
        }

        repaint();
        emit actionFrameFlipedV();
        break;
    }

    case ACTION_SMBX64_LEFT_TO_RIGHT:
        emit actionMirrorSMBX(-1);
        break;

    case ACTION_SMBX64_RIGHT_TO_LEFT:
        emit actionMirrorSMBX(+1);
        break;

    case ACTION_ERASE_UNUSED:
        emit actionUnusedErased();
        break;

    case ACTION_ERASE_FRAME:
    {
        QPainter p(&m_image);
        p.setCompositionMode(QPainter::CompositionMode_Source);
        p.fillRect(m_image.rect(), QColor(0, 0, 0, 0));
        p.end();
        repaint();
        emit actionFramePasted();
        break;
    }

    case ACTION_FIX_2PIX:
    {
        QImage i = m_image.scaled(m_image.size() / 2, Qt::KeepAspectRatio, Qt::FastTransformation);
        QPainter p(&m_image);
        p.setCompositionMode(QPainter::CompositionMode_Source);
        p.drawImage(0, 0, i.scaled(i.size() * 2, Qt::KeepAspectRatio, Qt::FastTransformation));
        p.end();
        repaint();
        emit actionFramePasted();
        break;
    }
    }
}

QSize FrameTuneScene::sizeHint() const
{
    return QSize(300, 300);
}

void FrameTuneScene::paintEvent(QPaintEvent * /*event*/)
{
    QPainter painter(this);
    painter.fillRect(rect(), QBrush(m_bgColor));

    if(m_backgroundChess)
        drawChess(painter, rect());

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
        int y = 0, x_begin = 0, x_end = 0;
        int wall_x = 0, wall_y_begin = 0, wall_y_end = 0;
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
            abort();
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


    }

    if(!m_ref.isNull())
    {
        painter.save();
        painter.setOpacity(m_refOpacity);
        painter.drawPixmap(dst, m_ref);
        painter.restore();
    }

    if(!m_curTool.isNull())
    {
        QPoint globalCursorPos = mapFromGlobal(QCursor::pos());
        painter.save();
        m_curTool->drawPreview(painter, dst, m_zoom, mapToImg(globalCursorPos));
        painter.restore();
    }

    if(!m_image.isNull() && m_drawGrid && m_drawMetaData)
    {
        QColor gridColour = m_backgroundChess ? Qt::black : Qt::gray;
        painter.save();
        painter.setPen(QPen(gridColour, 1));
        painter.setBrush(Qt::transparent);
        painter.drawRect(dst);
        painter.restore();

        painter.save();
        painter.setOpacity(5.0);
        painter.setPen(QPen(gridColour, 1, Qt::DotLine));

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
    prevPos = zoomedPos(event->Q_EventLocalPos());
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

    auto p = zoomedPos(event->Q_EventLocalPos());
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
