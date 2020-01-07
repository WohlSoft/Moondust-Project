#include "direction_switch_widget.h"

#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QHelpEvent>
#include <QToolTip>
#include <QStyleOptionFrame>


/**
 * @brief Turn a full-size rectangle into square and align it into center of given area
 * @param r Rectangle to squarize
 */
static void toSquareAndAlignToCenter(QRect &r)
{
    if(r.width() > r.height()) // If width is bigger than height
    {
        int w = r.height(); // Take height size and set it to width
        int x = (r.width() - w) / 2;
        r.setX(x); // Move to center
        r.setWidth(w);
    }
    else if(r.width() < r.height()) // If height is bigger than width
    {
        int h = r.width(); // Take width size and set it to height
        int y = (r.height() - h) / 2;
        r.setY(y); // Move to center
        r.setHeight(h);
    }
}

DirectionSwitchWidget::DirectionSwitchWidget(QWidget *parent) : QWidget(parent)
{
    setMinimumWidth(60);
    setMinimumHeight(60);
    setMouseTracking(true); // Mouse tracking is needed to highlight hovering buttons and show tooltips
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
    for(auto it = m_valuesMap.begin(); it != m_valuesMap.end(); it++) // Find a mapped value
    {
        if(it.value() == direction) // If it is matching, take as a current side
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

void DirectionSwitchWidget::unMapValue(DirectionSwitchWidget::Sides side)
{
    if(m_valuesMap.contains(side))
        m_valuesMap.remove(side);
}

void DirectionSwitchWidget::mapToolTip(DirectionSwitchWidget::Sides side, QString toolTip)
{
    m_toolTipsMap[side] = toolTip;
}

void DirectionSwitchWidget::mousePressEvent(QMouseEvent *e)
{
    if(e->button() != Qt::LeftButton)
        return;

    Sides s = findSide(e->x(), e->y()); // Find the side under mouse cursor
    if(s != S_UNKNOWN) // If anything was found, press it!
    {
        m_pressSide = s;
        update();
        repaint();
    }
}

void DirectionSwitchWidget::mouseMoveEvent(QMouseEvent *e)
{
    Sides r = findSide(e->x(), e->y()); // Find the side under mouse cursor
    if(r != m_hoverSide) // If anything was found, highlight it!
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

    // The "Button pressed" event should happen once mouse was released together with hovering a target button
    // It's possible to "cancel" the pressing by moving mouse out of a button and releasing it

    if(m_hoverSide == m_pressSide && (m_pressSide != S_UNKNOWN))
    {
        int newVal = m_valuesMap[m_hoverSide]; // Find a side value that was pressed
        m_currentSide = m_hoverSide;
        if(newVal != m_direction) // If it was REALLY changed
        {
            m_direction = newVal; // Assign and emit all value change signals
            emit directionChanged(m_direction);
            emit clicked(m_direction);
        }
    }
    m_pressSide = S_UNKNOWN; // Clear pressed state
    m_hoverSide = findSide(e->x(), e->y()); // Keep hover side same
    // Repaint widget
    update();
    repaint();
}

void DirectionSwitchWidget::leaveEvent(QEvent *)
{
    // We should reset hover state on widget leave event
    // Otherwise, it may stay "hovering" if you'll move mouse too fast
    m_hoverSide = S_UNKNOWN;
    update();
    repaint();
}

void DirectionSwitchWidget::drawButton(QPainter &painter, int cellWidth, int cellHeight,
                                       DirectionSwitchWidget::Sides side)
{
    if(m_pixmaps.contains(side)) // Find existing pixmap
    {
        const QPixmap &c = getPixMap(side); // Get a pixmap for the side
        // Align pixmap to the center of a side on a 3x3 grid
        int offsetX = (cellWidth / 2) - (c.width() / 2);
        int offsetY = (cellHeight / 2) - (c.height() / 2);
        QRect p = getRect(side); // Get the position and size of 3x3 grid cell for a widget's side
        // Finally, draw the pixmap
        painter.drawPixmap(p.left() + offsetX, p.top() + offsetY, c);
    }
}

void DirectionSwitchWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QRect r = rect();
    toSquareAndAlignToCenter(r);

    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(Qt::transparent));
    painter.drawRect(r);

    // When widget is disabled, set the 50% opacity of drawn content
    if(!isEnabled())
    {
        painter.setOpacity(0.5);
    }

    // We should know a size of each cell of 3x3 grid
    int cellWidth = r.width() / 3;
    int cellHeight = r.height() / 3;

    if(m_rotatingCenter)
    {
        // For a rotating center, we will do own logic here and we'll don't use drawButton() call
        if(m_pixmaps.contains(S_CENTER)) // Check is center pixmap available
        {
            const QPixmap &c = getPixMap(S_CENTER); // Take it
            // To avoid glitches, rememer a current state of painter first
            painter.save();
            // To correctly rotate a pixmap, move an anchor into the center of the area
            painter.translate(r.center().x() + 1, r.center().y() + 1);
            // For each side, rotate a pixmap with giving degrees values
            switch(m_currentSide)
            {
            default:
            case S_TOP:
                painter.rotate(0.0); // Pointing up is a default state of pixmap, don't rotate
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
            // After rotating of picture, we should align it
            // (the rotation is happens at left-top corner anchor by default)
            painter.drawPixmap(-c.width() / 2, -c.height() / 2, c);
            // Reset any transforms we did
            painter.resetTransform();
            // Restore painter's state that was saved before
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

    if(m_hasDiagonals) // Draw diagonals when they are enabled
    {
        drawButton(painter, cellWidth, cellHeight, S_TOP_LEFT);
        drawButton(painter, cellWidth, cellHeight, S_TOP_RIGHT);
        drawButton(painter, cellWidth, cellHeight, S_BOTTOM_LEFT);
        drawButton(painter, cellWidth, cellHeight, S_BOTTOM_RIGHT);
    }

    // Highlight a hovering but not pressed yet button
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
    // Highlight a hovering and pressed button
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

    // When widget is disabled, fill it with dotties to distort it's view
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
    toSquareAndAlignToCenter(r);

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
    toSquareAndAlignToCenter(r);
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
