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

#include <QtMath>

#include "wld_scene.h"
#include "../edit_world/world_edit.h"

#include "../common_features/mainwinconnect.h"
#include "../common_features/grid.h"
#include "wld_item_placing.h"

#include "../file_formats/file_formats.h"


void WldScene::applyResizers()
{
    if(pResizer!=NULL )
    {
        switch(pResizer->type)
        {
        case 0:
            setScreenshotSelector(false, true);
            break;
        default:
            break;
        }
    }
}


void WldScene::resetResizers()
{
    if(pResizer!=NULL )
    {
        switch(pResizer->type)
        {
        case 0:
            setScreenshotSelector(false, false);
            break;
        default:
            break;
        }
    }
}



void WldScene::setScreenshotSelector(bool enabled, bool accept)
{
    bool do_signal=false;
    if((enabled)&&(pResizer==NULL))
    {
        MainWinConnect::pMainWin->on_actionSelect_triggered(); //Reset mode

        pResizer = new ItemResizer( QSize(captutedSize.width(), captutedSize.height()), Qt::yellow, 32 );
        this->addItem(pResizer);
        pResizer->setPos(captutedSize.x(), captutedSize.y());
        pResizer->type=0;
        pResizer->_minSize = QSizeF(320, 200);
        this->setFocus(Qt::ActiveWindowFocusReason);
        //DrawMode=true;
        MainWinConnect::pMainWin->activeWldEditWin()->changeCursor(WorldEdit::MODE_Resizing);
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

    if(do_signal) emit screenshotSizeCaptured();
}
