#include "key_dropper.h"
#include "itemscene.h"

#include <QKeyEvent>
#include <QApplication>

KeyDropper::KeyDropper(QWidget *parent) :
    QDockWidget(parent)
{}

void KeyDropper::keyPressEvent(QKeyEvent *event)
{
    if(parent())
    {
        ItemScene* s = qobject_cast<ItemScene*>(parent());
        s->keyPressEvent(event);
        qApp->setActiveWindow(s);
        s->setFocus();
        return;
    }
    QDockWidget::keyPressEvent(event);
}

void KeyDropper::keyReleaseEvent(QKeyEvent *event)
{
//    if(parent())
//    {
//        ItemScene* s = qobject_cast<ItemScene*>(parent());
//        s->keyReleaseEvent(event);
//    }
    QDockWidget::keyReleaseEvent(event);
}
