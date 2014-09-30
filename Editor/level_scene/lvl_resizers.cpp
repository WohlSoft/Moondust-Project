/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "lvlscene.h"
#include "../edit_level/level_edit.h"
#include <QtMath>

#include "item_block.h"
#include "item_water.h"

#include "../common_features/mainwinconnect.h"
#include "../common_features/grid.h"
#include "lvl_item_placing.h"

#include "../file_formats/file_formats.h"

#include "../common_features/item_rectangles.h"


void LvlScene::applyResizers()
{
    if(pResizer!=NULL )
    {
        switch(pResizer->type)
        {
        case 4:
            setScreenshotSelector(false, true);
            break;
        case 3:
            setPhysEnvResizer(NULL, false, true);
            break;
        case 2:
            setBlockResizer(NULL, false, true);
            break;
        case 1:
            setEventSctSizeResizer(-1, false, true);
            break;
        case 0:
        default:
            MainWinConnect::pMainWin->on_applyResize_clicked();
        }
    }
}


void LvlScene::resetResizers()
{
    if(pResizer!=NULL )
    {
        switch(pResizer->type)
        {
        case 4:
            setScreenshotSelector(false, false);
            break;
        case 3:
            setPhysEnvResizer(NULL, false, false);
            break;
        case 2:
            setBlockResizer(NULL, false, false);
            break;
        case 1:
            setEventSctSizeResizer(-1, false, false);
            break;
        case 0:
        default:
            setSectionResizer(false, false);
            MainWinConnect::pMainWin->on_cancelResize_clicked();
        }
    }
}


void LvlScene::setSectionResizer(bool enabled, bool accept)
{
    if((enabled)&&(pResizer==NULL))
    {
        MainWinConnect::pMainWin->on_actionSelect_triggered(); //Reset mode

        int x = LvlData->sections[LvlData->CurSection].size_left;
        int y = LvlData->sections[LvlData->CurSection].size_top;
        int w = LvlData->sections[LvlData->CurSection].size_right;
        int h = LvlData->sections[LvlData->CurSection].size_bottom;

        pResizer = new ItemResizer( QSize((long)fabs(x-w), (long)fabs(y-h)), Qt::green, 32 );
        this->addItem(pResizer);
        pResizer->setPos(x, y);
        pResizer->type=0;
        pResizer->_minSize = QSizeF(800, 600);
        this->setFocus(Qt::ActiveWindowFocusReason);
        //DrawMode=true;
        MainWinConnect::pMainWin->activeLvlEditWin()->changeCursor(leveledit::MODE_Resizing);
        MainWinConnect::pMainWin->resizeToolbarVisible(true);
    }
    else
    {
        if(pResizer!=NULL)
        {
            if(accept)
            {
                #ifdef _DEBUG_
                WriteToLog(QtDebugMsg, QString("SECTION RESIZE -> to %1 x %2").arg(pResizer->_width).arg(pResizer->_height));
                #endif
                long l = pResizer->pos().x();
                long t = pResizer->pos().y();
                long r = l+pResizer->_width;
                long b = t+pResizer->_height;
                long oldL = LvlData->sections[LvlData->CurSection].size_left;
                long oldR = LvlData->sections[LvlData->CurSection].size_right;
                long oldT = LvlData->sections[LvlData->CurSection].size_top;
                long oldB = LvlData->sections[LvlData->CurSection].size_bottom;
                LvlData->sections[LvlData->CurSection].size_left = l;
                LvlData->sections[LvlData->CurSection].size_right = r;
                LvlData->sections[LvlData->CurSection].size_top = t;
                LvlData->sections[LvlData->CurSection].size_bottom = b;

                addResizeSectionHistory(LvlData->CurSection, oldL, oldT, oldR, oldB, l, t, r, b);

                ChangeSectionBG(LvlData->sections[LvlData->CurSection].background);
                drawSpace();
                LvlData->modified = true;
            }
            delete pResizer;
            pResizer = NULL;
            MainWinConnect::pMainWin->on_actionSelect_triggered();
            MainWinConnect::pMainWin->resizeToolbarVisible(false);
            //resetResizingSection=true;
        }
        DrawMode=false;
    }
}

