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

#include "npcedit.h"
#include "./ui_npcedit.h"
#include "./file_formats/file_formats.h"

#include "../common_features/mainwinconnect.h"

npcedit::npcedit(dataconfigs * configs, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::npcedit)
{
    pConfigs = configs;
    PreviewScene=NULL;
    physics=NULL;
    npcPreview=NULL;
    npc_id = 0;
    FileType = 2;
    direction = -1;
    isUntitled = true;
    isModyfied  = false;
    ui->setupUi(this);
}

npcedit::~npcedit()
{
    delete ui;
    MainWinConnect::pMainWin->updateMenus(true);
}



void npcedit::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        if(physics) delete physics;
        if(npcPreview) delete npcPreview;
        if(PreviewScene) delete PreviewScene;
        event->accept();
    } else {
        event->ignore();
    }
}

