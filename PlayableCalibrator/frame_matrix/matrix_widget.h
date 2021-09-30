#ifndef MATRIXWIDGET_H
#define MATRIXWIDGET_H

#include <QWidget>
#include <QPixmap>

class MatrixWidget : public QWidget
{
    Q_OBJECT

    QPixmap m_texture;
    int m_gridW = 10;
    int m_gridH = 10;
    int m_selectedX = 0;
    int m_selectedY = 0;

public:
    explicit MatrixWidget(QWidget *parent = nullptr);

    void setTexture(const QPixmap &texture, int cellsX, int cellsY);
    void setGridSize(int w, int h);
    void setFrame(int x, int y);

protected:
    void paintEvent(QPaintEvent *event);
};

#endif // MATRIXWIDGET_H
