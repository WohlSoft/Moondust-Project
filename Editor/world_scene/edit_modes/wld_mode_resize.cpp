#include "wld_mode_resize.h"

#include "../wld_scene.h"
#include "../../common_features/mainwinconnect.h"
#include "../../common_features/item_rectangles.h"

WLD_ModeResize::WLD_ModeResize(QGraphicsScene *parentScene, QObject *parent)
    : EditMode("Resize", parentScene, parent)
{}

WLD_ModeResize::~WLD_ModeResize()
{}

void WLD_ModeResize::set()
{
    if(!scene) return;
    WldScene *s = dynamic_cast<WldScene *>(scene);

    s->EraserEnabled=false;
    s->PasteFromBuffer=false;
    s->DrawMode=true;
    s->disableMoveItems=true;

    s->clearSelection();
    s->resetCursor();
    s->unserPointSelector();

    s->_viewPort->setInteractive(true);
    s->_viewPort->setCursor(Qt::ArrowCursor);
    s->_viewPort->setDragMode(QGraphicsView::NoDrag);
}

void WLD_ModeResize::mousePress(QGraphicsSceneMouseEvent *mouseEvent)
{
    Q_UNUSED(mouseEvent);
    if(!scene) return;
    WldScene *s = dynamic_cast<WldScene *>(scene);

    s->MousePressEventOnly = true;
    s->mousePressEvent(mouseEvent);
    dontCallEvent = true;
}

void WLD_ModeResize::mouseMove(QGraphicsSceneMouseEvent *mouseEvent)
{
    Q_UNUSED(mouseEvent);
    if(!scene) return;
    WldScene *s = dynamic_cast<WldScene *>(scene);
    s->clearSelection();

    s->MouseMoveEventOnly = true;
    s->mouseMoveEvent(mouseEvent);
    dontCallEvent = true;
}

void WLD_ModeResize::mouseRelease(QGraphicsSceneMouseEvent *mouseEvent)
{
    Q_UNUSED(mouseEvent);
}

void WLD_ModeResize::keyPress(QKeyEvent *keyEvent)
{
    Q_UNUSED(keyEvent);

}

void WLD_ModeResize::keyRelease(QKeyEvent *keyEvent)
{
    if(!scene) return;
    WldScene *s = dynamic_cast<WldScene *>(scene);

    switch(keyEvent->key())
    {
        case (Qt::Key_Escape):
            s->resetResizers();
            break;
        case (Qt::Key_Enter):
        case (Qt::Key_Return):
            s->applyResizers();
                //setSectionResizer(false, true);
            break;
        default:
            break;
    }
}
