/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <common_features/main_window_ptr.h>
#include <PGE_File_Formats/file_formats.h>

#include "npcedit.h"
#include <ui_npcedit.h>


NpcEdit::NpcEdit(MainWindow *mw,
                 dataconfigs *configs,
                 QWidget *parent) :
    EditBase(mw, parent),
    ui(new Ui::NpcEdit)
{
    m_configPack = configs;
    ui->setupUi(this);
}

NpcEdit::~NpcEdit()
{
    delete ui;
}

void NpcEdit::reTranslate()
{
    QString backup = windowTitle();
    ui->retranslateUi(this);
    setWindowTitle(backup);
}

void NpcEdit::closeEvent(QCloseEvent *event)
{
    if(maybeSave())
    {
        m_npcPreviewHitBox.reset();
        m_npcPreviewBody.reset();
        m_previewScene.reset();
        event->accept();
    }
    else
        event->ignore();
}
