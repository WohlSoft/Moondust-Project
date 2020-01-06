#include "direction_switch_widget.h"

#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QHelpEvent>
#include <QToolTip>
#include <QStyleOptionFrame>

DirectionSwitchWidget::DirectionSwitchWidget(QWidget *parent) : QWidget(parent)
{
    setMinimumWidth(60);
    setMinimumHeight(60);
    setMouseTracking(true);
}

DirectionSwitchWidget::~DirectionSwitchWidget()
{}

void DirectionSwitchWidget::setUseDiagonals(bool diagonals)
{
    m_hasDiagonals = diagonals;
    update();
    repaint();
}

bool DirectionSwitchWidget::useDiagonals()
{
    return m_hasDiagonals;
}

void DirectionSwitchWidget::setDirection(int direction)
{
    m_direction = direction;
    for(auto it = m_valuesMap.begin(); it != m_valuesMap.end(); it++)
    {
        if(it.value() == direction)
        {
            m_currentSide = it.key();
            break;
        }
    }

    emit directionChanged(m_direction);
    update();
    repaint();
}

int DirectionSwitchWidget::direction()
{
    return m_direction;
}

void DirectionSwitchWidget::setRotatingCenter(bool rc)
{
    m_rotatingCenter = rc;
    update();
    repaint();
}

bool DirectionSwitchWidget::rotatingCenter()
{
    return m_rotatingCenter;
}

void DirectionSwitchWidget::setPixmap(DirectionSwitchWidget::Sides side, const QPixmap &pixmap)
{
    m_pixmaps[side] = pixmap;
    update();
    repaint();
}

void DirectionSwitchWidget::setPixmapOn(DirectionSwitchWidget::Sides side, const QPixmap &pixmap)
{
    m_pixmapsOn[side] = pixmap;
    update();
    repaint();
}

void DirectionSwitchWidget::mapValue(DirectionSwitchWidget::Sides side, int value)
{
    m_valuesMap[side] = value;
}

void DirectionSwitchWidget::mapToolTip(DirectionSwitchWidget::Sides side, QString toolTip)
{
    m_toolTipsMap[side] = toolTip;
}

void DirectionSwitchWidget::mousePressEvent(QMouseEvent *e)
{
    if(e->button() != Qt::LeftButton)
        return;

    Sides s = findSide(e->x(), e->y());
    if(s != S_UNKNOWN)
    {
        m_pressSide = s;
        update();
        repaint();
    }
}

void DirectionSwitchWidget::mouseMoveEvent(QMouseEvent *e)
{
    Sides r = findSide(e->x(), e->y());
    if(r != m_hoverSide)
    {
        m_hoverSide = r;
        update();
        repaint();
    }
}

void DirectionSwitchWidget::mouseReleaseEvent(QMouseEvent *e)
{
    if(e->button() != Qt::LeftButton)
        return;

    if(m_hoverSide == m_pressSide && (m_pressSide != S_UNKNOWN))
    {
        int newVal = m_valuesMap[m_hoverSide];
        m_currentSide = m_hoverSide;
        if(newVal != m_direction)
        {
            m_direction = newVal;
            emit directionChanged(m_direction);
            emit clicked(m_direction);
        }
    }
    m_pressSide = S_UNKNOWN;
    m_hoverSide = findSide(e->x(), e->y());
    update();
    repaint();
}

void DirectionSwitchWidget::leaveEvent(QEvent *)
{
    m_hoverSide = S_UNKNOWN;
    update();
    repaint();
}

void DirectionSwitchWidget::drawButton(QPainter &painter, int cellWidth, int cellHeight,
                                       DirectionSwitchWidget::Sides side)
{
    if(m_pixmaps.contains(side))
    {
        const QPixmap &c = getPixMap(side);
        int offsetX = (cellWidth / 2) - (c.width() / 2);
        int offsetY = (cellHeight / 2) - (c.height() / 2);
        QRect p = getRect(side);
        painter.drawPixmap(p.left() + offsetX, p.top() + offsetY, c);
    }
}

