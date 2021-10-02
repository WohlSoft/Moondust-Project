#include "frame_tune_scene.h"
#include <QPainter>
#include <QWheelEvent>
#include <QKeyEvent>
#include <cmath>


bool FrameTuneScene::scrollPossible()
{
    return m_scrollAllowed;
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

void FrameTuneScene::setImage(const QPixmap &image)
{
    m_image = image;
    if(!m_blockRepaint)
        repaint();
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
        m_scrollOffset = QPoint(0, 0);
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
    m_scrollOffset = QPoint(0, 0);
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

QSize FrameTuneScene::sizeHint() const
{
    return QSize(300, 300);
}

void FrameTuneScene::paintEvent(QPaintEvent * /*event*/)
{
    QPainter painter(this);
    painter.fillRect(rect(), QBrush(m_bgColor));

    auto canvas = rect();
    auto c = canvas.center() + (m_scrollOffset * m_zoom);
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
        painter.drawPixmap(dst, m_image);

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
}

void FrameTuneScene::wheelEvent(QWheelEvent *event)
{
    if(pressed)
        return;

    QPoint numDegrees = event->angleDelta();
    if(numDegrees.y() > 0)
    {
        if(m_zoom >= 10.0)
            m_zoom = 10.0;
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
        m_scrollOffset = QPoint(0, 0);
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
    prevPos = event->localPos() / m_zoom;
    button = event->button();
    QWidget::mousePressEvent(event);
}

void FrameTuneScene::mouseMoveEvent(QMouseEvent *event)
{
    if(!pressed)
    {
        QWidget::mouseMoveEvent(event);
        return;
    }

    auto p = event->localPos() / m_zoom;

    if(std::floor(std::abs(prevPos.x() - p.x())) >= 1)
    {
        if(button == Qt::MidButton && scrollPossible())
        {
            m_scrollOffset.setX(m_scrollOffset.x() + p.x() - prevPos.x());
            repaint();
        }
        else
            emit deltaX(button, p.x() - prevPos.x());
        prevPos.setX(p.x());
    }

    if(std::floor(std::abs(prevPos.y() - p.y())) >= 1)
    {
        if(button == Qt::MidButton && scrollPossible())
        {
            m_scrollOffset.setY(m_scrollOffset.y() + p.y() - prevPos.y());
            repaint();
        }
        else
            emit deltaY(button, p.y() - prevPos.y());
        prevPos.setY(p.y());
    }

    QWidget::mouseMoveEvent(event);
}

void FrameTuneScene::mouseReleaseEvent(QMouseEvent *event)
{
    if(!pressed || button != event->button())
    {
        QWidget::mouseReleaseEvent(event);
        return;
    }

    pressed = false;
    QWidget::mouseReleaseEvent(event);
    emit mouseReleased();
}
