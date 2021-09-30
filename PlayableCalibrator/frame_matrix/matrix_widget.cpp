#include "matrix_widget.h"
#include <QPaintEvent>
#include <QPainter>


MatrixWidget::MatrixWidget(QWidget *parent) : QWidget(parent)
{}

void MatrixWidget::setTexture(const QPixmap &texture, int cellsX, int cellsY)
{
    m_texture = texture;
    m_gridW = cellsX;
    m_gridH = cellsY;
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

void MatrixWidget::paintEvent(QPaintEvent * /*event*/)
{
    QPainter painter(this);

    painter.fillRect(rect(), QBrush(Qt::white));

    double w = rect().width();
    double h = rect().height();
    painter.drawPixmap(rect(), m_texture);

    QColor color;
    color.setRed(255);
    color.setBlue(0);
    color.setGreen(0);
    color.setAlpha(128);

    painter.save();
    painter.setBrush(QBrush(color));
    painter.drawEllipse(QPointF(
                            (w / m_gridW) * m_selectedX + (w / m_gridW) / 2,
                            (h / m_gridH) * m_selectedY + (h / m_gridH) / 2
                        ), (w / m_gridW) / 2, (h / m_gridH) / 2);
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

    painter.restore();

    painter.end();
}
