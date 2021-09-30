#include "mouse_scene.h"
#include <QGraphicsSceneMouseEvent>
#include <cmath>


MouseScene::MouseScene(QObject *parent) : QGraphicsScene(parent)
{}

void MouseScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(pressed)
    {
        QGraphicsScene::mousePressEvent(event);
        return;
    }

    pressed = true;
    prevPos = event->scenePos();
    button = event->button();
    QGraphicsScene::mousePressEvent(event);
}

void MouseScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(!pressed)
    {
        QGraphicsScene::mouseMoveEvent(event);
        return;
    }

    auto p = event->scenePos();

    if(std::floor(std::abs(prevPos.x() - p.x())) >= 1)
    {
        emit deltaX(button, p.x() - prevPos.x());
        prevPos.setX(p.x());
    }

    if(std::floor(std::abs(prevPos.y() - p.y())) >= 1)
    {
        emit deltaY(button, p.y() - prevPos.y());
        prevPos.setY(p.y());
    }

    QGraphicsScene::mouseMoveEvent(event);
}

void MouseScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if(!pressed || button != event->button())
    {
        QGraphicsScene::mouseReleaseEvent(event);
        return;
    }

    pressed = false;
    QGraphicsScene::mouseReleaseEvent(event);
}