static long eventID=0;
void LvlScene::setEventSctSizeResizer(long event, bool enabled, bool accept)
{
    if(event>=0)
        eventID=event;

    if((enabled)&&(pResizer==NULL))
    {
        MainWinConnect::pMainWin->on_actionSelect_triggered(); //Reset mode

        int x = LvlData->events[eventID].sets[LvlData->CurSection].position_left;
        int y = LvlData->events[eventID].sets[LvlData->CurSection].position_top;
        int w = LvlData->events[eventID].sets[LvlData->CurSection].position_right;
        int h = LvlData->events[eventID].sets[LvlData->CurSection].position_bottom;

        pResizer = new ItemResizer( QSize((long)fabs(x-w), (long)fabs(y-h)), Qt::yellow, 32 );
        this->addItem(pResizer);
        pResizer->setPos(x, y);
        pResizer->type=1;
        pResizer->_minSize = QSizeF(800, 600);
        this->setFocus(Qt::ActiveWindowFocusReason);
        //DrawMode=true;
        MainWinConnect::pMainWin->activeLvlEditWin()->changeCursor(leveledit::MODE_Resizing);
        MainWinConnect::pMainWin->resizeToolbarVisible(true);
    }
    else
    {
        if(pResizer!=NULL)
        {
            if(accept)
            {
                #ifdef _DEBUG_
                WriteToLog(QtDebugMsg, QString("SECTION RESIZE -> to %1 x %2").arg(pResizer->_width).arg(pResizer->_height));
                #endif
                long l = pResizer->pos().x();
                long t = pResizer->pos().y();
                long r = l+pResizer->_width;
                long b = t+pResizer->_height;
                //long oldL = LvlData->events[eventID].sets[LvlData->CurSection].position_left;
                //long oldR = LvlData->events[eventID].sets[LvlData->CurSection].position_right;
                //long oldT = LvlData->events[eventID].sets[LvlData->CurSection].position_top;
                //long oldB = LvlData->events[eventID].sets[LvlData->CurSection].position_bottom;
                QList<QVariant> sizeData;
                sizeData.push_back((qlonglong)LvlData->CurSection);
                sizeData.push_back((qlonglong)LvlData->events[eventID].sets[LvlData->CurSection].position_top);
                sizeData.push_back((qlonglong)LvlData->events[eventID].sets[LvlData->CurSection].position_right);
                sizeData.push_back((qlonglong)LvlData->events[eventID].sets[LvlData->CurSection].position_bottom);
                sizeData.push_back((qlonglong)LvlData->events[eventID].sets[LvlData->CurSection].position_left);
                sizeData.push_back((qlonglong)t);
                sizeData.push_back((qlonglong)r);
                sizeData.push_back((qlonglong)b);
                sizeData.push_back((qlonglong)l);
                addChangeEventSettingsHistory(LvlData->events[eventID].array_id, LvlScene::SETTING_EV_SECSIZE, QVariant(sizeData));

                LvlData->events[eventID].sets[LvlData->CurSection].position_left = l;
                LvlData->events[eventID].sets[LvlData->CurSection].position_right = r;
                LvlData->events[eventID].sets[LvlData->CurSection].position_top = t;
                LvlData->events[eventID].sets[LvlData->CurSection].position_bottom = b;

                //addResizeSectionHistory(LvlData->CurSection, oldL, oldT, oldR, oldB, l, t, r, b);

                //ChangeSectionBG(LvlData->sections[LvlData->CurSection].background);
                //drawSpace();
                LvlData->modified = true;
                MainWinConnect::pMainWin->eventSectionSettingsSync();
            }
            delete pResizer;
            pResizer = NULL;
            MainWinConnect::pMainWin->on_actionSelect_triggered();
            MainWinConnect::pMainWin->resizeToolbarVisible(false);
            //resetResizingSection=true;
        }
        DrawMode=false;
    }
}

