#ifndef MOUSESCENE_H
#define MOUSESCENE_H

#include <QGraphicsScene>

class QMouseEvent;

class MouseScene : public QGraphicsScene
{
    Q_OBJECT
    Qt::MouseButton button = Qt::NoButton;
    bool pressed = false;
    QPointF prevPos;

public:
    explicit MouseScene(QObject *parent = nullptr);

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

signals:
    void deltaX(Qt::MouseButton button, int delta);
    void deltaY(Qt::MouseButton button, int delta);
};

#endif // MOUSESCENE_H
