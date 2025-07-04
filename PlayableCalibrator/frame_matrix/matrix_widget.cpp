#include "matrix_widget.h"
#include <pge_qt_compat.h>
#include <QPaintEvent>
#include <QPainter>
#include <cmath>


MatrixWidget::MatrixWidget(QWidget *parent) : QWidget(parent)
{
    m_check = QPixmap(":/images/enabled.png");
    m_warn = QPixmap(":/images/warn.png");
}

void MatrixWidget::setTexture(const QPixmap &texture, int cellsX, int cellsY)
{
    m_texture = texture;
    m_gridW = cellsX;
    m_gridH = cellsY;

    int cw = (m_texture.width() / m_gridW);
    int ch = (m_texture.height() / m_gridH);

    const QImage t = m_texture.toImage();

    // Validate the 2pix-compatibility
    for(int cy = 0; cy < m_gridH; ++cy)
    {
        for(int cx = 0; cx < m_gridW; ++cx)
        {
            int px = cw * cx;
            int py = ch * cy;
            int plx = px + cw;
            int ply = py + ch;

            bool cellInvalid = false;

            for(int y = py; y < ply; y += 2)
            {
                for(int x = px; x < plx; x += 2)
                {
                    QRgb quad[4] = {t.pixel(x, y), t.pixel(x + 1, y), t.pixel(x, y + 1), t.pixel(x + 1, y + 1)};
                    if(quad[0] != quad[1] || quad[0] != quad[2] || quad[0] != quad[3])
                        cellInvalid = true;
                }
            }

            m_framesWithDefects[{cx, cy}] = cellInvalid;
        }
    }

    repaint();
}

void MatrixWidget::setGridSize(int w, int h)
{
    m_gridW = w;
    m_gridH = h;
    repaint();
}

void MatrixWidget::setFrame(int x, int y)
{
   m_selectedX = x;
   m_selectedY = y;
   repaint();
}

void MatrixWidget::resetFramesEnabled()
{
    m_enabledFrames.clear();
    repaint();
}

void MatrixWidget::setFrameEnabled(int x, int y, bool en)
{
    m_enabledFrames[{x, y}] = en;
    repaint();
}

void MatrixWidget::paintEvent(QPaintEvent * /*event*/)
{
    QPainter painter(this);

    painter.fillRect(rect(), QBrush(Qt::white));

    double w = rect().width();
    double h = rect().height();
    double cellW = (w / m_gridW);
    double cellH = (h / m_gridH);
    painter.drawPixmap(rect(), m_texture);

    QColor color;
    color.setRed(255);
    color.setBlue(0);
    color.setGreen(0);
    color.setAlpha(128);

    painter.save();
    painter.setBrush(QBrush(color));
    painter.drawEllipse(QPointF(cellW * m_selectedX + cellW / 2, double(cellH) * m_selectedY + cellH / 2), cellW / 2, cellH / 2);
    painter.restore();

    double ws = w / m_gridW;
    double hs = h / m_gridH;

    painter.save();
    painter.setPen(QPen(Qt::black, 1));

    if(m_gridW > 1)
    {
        for(int i = 1; i <= m_gridW - 1; i++)
            painter.drawLine(QPointF(ws * i, 0.0), QPointF(ws * i, h));

    }

    if(m_gridH > 1)
    {
        for(int i = 1; i <= m_gridH - 1; i++)
            painter.drawLine(QPointF(0.0, hs * i), QPointF(w, hs * i));
    }

    for(auto it = m_enabledFrames.begin(); it != m_enabledFrames.end(); ++it)
    {
        if(it.value())
        {
            int cx = cellW * it.key().first + cellW - m_check.width() - 4;
            int cy = cellH * it.key().second + 4;
            painter.drawPixmap(cx, cy, m_check);
        }
    }

    for(auto it = m_framesWithDefects.begin(); it != m_framesWithDefects.end(); ++it)
    {
        if(it.value())
        {
            int cx = cellW * it.key().first  + 4;
            int cy = cellH * it.key().second + 4;
            painter.drawPixmap(cx, cy, m_warn );
        }
    }

    painter.restore();

    painter.end();
}

void MatrixWidget::mouseReleaseEvent(QMouseEvent *event)
{
    auto p = event->Q_EventLocalPos();
    auto r = rect();
    int x = std::floor(m_gridW * (p.x() / r.width()));
    int y = std::floor(m_gridH * (p.y() / r.height()));

    if(x >= 0 && x < m_gridW && y >= 0 && y < m_gridH)
    {
        if(event->button() == Qt::LeftButton)
            emit cellClicked(x, y);
        else if(event->button() == Qt::MiddleButton)
            emit referenceSelected(x, y);
        else if(event->button() == Qt::RightButton)
        {
            bool en = false;
            if(m_enabledFrames.contains({x, y}))
                en = m_enabledFrames[{x, y}];
            emit cellToggled(x, y, !en);
        }
    }

    QWidget::mouseReleaseEvent(event);
}
