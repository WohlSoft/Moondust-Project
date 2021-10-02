#ifndef MATRIXWIDGET_H
#define MATRIXWIDGET_H

#include <QWidget>
#include <QPixmap>
#include <QMap>
#include <QPair>

class MatrixWidget : public QWidget
{
    Q_OBJECT

    QPixmap m_texture;
    QPixmap m_check;
    QPixmap m_warn;

    int m_gridW = 10;
    int m_gridH = 10;
    int m_selectedX = 0;
    int m_selectedY = 0;
    QMap<QPair<int, int>, bool> m_enabledFrames;
    QMap<QPair<int, int>, bool> m_framesWithDefects;

public:
    explicit MatrixWidget(QWidget *parent = nullptr);

    void setTexture(const QPixmap &texture, int cellsX, int cellsY);
    void setGridSize(int w, int h);
    void setFrame(int x, int y);
    void setFrameEnabled(int x, int y, bool en);

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);

signals:
    void cellClicked(int x, int y);
    void referenceSelected(int x, int y);
    void cellToggled(int x, int y, bool en);
};

#endif // MATRIXWIDGET_H
