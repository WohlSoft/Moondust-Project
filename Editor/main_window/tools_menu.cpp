/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2021 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <QProcess>
#include <common_features/util.h>
#include <common_features/app_path.h>
#include <tools/tilesets/tileset_editor.h>
#include <tools/tilesets/tilesetgroupeditor.h>
#include <tools/external_tools/audiocvt_sox_gui.h>
#include <dev_console/devconsole.h>

#include <main_window/dock/lvl_sctc_props.h>

#include <ui_mainwindow.h>
#include <mainwindow.h>

#include "qfile_dialogs_default_options.hpp"


#ifdef _WIN32
#define PGE_MAINTAINER_EXE "pge_maintainer.exe"
#define PGE_MAINTAINER_BUNLDE PGE_MAINTAINER_EXE
#define PGE_MAINTAINER_BUNLDE_MASK PGE_MAINTAINER_EXE
#elif defined(__APPLE__)
#define PGE_MAINTAINER_BUNLDE "PGE Maintainer.app"
#define PGE_MAINTAINER_BUNLDE_MASK "PGE Maintainer"
#define PGE_MAINTAINER_EXECUTABLE "/Contents/MacOS/PGE Maintainer"
#define PGE_MAINTAINER_EXE PGE_MAINTAINER_BUNLDE PGE_MAINTAINER_EXECUTABLE
#else
#define PGE_MAINTAINER_EXE "pge_maintainer"
#define PGE_MAINTAINER_BUNLDE PGE_MAINTAINER_EXE
#define PGE_MAINTAINER_BUNLDE_MASK PGE_MAINTAINER_EXE
#endif

/*!
 * \brief Find the engine application in the available paths list
 * \param enginePath found engine application
 * \return true if engine application has been found, false if engine application wasn't found
 */
static bool findMaintainerApp(QString &enginePath)
{
    QStringList possiblePaths;
    possiblePaths.push_back(ApplicationPath + QStringLiteral("/") + PGE_MAINTAINER_EXE);

#ifdef Q_OS_MAC
    {
        //! Because of path randomizer thing since macOS Sierra, we are must detect it by absolute path
        QString app = QStandardPaths::locate(QStandardPaths::ApplicationsLocation, "PGE Project/" PGE_MAINTAINER_BUNLDE, QStandardPaths::LocateDirectory);
        if(!app.isEmpty())
            possiblePaths.push_back(app + QStringLiteral(PGE_MAINTAINER_EXECUTABLE));
    }
#endif

    //! Reserve path in case Engine is not available in the default path
    if(!GlobalSettings::extra.maintainerPath.isEmpty())
        possiblePaths.push_back(GlobalSettings::extra.maintainerPath);

    for(QString &f : possiblePaths)
    {
        if(QFile::exists(f))
        {
            enginePath = f;
            return true;
        }
    }

    return false;
}

static bool findMaintainer(MainWindow *parent, QString &command)
{
    while(!findMaintainerApp(command))
    {
        QMessageBox::warning(parent, MainWindow::tr("Moondust Maintainer is not found"),
                             MainWindow::tr("Can't start the utility, Moondust Maintainer is not found: \n%1\n"
                                            "Please, choose the Moondust Maintainer application yourself!").arg(PGE_MAINTAINER_BUNLDE),
                             QMessageBox::Ok);

        command = QFileDialog::getOpenFileName(parent,
                                               MainWindow::tr("Choose the Moondust Maintainer application"),
                                               GlobalSettings::extra.maintainerPath,
                                               QString("Moondust Maintainer executable (%1);;All files (*.*)")
                                               .arg(PGE_MAINTAINER_BUNLDE_MASK), nullptr, c_fileDialogOptions);

        if(command.isEmpty())
            return false;

        GlobalSettings::extra.maintainerPath = command;
    }
    return true;
}


void MainWindow::on_actionConfigure_Tilesets_triggered()
{
    TilesetEditor *tilesetConfDia = new TilesetEditor(&configs, NULL, this);
    util::DialogToCenter(tilesetConfDia);
    tilesetConfDia->exec();
    delete tilesetConfDia;

    configs.loadTilesets();
    setTileSetBox();
}


void MainWindow::on_actionTileset_groups_editor_triggered()
{
    TilesetGroupEditor *groupDialog;
    if(activeChildWindow() == WND_Level)
        groupDialog = new TilesetGroupEditor(activeLvlEditWin()->scene, this);
    else if(activeChildWindow() == WND_World)
        groupDialog = new TilesetGroupEditor(activeWldEditWin()->scene, this);
    else
        groupDialog = new TilesetGroupEditor(NULL, this);

    util::DialogToCenter(groupDialog);
    groupDialog->exec();
    delete groupDialog;

    configs.loadTilesets();
    setTileSetBox();
}


void MainWindow::on_actionShow_Development_Console_triggered()
{
    DevConsole::show();
}

//template<typename T>
//void defConstructObjAndExec(T *&dialogObj, QWidget *parent = 0)
//{
//    static_assert(std::is_base_of<QDialog, T>::value, "dialogObj must be base of QDialog!");
//    if(!dialogObj)
//    {
//        dialogObj = new T(parent);
//    }
//    util::DialogToCenter(dialogObj, true);
//    dialogObj->show();
//    dialogObj->raise();
//    dialogObj->setFocus();
//}

void MainWindow::on_actionLazyFixTool_triggered()
{
    QString command;
    if(!findMaintainer(this, command))
        return;
    QProcess::startDetached(command, {"lazyfix"});
}

void MainWindow::on_actionGIFs2PNG_triggered()
{
    QString command;
    if(!findMaintainer(this, command))
        return;
    QProcess::startDetached(command, {"gifs2png"});
}

void MainWindow::on_actionPNG2GIFs_triggered()
{
    QString command;
    if(!findMaintainer(this, command))
        return;
    QProcess::startDetached(command, {"png2gifs"});
}


void MainWindow::on_actionAudioCvt_triggered()
{
    AudioCvt_Sox_gui sox_cvt(this);
    util::DialogToCenter(&sox_cvt, true);
    sox_cvt.exec();

    if(activeChildWindow() == WND_Level)
        dock_LvlSectionProps->refreshFileData();
}




void MainWindow::on_actionSprite_editor_triggered()
{
    QMessageBox::information(this, "Dummy", "This feature comming soon!", QMessageBox::Ok);
}
