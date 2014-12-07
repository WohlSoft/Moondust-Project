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


#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../script/command_compiler/autocodecompiler.h"
#include "../script/command_compiler/lunaluacompiler.h"
#include "../script/gui/additionalsettings.h"

void MainWindow::on_actionAdditional_Settings_triggered()
{
    if(activeChildWindow() == 1){
        if(!activeLvlEditWin()->LvlData.metaData.script)
            return;

        AdditionalSettings *addSetting = new AdditionalSettings(configs.config_dir + "lunadll_settings.ini", activeLvlEditWin()->LvlData.metaData.script);

        addSetting->exec();
        addSetting->cleanup();


        delete addSetting;
    }

}

void MainWindow::on_actionCompile_To_triggered()
{
    if(activeChildWindow() == 1){
        if(!activeLvlEditWin()->LvlData.metaData.script)
            return;

        LevelData *lvlData = &activeLvlEditWin()->LvlData;

        QDir pathOfFile = QDir(lvlData->path);
        if(!pathOfFile.exists(lvlData->filename)){
            pathOfFile.mkdir(lvlData->filename);
        }

        if(lvlData->metaData.script->usingCompilerType() == Script::COMPILER_LUNALUA){



            QFile lunaLuaFile(lvlData->path+"/"+lvlData->filename+"/"+"lunadll.lua");
            if(lunaLuaFile.exists()){
                if(QMessageBox::warning(this, tr("Already exsist"), tr("Lunadll.lua already exsist!\nOverwrite?"), QMessageBox::Yes|QMessageBox::No)==QMessageBox::No){
                    return;
                }
                lunaLuaFile.remove();
            }

            if(!lunaLuaFile.open(QFile::WriteOnly | QFile::Text)){
                QMessageBox::warning(this, tr("File save error"),
                                     tr("Cannot save file %1:\n%2.")
                                     .arg("lunadll.lua")
                                     .arg(lunaLuaFile.errorString()));
                return;
            }

            {
                QTextStream out(&lunaLuaFile);
                out << lvlData->metaData.script->compileCode();
            }
            lunaLuaFile.close();
            QMessageBox::information(this, tr("Successfully compiled!"), tr("Sucessfully compiled to %1/lunadll.lua").arg(lvlData->filename));
        }else if(lvlData->metaData.script->usingCompilerType() == Script::COMPILER_AUTOCODE){
            QFile lunaAutocodeFile(lvlData->path+"/"+lvlData->filename+"/"+"lunadll.txt");
            if(lunaAutocodeFile.exists()){
                if(QMessageBox::warning(this, tr("Already exsist"), tr("lunadll.txt already exsist!\nOverwrite?"), QMessageBox::Yes|QMessageBox::No)==QMessageBox::No){
                    return;
                }
                lunaAutocodeFile.remove();
            }

            if(!lunaAutocodeFile.open(QFile::WriteOnly | QFile::Text)){
                QMessageBox::warning(this, tr("File save error"),
                                     tr("Cannot save file %1:\n%2.")
                                     .arg("lunadll.lua")
                                     .arg(lunaAutocodeFile.errorString()));
                return;
            }

            {
                QTextStream out(&lunaAutocodeFile);
                out << lvlData->metaData.script->compileCode();
            }
            lunaAutocodeFile.close();
            QMessageBox::information(this, tr("Successfully compiled!"), tr("Sucessfully compiled to %1/lunadll.txt").arg(lvlData->filename));
        }
    }
}


void MainWindow::on_actionAutocode_Lunadll_Original_Language_triggered()
{
    if(activeChildWindow()==1){
        leveledit* lvledit = activeLvlEditWin();
        if(lvledit->LvlData.metaData.script){
            lvledit->LvlData.metaData.script->setUsingCompiler(new AutocodeCompiler(QList<EventCommand*>()));
            updateMenus(true);
        }
    }
}

void MainWindow::on_actionLunaLua_triggered()
{
    if(activeChildWindow()==1){
        leveledit* lvledit = activeLvlEditWin();
        if(lvledit->LvlData.metaData.script){
            lvledit->LvlData.metaData.script->setUsingCompiler(new LunaLuaCompiler(QList<EventCommand*>()));
            updateMenus(true);
        }
    }
}

