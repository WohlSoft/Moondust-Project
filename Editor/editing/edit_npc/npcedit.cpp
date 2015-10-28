/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2015 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <common_features/mainwinconnect.h>
#include <PGE_File_Formats/file_formats.h>

#include "npcedit.h"
#include <ui_npcedit.h>


NpcEdit::NpcEdit(dataconfigs * configs, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NpcEdit)
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

NpcEdit::~NpcEdit()
{
    delete ui;
    MainWinConnect::pMainWin->updateMenus(true);
}



void NpcEdit::closeEvent(QCloseEvent *event)
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

