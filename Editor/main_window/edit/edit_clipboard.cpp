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

#include <QClipboard>

#include <ui_mainwindow.h>
#include <mainwindow.h>
#include <PGE_File_Formats/file_formats.h>

//Copy
void MainWindow::on_actionCopy_triggered()
{
    int q1=0, q2=0, q3=0, q4=0, q5=0;
    if (activeChildWindow() == WND_Level) //if active window is a levelEditor
    {
       LvlBuffer=activeLvlEditWin()->scene->copy();
       q1 += LvlBuffer.blocks.size();
       q2 += LvlBuffer.bgo.size();
       q3 += LvlBuffer.npc.size();
       q4 += LvlBuffer.physez.size();
       statusBar()->showMessage(tr("%1 blocks, %2 BGO, %3 NPC, %4 Water items have been copied to clipboard").arg(q1).arg(q2).arg(q3).arg(q4), 2000);
       //Save buffer into external clipboard in PGE-X format
       QString raw;
       FileFormats::WriteExtendedLvlFileRaw(LvlBuffer, raw);
       QApplication::clipboard()->setText(raw);
    }
    else
    if (activeChildWindow() == WND_World) //if active window is a worldEditor
    {
       WldBuffer=activeWldEditWin()->scene->copy();
       q1 += WldBuffer.tiles.size();
       q2 += WldBuffer.scenery.size();
       q3 += WldBuffer.paths.size();
       q4 += WldBuffer.levels.size();
       q5 += WldBuffer.music.size();
       statusBar()->showMessage(tr("%1 tiles, %2 sceneries, %3 paths, %4 levels, %5 music boxes items have been copied to clipboard").arg(q1).arg(q2).arg(q3).arg(q4).arg(q5), 2000);
       //Save buffer into external clipboard in PGE-X format
       QString raw;
       FileFormats::WriteExtendedWldFileRaw(WldBuffer, raw);
       QApplication::clipboard()->setText(raw);
    }
}


//Cut
void MainWindow::on_actionCut_triggered()
{
    int q1=0, q2=0, q3=0, q4=0, q5=0;
    if (activeChildWindow() == WND_Level) //if active window is a levelEditor
    {
       LvlBuffer=activeLvlEditWin()->scene->copy(true);
       q1 += LvlBuffer.blocks.size();
       q2 += LvlBuffer.bgo.size();
       q3 += LvlBuffer.npc.size();
       q4 += LvlBuffer.physez.size();
       statusBar()->showMessage(tr("%1 blocks, %2 BGO, %3 NPC, %4 Water items have been moved to clipboard").arg(q1).arg(q2).arg(q3).arg(q4), 2000);
       QString raw;
       FileFormats::WriteExtendedLvlFileRaw(LvlBuffer, raw);
       QApplication::clipboard()->setText(raw);
    }
    else
    if (activeChildWindow() == WND_World) //if active window is a worldEditor
    {
       WldBuffer=activeWldEditWin()->scene->copy(true);
       q1 += WldBuffer.tiles.size();
       q2 += WldBuffer.scenery.size();
       q3 += WldBuffer.paths.size();
       q4 += WldBuffer.levels.size();
       q5 += WldBuffer.music.size();
       statusBar()->showMessage(tr("%1 tiles, %2 sceneries, %3 paths, %4 levels, %5 music boxes items have been moved to clipboard").arg(q1).arg(q2).arg(q3).arg(q4).arg(q5), 2000);
       QString raw;
       FileFormats::WriteExtendedWldFileRaw(WldBuffer, raw);
       QApplication::clipboard()->setText(raw);
    }
}

//Paste
void MainWindow::on_actionPaste_triggered()
{
    if (activeChildWindow() == WND_Level)
    {
        LevelData tmp;
        QString raw=QApplication::clipboard()->text();
        if( FileFormats::ReadExtendedLvlFileRaw(raw, "<clipboard>", tmp) )
        {
            LvlBuffer = tmp;
        }

        if
        (   //if buffer is empty
                (LvlBuffer.blocks.size()==0)&&
                (LvlBuffer.bgo.size()==0)&&
                (LvlBuffer.npc.size()==0)&&
                (LvlBuffer.physez.size()==0)
        ) return;
    }
    else
    if (activeChildWindow() == WND_World)
    {
        WorldData tmp;
        QString raw = QApplication::clipboard()->text();
        if( FileFormats::ReadExtendedWldFileRaw(raw, "<clipboard>", tmp) )
        {
            WldBuffer = tmp;
        }

        if
        (   //if buffer is empty
                (WldBuffer.tiles.size()==0)&&
                (WldBuffer.scenery.size()==0)&&
                (WldBuffer.paths.size()==0)&&
                (WldBuffer.levels.size()==0)&&
                (WldBuffer.music.size()==0)
        ) return;
    }

    resetEditmodeButtons();
    ui->actionSelect->setChecked(1);

    if (activeChildWindow() == WND_Level)
    {
       activeLvlEditWin()->scene->SwitchEditingMode(LvlScene::MODE_PasteFromClip);
       activeLvlEditWin()->scene->m_dataBuffer = LvlBuffer;
    }
    else
    if (activeChildWindow() == WND_World)
    {
       activeWldEditWin()->scene->SwitchEditingMode(LvlScene::MODE_PasteFromClip);
       activeWldEditWin()->scene->WldBuffer = WldBuffer;
    }

}

