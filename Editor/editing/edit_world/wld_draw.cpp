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

#include <QtWidgets>
#include <QGraphicsItem>
#include <QPixmap>
#include <QGraphicsScene>
#include <QProgressDialog>

#include "world_edit.h"
#include <ui_world_edit.h>

#include <file_formats/wld_filedata.h>
//#include "../level_scene/lvl_scene.h"
#include <common_features/logger.h>


#include <QDebug>

bool WorldEdit::DrawObjects(QProgressDialog &progress)
{
    //int DataSize = progress.maximum();
    int TotalSteps = 5;

        if(!progress.wasCanceled())

            progress.setLabelText(tr("1/%1 Loading user data").arg(TotalSteps));

    qApp->processEvents();
    scene->loadUserData(progress);

        if(progress.wasCanceled()) return false;

        if(!progress.wasCanceled())
            progress.setLabelText(tr("1/%1 Applying Tiles").arg(TotalSteps));

    progress.setValue(progress.value()+1);
    qApp->processEvents();
    scene->setTiles(progress);

        if(progress.wasCanceled()) return false;

        if(!progress.wasCanceled())
            progress.setLabelText(tr("2/%1 Applying Sceneries...").arg(TotalSteps));

    progress.setValue(progress.value()+1);
    qApp->processEvents();
    scene->setSceneries(progress);

        if(progress.wasCanceled()) return false;

        if(!progress.wasCanceled())
            progress.setLabelText(tr("3/%1 Applying Paths...").arg(TotalSteps));

    progress.setValue(progress.value()+1);
    qApp->processEvents();
    scene->setPaths(progress);

        if(progress.wasCanceled()) return false;

        if(!progress.wasCanceled())
            progress.setLabelText(tr("4/%1 Applying Levels...").arg(TotalSteps));

    progress.setValue(progress.value()+1);
    progress.setValue(progress.value()+1);
    qApp->processEvents();
    scene->setLevels(progress);

        if(progress.wasCanceled()) return false;

        if(!progress.wasCanceled())
            progress.setLabelText(tr("5/%1 Applying Musics...").arg(TotalSteps));

    progress.setValue(progress.value()+1);
    qApp->processEvents();

    scene->setMusicBoxes(progress);

        if(progress.wasCanceled()) return false;

    if(scene->opts.animationEnabled)
        scene->startAnimation(); //Apply block animation

    if(!sceneCreated)
    {
        ui->graphicsView->setScene(scene);
        sceneCreated = true;
        connect(scene, SIGNAL(screenshotSizeCaptured()), this, SLOT(ExportingReady()));
    }
    if(!progress.wasCanceled())
        progress.setValue(progress.maximum());
    return true;
}