void DirectionSwitchWidget::alignRect(QRect &r)
{
    if(r.width() > r.height())
    {
        int w = r.height();
        int x = (r.width() - w) / 2;
        r.setX(x);
        r.setWidth(w);
    }
    else if(r.width() < r.height())
    {
        int h = r.width();
        int y = (r.height() - h) / 2;
        r.setY(y);
        r.setHeight(h);
    }
}

void DirectionSwitchWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QRect r = rect();
    alignRect(r);

    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(Qt::transparent));
    painter.drawRect(r);

    if(!isEnabled())
    {
        painter.setOpacity(0.5);
    }

    int cellWidth = r.width() / 3;
    int cellHeight = r.height() / 3;

    if(m_rotatingCenter)
    {
        if(m_pixmaps.contains(S_CENTER))
        {
            const QPixmap &c = getPixMap(S_CENTER);
            painter.save();
            painter.translate(r.center().x() + 1, r.center().y() + 1);
            switch(m_currentSide)
            {
            default:
            case S_TOP:
                painter.rotate(0.0);
                break;
            case S_BOTTOM:
                painter.rotate(180.0);
                break;
            case S_LEFT:
                painter.rotate(-90.0);
                break;
            case S_RIGHT:
                painter.rotate(90.0);
                break;
            case S_TOP_LEFT:
                painter.rotate(-45);
                break;
            case S_TOP_RIGHT:
                painter.rotate(45.0);
                break;
            case S_BOTTOM_LEFT:
                painter.rotate(-90.0 - 45.0);
                break;
            case S_BOTTOM_RIGHT:
                painter.rotate(180.0 - 45.0);
                break;
            }
            painter.drawPixmap(-c.width() / 2, -c.height() / 2, c);
            painter.resetTransform();
            painter.restore();
        }
    }
    else
    {
        drawButton(painter, cellWidth, cellHeight, S_CENTER);
    }
    drawButton(painter, cellWidth, cellHeight, S_LEFT);
    drawButton(painter, cellWidth, cellHeight, S_RIGHT);
    drawButton(painter, cellWidth, cellHeight, S_TOP);
    drawButton(painter, cellWidth, cellHeight, S_BOTTOM);

    if(m_hasDiagonals)
    {
        drawButton(painter, cellWidth, cellHeight, S_TOP_LEFT);
        drawButton(painter, cellWidth, cellHeight, S_TOP_RIGHT);
        drawButton(painter, cellWidth, cellHeight, S_BOTTOM_LEFT);
        drawButton(painter, cellWidth, cellHeight, S_BOTTOM_RIGHT);
    }

    if((m_hoverSide != S_UNKNOWN) && (m_pressSide == S_UNKNOWN))
    {
        painter.save();
        QStyleOptionFrame option;
        option.initFrom(this);
        option.rect = getRect(m_hoverSide);
        option.state = QStyle::State_Raised;
        option.frameShape = QFrame::HLine;
        style()->drawPrimitive(QStyle::PE_Frame, &option, &painter, this);
        painter.restore();
    }
    else if(m_hoverSide == m_pressSide && (m_pressSide != S_UNKNOWN))
    {
        painter.save();
        QStyleOptionFrame option;
        option.initFrom(this);
        option.rect = getRect(m_hoverSide);
        option.state = QStyle::State_Sunken;
        option.frameShape = QFrame::HLine;
        style()->drawPrimitive(QStyle::PE_Frame, &option, &painter, this);
        painter.restore();
    }

    if(!isEnabled())
    {
        painter.setBrush(QBrush(Qt::black, Qt::Dense4Pattern));
        painter.drawRect(r);
    }
}

bool DirectionSwitchWidget::event(QEvent *event)
{
    if (event->type() == QEvent::ToolTip)
    {
        QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);
        Sides side = findSide(helpEvent->pos());
        if(side != S_UNKNOWN && m_toolTipsMap.contains(side))
            QToolTip::showText(helpEvent->globalPos(), m_toolTipsMap[side]);
        else
        {
            QToolTip::hideText();
            event->ignore();
        }
        return true;
    }
    return QWidget::event(event);
}

