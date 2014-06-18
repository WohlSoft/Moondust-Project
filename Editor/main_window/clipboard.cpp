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

#include "../ui_mainwindow.h"
#include "../mainwindow.h"


//Copy
void MainWindow::on_actionCopy_triggered()
{
    int q1=0, q2=0, q3=0, q4=0;
    if (activeChildWindow()==1) //if active window is a levelEditor
    {
       LvlBuffer=activeLvlEditWin()->scene->copy();
       q1 += LvlBuffer.blocks.size();
       q2 += LvlBuffer.bgo.size();
       q3 += LvlBuffer.npc.size();
       q4 += LvlBuffer.water.size();
       statusBar()->showMessage(tr("%1 blocks, %2 BGO, %3 NPC, %4 Water items have been copied to clipboard").arg(q1).arg(q2).arg(q3).arg(q4), 2000);
    }

}


//Cut
void MainWindow::on_actionCut_triggered()
{
    int q1=0, q2=0, q3=0, q4=0;
    if (activeChildWindow()==1) //if active window is a levelEditor
    {
       LvlBuffer=activeLvlEditWin()->scene->copy(true);
       q1 += LvlBuffer.blocks.size();
       q2 += LvlBuffer.bgo.size();
       q3 += LvlBuffer.npc.size();
       q4 += LvlBuffer.water.size();
       //statusBar()->showMessage(tr("%1 blocks, %2 BGO, %3 NPC items have been moved to clipboard").arg(q1).arg(q2).arg(q3), 2000);
       statusBar()->showMessage(tr("%1 blocks, %2 BGO, %3 NPC, %4 Water items have been moved to clipboard").arg(q1).arg(q2).arg(q3).arg(q4), 2000);
    }
}

//Paste
void MainWindow::on_actionPaste_triggered()
{
    if
    (   //if buffer is empty
            (LvlBuffer.blocks.size()==0)&&
            (LvlBuffer.bgo.size()==0)&&
            (LvlBuffer.npc.size()==0)&&
            (LvlBuffer.water.size()==0)
    ) return;

    resetEditmodeButtons();
    ui->actionSelect->setChecked(1);

    if (activeChildWindow()==1)
    {
       activeLvlEditWin()->changeCursor(4);
       activeLvlEditWin()->scene->EditingMode = 4;
       activeLvlEditWin()->scene->EraserEnabled = false;
       activeLvlEditWin()->scene->disableMoveItems=true;

       activeLvlEditWin()->scene->LvlBuffer = LvlBuffer;
    }

}