void LvlScene::setBlockResizer(QGraphicsItem * targetBlock, bool enabled, bool accept)
{
    if((enabled)&&(pResizer==NULL))
    {
        MainWinConnect::pMainWin->on_actionSelect_triggered(); //Reset mode

        int x = ((ItemBlock *)targetBlock)->blockData.x;
        int y = ((ItemBlock *)targetBlock)->blockData.y;
        int w = ((ItemBlock *)targetBlock)->blockData.w;
        int h = ((ItemBlock *)targetBlock)->blockData.h;

        pResizer = new ItemResizer( QSize(w, h), Qt::blue, 32 );
        this->addItem(pResizer);
        pResizer->setPos(x, y);
        pResizer->type=2;
        pResizer->targetItem = targetBlock;
        pResizer->_minSize = QSizeF(64, 64);
        this->setFocus(Qt::ActiveWindowFocusReason);
        //DrawMode=true;
        MainWinConnect::pMainWin->activeLvlEditWin()->changeCursor(leveledit::MODE_Resizing);
        MainWinConnect::pMainWin->resizeToolbarVisible(true);
    }
    else
    {
        if(pResizer!=NULL)
        {
            if(accept)
            {
                #ifdef _DEBUG_
                WriteToLog(QtDebugMsg, QString("BLOCK RESIZE -> to %1 x %2").arg(pResizer->_width).arg(pResizer->_height));
                #endif
                long x = pResizer->pos().x();
                long y = pResizer->pos().y();
                long w = pResizer->_width;
                long h = pResizer->_height;
                long oldX = ((ItemBlock *)pResizer->targetItem)->blockData.x;
                long oldY = ((ItemBlock *)pResizer->targetItem)->blockData.y;
                long oldW = ((ItemBlock *)pResizer->targetItem)->blockData.w;
                long oldH = ((ItemBlock *)pResizer->targetItem)->blockData.h;
                ((ItemBlock *)pResizer->targetItem)->blockData.x = x;
                ((ItemBlock *)pResizer->targetItem)->blockData.y = y;
                ((ItemBlock *)pResizer->targetItem)->blockData.w = w;
                ((ItemBlock *)pResizer->targetItem)->blockData.h = h;

                ((ItemBlock *)pResizer->targetItem)->setBlockSize( QRect(x,y,w,h) );
                LvlData->modified = true;

                addResizeBlockHistory(((ItemBlock *)pResizer->targetItem)->blockData, oldX, oldY, oldX+oldW, oldY+oldH, x, y, x+w, y+h);

                //ChangeSectionBG(LvlData->sections[LvlData->CurSection].background);
                //drawSpace();
            }
            delete pResizer;
            pResizer = NULL;
            MainWinConnect::pMainWin->on_actionSelect_triggered();
            MainWinConnect::pMainWin->resizeToolbarVisible(false);
            //resetResizingSection=true;
        }
        DrawMode=false;
    }
}