DirectionSwitchWidget::Sides DirectionSwitchWidget::findSide(int x, int y)
{
    QRect r = rect();
    alignRect(r);

    int cellWidth = r.width() / 3;
    int cellHeight = r.height() / 3;
    Sides side = S_UNKNOWN;

    if(x >= r.x() + cellWidth && x < r.x() + (cellWidth * 2) &&
       y >= r.y() && y < r.y() + cellHeight)
    {
        side = S_TOP;
    }
    else if(x >= r.x() + cellWidth && x < r.x() + (cellWidth * 2) &&
            y >= r.y() + (cellHeight * 2) && y < r.y() + (cellHeight * 3))
    {
        side = S_BOTTOM;
    }
    else if(x >= r.x() && x < r.x() + cellWidth &&
            y >= r.y() + cellHeight && y < r.y() + (cellHeight * 2))
    {
        side = S_LEFT;
    }
    else if(x >= r.x() + (cellWidth * 2) && x < r.x() + (cellWidth * 3) &&
            y >= r.y() + cellHeight && y < r.y() + (cellHeight * 2))
    {
        side = S_RIGHT;
    }

    else if(m_hasDiagonals &&
            x >= r.x() && x < r.x() + cellWidth &&
            y >= r.y() && y < r.y() + cellHeight)
    {
        side = S_TOP_LEFT;
    }
    else if(m_hasDiagonals &&
            x >= r.x() + (cellWidth * 2) && x < r.x() + (cellWidth * 3) &&
            y >= r.y() && y < r.y() + cellHeight)
    {
        side = S_TOP_RIGHT;
    }
    else if(m_hasDiagonals &&
            x >= r.x() && x < r.x() + cellWidth &&
            y >= r.y() + (cellHeight * 2) && y < r.y() + (cellHeight * 3))
    {
        side = S_BOTTOM_LEFT;
    }
    else if(m_hasDiagonals &&
            x >= r.x() + (cellWidth * 2) && x < r.x() + (cellWidth * 3) &&
            y >= r.y() + (cellHeight * 2) && y < r.y() + (cellHeight * 3))
    {
        side = S_BOTTOM_RIGHT;
    }

    if(!m_valuesMap.contains(side))
    {
        side = S_UNKNOWN;
    }

    return side;
}

DirectionSwitchWidget::Sides DirectionSwitchWidget::findSide(const QPoint &pos)
{
    return findSide(pos.x(), pos.y());
}

QRect DirectionSwitchWidget::getRect(DirectionSwitchWidget::Sides side)
{
    QRect r = rect();
    alignRect(r);
    int cellWidth = r.width() / 3;
    int cellHeight = r.height() / 3;

    switch(side)
    {
    case S_CENTER:
        return QRect(r.x() + cellWidth, r.y() + cellHeight,
                     cellWidth - 1, cellHeight - 1);
    case S_LEFT:
        return QRect(r.x(), r.y() + cellHeight,
                     cellWidth - 1, cellHeight - 1);
    case S_RIGHT:
        return QRect(r.x() + cellWidth * 2, r.y() + cellHeight,
                     cellWidth - 1, cellHeight - 1);
    case S_TOP:
        return QRect(r.x() + cellWidth, r.y(),
                     cellWidth - 1, cellHeight - 1);
    case S_BOTTOM:
        return QRect(r.x() + cellWidth, r.y() + cellHeight * 2,
                     cellWidth - 1, cellHeight - 1);
    case S_TOP_LEFT:
        return QRect(r.x(), r.y(),
                     cellWidth - 1, cellHeight - 1);
    case S_TOP_RIGHT:
        return QRect(r.x() + cellWidth * 2, r.y(),
                     cellWidth - 1, cellHeight - 1);
    case S_BOTTOM_LEFT:
        return QRect(r.x(), r.y() + cellHeight * 2,
                     cellWidth - 1, cellHeight - 1);
    case S_BOTTOM_RIGHT:
        return QRect(r.x() + cellWidth * 2, r.y() + cellHeight * 2,
                     cellWidth - 1, cellHeight - 1);
    default:
    case S_UNKNOWN:
        return QRect();
    }
}

const QPixmap &DirectionSwitchWidget::getPixMap(DirectionSwitchWidget::Sides side)
{
    if(m_currentSide == side && m_pixmapsOn.contains(side))
        return m_pixmapsOn[side];
    else
        return m_pixmaps[side];
}
