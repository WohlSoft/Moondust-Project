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

#include "npcedit.h"
#include "./ui_npcedit.h"
#include "./file_formats/file_formats.h"

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
}



void npcedit::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}