void LvlScene::setPhysEnvResizer(QGraphicsItem * targetRect, bool enabled, bool accept)
{
    if((enabled)&&(pResizer==NULL))
    {
        MainWinConnect::pMainWin->on_actionSelect_triggered(); //Reset mode

        int x = ((ItemWater *)targetRect)->waterData.x;
        int y = ((ItemWater *)targetRect)->waterData.y;
        int w = ((ItemWater *)targetRect)->waterData.w;
        int h = ((ItemWater *)targetRect)->waterData.h;

        pResizer = new ItemResizer( QSize(w, h), Qt::darkYellow, 16 );
        this->addItem(pResizer);
        pResizer->setPos(x, y);
        pResizer->type=3;
        pResizer->targetItem = targetRect;
        pResizer->_minSize = QSizeF(16, 16);
        this->setFocus(Qt::ActiveWindowFocusReason);
        //DrawMode=true;
        MainWinConnect::pMainWin->activeLvlEditWin()->changeCursor(leveledit::MODE_Resizing);
        MainWinConnect::pMainWin->resizeToolbarVisible(true);
    }
    else
    {
        if(pResizer!=NULL)
        {
            if(accept)
            {
                #ifdef _DEBUG_
                WriteToLog(QtDebugMsg, QString("Water RESIZE -> to %1 x %2").arg(pResizer->_width).arg(pResizer->_height));
                #endif
                long x = pResizer->pos().x();
                long y = pResizer->pos().y();
                long w = pResizer->_width;
                long h = pResizer->_height;
                long oldX = ((ItemWater *)pResizer->targetItem)->waterData.x;
                long oldY = ((ItemWater *)pResizer->targetItem)->waterData.y;
                long oldW = ((ItemWater *)pResizer->targetItem)->waterData.w;
                long oldH = ((ItemWater *)pResizer->targetItem)->waterData.h;
                ((ItemWater *)pResizer->targetItem)->waterData.x = x;
                ((ItemWater *)pResizer->targetItem)->waterData.y = y;
                ((ItemWater *)pResizer->targetItem)->waterData.w = w;
                ((ItemWater *)pResizer->targetItem)->waterData.h = h;

                ((ItemWater *)pResizer->targetItem)->setRectSize( QRect(x,y,w,h) );
                LvlData->modified = true;

                addResizeWaterHistory(((ItemWater *)pResizer->targetItem)->waterData, oldX, oldY, oldX+oldW, oldY+oldH, x, y, x+w, y+h);

                //ChangeSectionBG(LvlData->sections[LvlData->CurSection].background);
                //drawSpace();
            }
            delete pResizer;
            pResizer = NULL;
            MainWinConnect::pMainWin->on_actionSelect_triggered();
            MainWinConnect::pMainWin->resizeToolbarVisible(false);
            //resetResizingSection=true;
        }
        DrawMode=false;
    }
}

void LvlScene::setScreenshotSelector()
{
    isFullSection=true;
    emit screenshotSizeCaptured();
}

void LvlScene::setScreenshotSelector(bool enabled, bool accept)
{
    bool do_signal=false;
    if((enabled)&&(pResizer==NULL))
    {
        MainWinConnect::pMainWin->on_actionSelect_triggered(); //Reset mode

        pResizer = new ItemResizer( QSize(captutedSize.width(), captutedSize.height()), Qt::yellow, 2 );
        this->addItem(pResizer);
        pResizer->setPos(captutedSize.x(), captutedSize.y());
        pResizer->type=4;
        pResizer->_minSize = QSizeF(320, 200);
        this->setFocus(Qt::ActiveWindowFocusReason);
        //DrawMode=true;
        MainWinConnect::pMainWin->activeLvlEditWin()->changeCursor(WorldEdit::MODE_Resizing);
        MainWinConnect::pMainWin->resizeToolbarVisible(true);
    }
    else
    {
        if(pResizer!=NULL)
        {
            if(accept)
            {
                #ifdef _DEBUG_
                WriteToLog(QtDebugMsg, QString("SCREENSHOT SELECTION ZONE -> to %1 x %2").arg(pResizer->_width).arg(pResizer->_height));
                #endif

                captutedSize = QRectF( pResizer->pos().x(),
                                       pResizer->pos().y(),
                                       pResizer->_width,
                                       pResizer->_height);
                do_signal=true;
            }
            delete pResizer;
            pResizer = NULL;
            MainWinConnect::pMainWin->on_actionSelect_triggered();
            MainWinConnect::pMainWin->resizeToolbarVisible(false);
            //resetResizingSection=true;
        }
        DrawMode=false;
    }
    isFullSection=false;

    if(do_signal) emit screenshotSizeCaptured();
}
