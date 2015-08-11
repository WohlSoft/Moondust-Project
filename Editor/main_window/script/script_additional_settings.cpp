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

#include <script/gui/additionalsettings.h>

#include "mainwindow.h"
#include "ui_mainwindow.h"

void MainWindow::on_actionAdditional_Settings_triggered()
{
    if(activeChildWindow() == 1){
        if(!activeLvlEditWin()->LvlData.metaData.script)
            return;

        AdditionalSettings *addSetting = new AdditionalSettings(configs.config_dir + "lunadll_settings.ini", activeLvlEditWin()->LvlData.metaData.script, this);

        addSetting->exec();
        addSetting->cleanup();


        delete addSetting;
    }

}


