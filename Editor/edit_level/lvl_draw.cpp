/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#include <QtWidgets>
#include <QGraphicsItem>
#include <QPixmap>
#include <QGraphicsScene>
#include <QProgressDialog>

#include "leveledit.h"
#include "../ui_leveledit.h"

#include "../file_formats/lvl_filedata.h"
#include "../level_scene/lvlscene.h"
#include "../common_features/logger.h"


#include <QDebug>


bool leveledit::DrawObjects(QProgressDialog &progress)
{
    int DataSize = progress.maximum();
    int TotalSteps = 6;

        if(!progress.wasCanceled())
            progress.setLabelText(tr("1/%1 Loading user data").arg(TotalSteps));

    scene->loadUserData(progress);

        if(progress.wasCanceled()) return false;

        if(!progress.wasCanceled())
            progress.setLabelText(tr("1/%1 Applying Backgrounds").arg(TotalSteps));
    scene->makeSectionBG(progress);

        if(progress.wasCanceled()) return false;

        if(!progress.wasCanceled())
            progress.setLabelText(tr("2/%1 Applying BGOs...").arg(TotalSteps));
    scene->setBGO(progress);

        if(progress.wasCanceled()) return false;

        if(!progress.wasCanceled())
            progress.setLabelText(tr("3/%1 Applying Blocks...").arg(TotalSteps));
    scene->setBlocks(progress);

        if(progress.wasCanceled()) return false;

        if(!progress.wasCanceled())
            progress.setLabelText(tr("4/%1 Applying NPCs...").arg(TotalSteps));
    scene->setNPC(progress);

        if(progress.wasCanceled()) return false;

        if(!progress.wasCanceled())
            progress.setLabelText(tr("5/%1 Applying Waters...").arg(TotalSteps));
    scene->setWaters(progress);

        if(progress.wasCanceled()) return false;

        if(!progress.wasCanceled())
            progress.setLabelText(tr("6/%1 Applying Doors...").arg(TotalSteps));
    scene->setDoors(progress);

        if(progress.wasCanceled()) return false;

    scene->setPlayerPoints();

    scene->drawSpace();


    if(scene->opts.animationEnabled)
        scene->startBlockAnimation();//Apply block animation

    scene->applyLayersVisible();

    /*
    scene->setSceneRect(LvlData.sections[0].size_left-1000,
                        LvlData.sections[0].size_top-1000,
                        LvlData.sections[0].size_right+1000,
                        LvlData.sections[0].size_bottom+1000);
    */

    if(!sceneCreated)
    {
        ui->graphicsView->setScene(scene);
        sceneCreated = true;
    }

    if(!progress.wasCanceled())
        progress.setValue(DataSize);
    return true;
}
