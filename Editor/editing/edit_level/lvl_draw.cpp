/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2024 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <QGraphicsItem>
#include <QPixmap>
#include <QGraphicsScene>
#include <QProgressDialog>
#include <QDebug>

#include <common_features/logger.h>
#include <editing/_scenes/level/lvl_scene.h>
#include <PGE_File_Formats/lvl_filedata.h>

#include "level_edit.h"
#include <ui_leveledit.h>

bool LevelEdit::DrawObjects(QProgressDialog &progress)
{
    int DataSize = progress.maximum();
    int TotalSteps = 6;

    if(!progress.wasCanceled())
        progress.setLabelText(tr("1/%1 Loading user data...").arg(TotalSteps));

    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    scene->loadUserData(progress);

    if(progress.wasCanceled()) return false;

    if(!progress.wasCanceled())
        progress.setLabelText(tr("1/%1 Loading Backgrounds...").arg(TotalSteps));

    progress.setValue(progress.value() + 1);
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    scene->makeSectionBG(progress);

    if(progress.wasCanceled()) return false;

    if(!progress.wasCanceled())
        progress.setLabelText(tr("2/%1 Loading BGOs...").arg(TotalSteps));

    progress.setValue(progress.value() + 1);
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    scene->setBGO(progress);

    if(progress.wasCanceled()) return false;

    if(!progress.wasCanceled())
        progress.setLabelText(tr("3/%1 Loading Blocks...").arg(TotalSteps));

    progress.setValue(progress.value() + 1);
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    scene->setBlocks(progress);

    if(progress.wasCanceled()) return false;

    if(!progress.wasCanceled())
        progress.setLabelText(tr("4/%1 Loading NPCs...").arg(TotalSteps));

    progress.setValue(progress.value() + 1);
    progress.setValue(progress.value() + 1);
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    scene->setNPC(progress);

    if(progress.wasCanceled()) return false;

    if(!progress.wasCanceled())
        progress.setLabelText(tr("5/%1 Loading PhysEZ...", "PhysEZ - Physical Environment Zone.").arg(TotalSteps));

    progress.setValue(progress.value() + 1);
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    scene->setWaters(progress);

    if(progress.wasCanceled()) return false;

    if(!progress.wasCanceled())
        progress.setLabelText(tr("6/%1 Loading Doors...").arg(TotalSteps));


    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    scene->setDoors(progress);

    if(progress.wasCanceled()) return false;

    scene->setPlayerPoints();

    scene->drawSpace();

    if(scene->m_opts.animationEnabled)
        scene->startAnimation();

    scene->applyLayersVisible();

    scene->Debugger_updateItemList();

    if(!sceneCreated)
    {
        ui->graphicsView->setScene(scene);
        sceneCreated = true;
        connect(scene, SIGNAL(screenshotSizeCaptured()), this, SLOT(ExportingReady()));
    }
    if(!progress.wasCanceled())
        progress.setValue(DataSize);
    return true;
}
