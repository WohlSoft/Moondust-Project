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

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QDesktopServices>
#include <QFileInfo>
#include <QUrl>
#include <QDir>

static void showUnsavedFileNotify(MainWindow *p)
{
    QMessageBox::warning(p,
                         MainWindow::tr("File is not saved"),
                         MainWindow::tr("Impossible to open/create script file.\n"
                                        "Please save file firtst"));

}

static void openLuaFile(QString path, QString fileName)
{
    QString fullPath = path+"/"+fileName;
    if(!QFile::exists(path+"/"+fileName))
    {
        QDir dir;
        dir.mkpath(path);
        QFile f(fullPath);
        f.open(QIODevice::WriteOnly|QIODevice::Truncate);
    }
    QDesktopServices::openUrl(QUrl::fromLocalFile(fullPath));
}

void MainWindow::on_actionCreateScriptLocal_triggered()
{
    int wnd = activeChildWindow();
    if(wnd == WND_Level)
    {
        LevelEdit* lvl = activeLvlEditWin();
        if(!lvl)
            return;
        if(lvl->isUntitled)
        {
            showUnsavedFileNotify(this);
            return;
        }
        openLuaFile(lvl->LvlData.meta.path+"/"+lvl->LvlData.meta.filename, configs.localScriptName_lvl);
    }
    else
    if(wnd == WND_World)
    {
        WorldEdit* wld = activeWldEditWin();
        if(!wld)
            return;
        if(wld->isUntitled)
        {
            showUnsavedFileNotify(this);
            return;
        }
        openLuaFile(wld->WldData.meta.path+"/"+wld->WldData.meta.filename, configs.localScriptName_wld);
    }
}

void MainWindow::on_actionCreateScriptEpisode_triggered()
{
    int wnd = activeChildWindow();
    if(wnd == WND_Level)
    {
        LevelEdit* lvl = activeLvlEditWin();
        if(!lvl)
            return;
        if(lvl->isUntitled)
        {
            showUnsavedFileNotify(this);
            return;
        }
        openLuaFile(lvl->LvlData.meta.path, configs.commonScriptName_lvl);
    }
    else
    if(wnd == WND_World)
    {
        WorldEdit* wld = activeWldEditWin();
        if(!wld)
            return;
        if(wld->isUntitled)
        {
            showUnsavedFileNotify(this);
            return;
        }
        openLuaFile(wld->WldData.meta.path, configs.commonScriptName_wld);
    }
}

void MainWindow::on_actionLunaLUA_lvl_triggered()
{
    int wnd = activeChildWindow();
    if(wnd == WND_Level)
    {
        LevelEdit* lvl = activeLvlEditWin();
        if(!lvl)
            return;
        if(lvl->isUntitled)
        {
            showUnsavedFileNotify(this);
            return;
        }
        openLuaFile(lvl->LvlData.meta.path+"/"+lvl->LvlData.meta.filename, "lunadll.lua");
    }
    else
    if(wnd == WND_World)
    {
        WorldEdit* wld = activeWldEditWin();
        if(!wld)
            return;
        if(wld->isUntitled)
        {
            showUnsavedFileNotify(this);
            return;
        }
        openLuaFile(wld->WldData.meta.path+"/"+wld->WldData.meta.filename, "lunadll.lua");
    }
}

void MainWindow::on_actionLunaLUA_eps_triggered()
{
    int wnd = activeChildWindow();
    if(wnd == WND_Level)
    {
        LevelEdit* lvl = activeLvlEditWin();
        if(!lvl)
            return;
        if(lvl->isUntitled)
        {
            showUnsavedFileNotify(this);
            return;
        }
        openLuaFile(lvl->LvlData.meta.path, "lunaworld.lua");
    }
    else
    if(wnd == WND_World)
    {
        WorldEdit* wld = activeWldEditWin();
        if(!wld)
            return;
        if(wld->isUntitled)
        {
            showUnsavedFileNotify(this);
            return;
        }
        openLuaFile(wld->WldData.meta.path, "lunaworld.lua");
    }
}

void MainWindow::on_actionLunaLUA_wld_triggered()
{
    int wnd = activeChildWindow();
    if(wnd == WND_Level)
    {
        LevelEdit* lvl = activeLvlEditWin();
        if(!lvl)
            return;
        if(lvl->isUntitled)
        {
            showUnsavedFileNotify(this);
            return;
        }
        openLuaFile(lvl->LvlData.meta.path, "lnuaoverworld.lua");
    }
    else
    if(wnd == WND_World)
    {
        WorldEdit* wld = activeWldEditWin();
        if(!wld)
            return;
        if(wld->isUntitled)
        {
            showUnsavedFileNotify(this);
            return;
        }
        openLuaFile(wld->WldData.meta.path, "lnuaoverworld.lua");
    }
}
