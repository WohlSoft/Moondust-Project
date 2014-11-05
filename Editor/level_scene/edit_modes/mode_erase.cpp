#include "mode_erase.h"

#include "../lvl_scene.h"
#include "../../common_features/mainwinconnect.h"
#include "../../common_features/item_rectangles.h"

#include "../item_bgo.h"
#include "../item_block.h"
#include "../item_npc.h"
#include "../item_water.h"
#include "../item_playerpoint.h"
#include "../item_door.h"

#include "../../common_features/themes.h"

LVL_ModeErase::LVL_ModeErase(QGraphicsScene *parentScene, QObject *parent)
    : EditMode("Erase", parentScene, parent)
{}


LVL_ModeErase::~LVL_ModeErase()
{}


void LVL_ModeErase::set()
{
    if(!scene) return;
    LvlScene *s = dynamic_cast<LvlScene *>(scene);

    s->EraserEnabled=false;
    s->PasteFromBuffer=false;
    s->DrawMode=false;
    s->disableMoveItems=false;

    s->clearSelection();
    s->resetCursor();
    s->resetResizers();

    s->_viewPort->setInteractive(true);
    s->_viewPort->setCursor(QCursor(Themes::Image(Themes::cursor_erasing), 0, 0));
    s->_viewPort->setDragMode(QGraphicsView::RubberBandDrag);
}

void LVL_ModeErase::mousePress(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(!scene) return;
    LvlScene *s = dynamic_cast<LvlScene *>(scene);

    if( mouseEvent->buttons() & Qt::RightButton )
    {
        MainWinConnect::pMainWin->on_actionSelect_triggered();
        dontCallEvent = true;
        return;
    }

    if(s->cursor){
       s->cursor->show();
       s->cursor->setPos(mouseEvent->scenePos());
    }

    s->MousePressEventOnly = true;
    s->mousePressEvent(mouseEvent);
    dontCallEvent = true;

    QList<QGraphicsItem*> selectedList = s->selectedItems();
    if (!selectedList.isEmpty())
    {
        s->removeItemUnderCursor();
        s->Debugger_updateItemList();
        s->EraserEnabled=true;
    }
}

void LVL_ModeErase::mouseMove(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(!scene) return;
    LvlScene *s = dynamic_cast<LvlScene *>(scene);

    if(s->cursor) s->cursor->setPos(mouseEvent->scenePos());
    if (s->EraserEnabled)// Remove All items, placed under Cursor
    {
        s->removeItemUnderCursor();
        s->Debugger_updateItemList();
    }
}

void LVL_ModeErase::mouseRelease(QGraphicsSceneMouseEvent *mouseEvent)
{
    Q_UNUSED(mouseEvent);
    if(!scene) return;
    LvlScene *s = dynamic_cast<LvlScene *>(scene);

    if(!s->overwritedItems.blocks.isEmpty()||
        !s->overwritedItems.bgo.isEmpty()||
        !s->overwritedItems.npc.isEmpty() )
    {
        s->addRemoveHistory(s->overwritedItems);
        s->overwritedItems.blocks.clear();
        s->overwritedItems.bgo.clear();
        s->overwritedItems.npc.clear();
    }
    QList<QGraphicsItem*> selectedList = s->selectedItems();

    // check for grid snap
    if ((!selectedList.isEmpty())&&(s->mouseMoved))
    {
        s->removeLvlItems(selectedList);
        selectedList = s->selectedItems();
        s->Debugger_updateItemList();
    }

    s->EraserEnabled = false;
}

void LVL_ModeErase::keyPress(QKeyEvent *keyEvent)
{
    Q_UNUSED(keyEvent);
}


void LVL_ModeErase::keyRelease(QKeyEvent *keyEvent)
{
    switch(keyEvent->key())
    {
        case (Qt::Key_Escape):
            MainWinConnect::pMainWin->on_actionSelect_triggered();
            break;
        default:
            break;
    }
}
