#ifndef GRAPHICSWORKSPACE_H
#define GRAPHICSWORKSPACE_H

#include <QGraphicsView>
#include <QTimer>
#include <QScrollBar>
#include <QKeyEvent>

class GraphicsWorkspace : public QGraphicsView
{
    Q_OBJECT
public:
    explicit GraphicsWorkspace(QWidget *parent = 0);
    qreal zoom();
    void setZoom(qreal zoom);

signals:

public slots:

private slots:
    void moveLeft();
    void moveRight();
    void moveUp();
    void moveDown();
    void updateTimerInterval();

private:
    QTimer lMover;
    QTimer rMover;
    QTimer uMover;
    QTimer dMover;
    int step;
    int keyTime;
    qreal zoomValue;

protected:
    virtual void 	focusOutEvent ( QFocusEvent * event );

    virtual void 	keyPressEvent ( QKeyEvent * event );
    virtual void 	keyReleaseEvent ( QKeyEvent * event );
    virtual void 	wheelEvent ( QWheelEvent * event );

};

#endif // GRAPHICSWORKSPACE_H
