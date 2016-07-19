/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2016 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <common_features/graphics_funcs.h>

#include <ui_mainwindow.h>
#include <mainwindow.h>


// //////////////////////////////////////////////////////////////
void MainWindow::on_actionGridEn_triggered(bool checked)
{
    if (activeChildWindow()==1)
    {
       activeLvlEditWin()->scene->m_opts.grid_snap = checked;
    }
    else
    if (activeChildWindow()==3)
    {
       activeWldEditWin()->scene->opts.grid_snap = checked;
    }
}


void MainWindow::on_actionShowGrid_triggered(bool checked)
{
    GlobalSettings::LvlOpts.grid_show = checked;
    if (activeChildWindow()==1)
    {
       activeLvlEditWin()->scene->m_opts.grid_show = checked;
       activeLvlEditWin()->scene->update();
    }
    else
    if (activeChildWindow()==3)
    {
       activeWldEditWin()->scene->opts.grid_show = checked;
       activeWldEditWin()->scene->update();
    }
}

// //////////////////////////////////////////////////////////////
void MainWindow::on_actionAnimation_triggered(bool checked)
{
    GlobalSettings::LvlOpts.animationEnabled = checked;
    if (activeChildWindow()==1)
    {
        LevelEdit *e=activeLvlEditWin(); if(!e) return;
        LvlScene  *s=e->scene; if(!s) return;
        if(GlobalSettings::LvlOpts.animationEnabled)
            s->startAnimation();
        else
            s->stopAnimation();
        s->m_opts.animationEnabled = GlobalSettings::LvlOpts.animationEnabled;
    }
    else
    if (activeChildWindow()==3)
    {
        activeWldEditWin()->scene->opts.animationEnabled = GlobalSettings::LvlOpts.animationEnabled;
        if(GlobalSettings::LvlOpts.animationEnabled)
        {
            activeWldEditWin()->scene->startAnimation();
        }
        else
            activeWldEditWin()->scene->stopAnimation();
    }
}


void MainWindow::on_actionCollisions_triggered(bool checked)
{
    GlobalSettings::LvlOpts.collisionsEnabled = checked;
    if (activeChildWindow()==1)
    {
        activeLvlEditWin()->scene->m_opts.collisionsEnabled = GlobalSettings::LvlOpts.collisionsEnabled;
    }
    else
    if (activeChildWindow()==3)
    {
        activeWldEditWin()->scene->opts.collisionsEnabled = GlobalSettings::LvlOpts.collisionsEnabled;
    }

}

// //////////////////////////////////////////////////////////////
void MainWindow::on_actionSwitch_to_Fullscreen_triggered(bool checked)
{
    if(checked){
        //this->hide();
        this->showFullScreen();
    }else{
        //this->hide();
        this->showNormal();
    }
}


void MainWindow::on_actionSemi_transparent_paths_triggered(bool checked)
{
    GlobalSettings::LvlOpts.semiTransparentPaths = checked;
    if(activeChildWindow()==3)
    {
        activeWldEditWin()->scene->opts.semiTransparentPaths = GlobalSettings::LvlOpts.semiTransparentPaths;
        activeWldEditWin()->scene->setSemiTransparentPaths(GlobalSettings::LvlOpts.semiTransparentPaths);
    }

}
