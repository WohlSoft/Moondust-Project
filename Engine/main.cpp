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


#include <QApplication>
#include <QDesktopWidget>
#include <QElapsedTimer>
#include <QFileInfo>
#include <QDir>

#include <QtDebug>

#include "version.h"

#include "common_features/app_path.h"
#include "common_features/graphics_funcs.h"
#include "common_features/logger.h"

#include "data_configs/select_config.h"
#include "data_configs/config_manager.h"

#include "physics/phys_util.h"

#include "graphics/window.h"
#include "graphics/gl_renderer.h"
#undef main

#include <file_formats.h>
#include "fontman/font_manager.h"
#include "gui/pge_msgbox.h"
#include "networking/intproc.h"
#include "graphics/graphics.h"

#include "scenes/scene_level.h"
#include "scenes/scene_loading.h"
#include "scenes/scene_title.h"

#include <Box2D/Box2D.h>

#include <iostream>
using namespace std;


LevelScene* lScene;
enum Level_returnTo
{
    RETURN_TO_MAIN_MENU=0,
    RETURN_TO_WORLDMAP,
    RETURN_TO_GAMEOVER_SCREEN,
    RETURN_TO_CREDITS_SCREEN,
    RETURN_TO_EXIT
};
Level_returnTo end_level_jump=RETURN_TO_EXIT;

int main(int argc, char *argv[])
{

    QApplication::addLibraryPath( QFileInfo(argv[0]).dir().path() );

    QApplication a(argc, argv);


    ///Generating application path

    //Init system paths
    AppPathManager::initAppPath();

    foreach(QString arg, a.arguments())
    {
        if(arg=="--install")
        {
            AppPathManager::install();
            AppPathManager::initAppPath();

            QApplication::quit();
            QApplication::exit();

            return 0;
        }
    }

    //Init log writer
    LoadLogSettings();

    QString configPath="";
    QString fileToPpen = "";//ApplicationPath+"/physics.lvl";
    bool debugMode=false; //enable debug mode
    bool interprocessing=false; //enable interprocessing

    bool skipFirst=true;
    foreach(QString param, a.arguments())
    {
        if(skipFirst) {skipFirst=false; continue;}
        qDebug() << param;

        if(param.startsWith("--config="))
        {
            QStringList tmp;
            tmp = param.split('=');
            if(tmp.size()>1)
            {
                configPath = tmp.last();
                if(!SMBX64::qStr(configPath))
                {
                    configPath = FileFormats::removeQuotes(configPath);
                }
            }
        }
        else
        if(param == ("--debug"))
        {
            debugMode=true;
        }
        else
        if(param == ("--interprocessing"))
        {
            IntProc::init();
            interprocessing=true;
        }
        else
        {
            fileToPpen = param;
        }
    }



    ////Check & ask for configuration pack


    //Create empty config directory if not exists
    if(!QDir(ApplicationPath + "/" +  "configs").exists())
        QDir().mkdir(ApplicationPath + "/" +  "configs");

    // Config manager
    SelectConfig *cmanager = new SelectConfig();
    cmanager->setWindowFlags (Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    cmanager->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, cmanager->size(), a.desktop()->availableGeometry() ));

    QString configPath_manager = cmanager->isPreLoaded();

    //If application runned first time or target configuration is not exist
    if(configPath_manager.isEmpty() && configPath.isEmpty())
    {
        //Ask for configuration
        if(cmanager->exec()==QDialog::Accepted)
        {
            configPath = cmanager->currentConfig;
        }
        else
        {
            delete cmanager;
            IntProc::quit();
            exit(0);
        }
    }
    delete cmanager;



    //Load selected configuration pack



    ConfigManager::setConfigPath(configPath);
    if(!ConfigManager::loadBasics()) exit(1);

    //Init Window
    if(!PGE_Window::init(QString("Platformer Game Engine - v")+_FILE_VERSION+_FILE_RELEASE)) exit(1);

    //Init OpenGL (to work with textures, OpenGL should be load)
    if(!GlRenderer::init()) exit(1);

    //Init font manager
    FontManager::init();

    glFlush();
    SDL_GL_SwapWindow(PGE_Window::window);

    SDL_Event event; //  Events of SDL
    while ( SDL_PollEvent(&event) )
    {}


if(!fileToPpen.isEmpty())
{
    if(
       (fileToPpen.endsWith(".lvl", Qt::CaseInsensitive))
            ||
       (fileToPpen.endsWith(".lvlx", Qt::CaseInsensitive)))

    goto PlayLevel;
}

if(interprocessing) goto PlayLevel;

LoadingScreen:
{
    LoadingScene *ttl = new LoadingScene;
    ttl->setWaitTime(15000);

    ttl->init();
    ttl->setFade(25, 0.0f, 0.02f);
    int ret = ttl->exec();
    delete ttl;
    if(ret==-1) goto ExitFromApplication;

    goto MainMenu;
}

CreditsScreen:
{

    goto MainMenu;
}

GameOverScreen:
{

    goto MainMenu;
}

MainMenu:
{
    TitleScene * iScene = new TitleScene();
    iScene->setFade(25, 0.0f, 0.05f);
    int answer = iScene->exec();
    PlayLevelResult   res_level   = iScene->result_level;
    //PlayEpisodeResult res_episode = iScene->result_episode;
    delete iScene;

    switch(answer)
    {
        case TitleScene::ANSWER_EXIT:
            goto ExitFromApplication;
        case TitleScene::ANSWER_CREDITS:
            goto CreditsScreen;
        case TitleScene::ANSWER_LOADING:
            goto LoadingScreen;
        case TitleScene::ANSWER_GAMEOVER:
            goto GameOverScreen;
        case TitleScene::ANSWER_PLAYLEVEL:
            end_level_jump=RETURN_TO_MAIN_MENU;
            fileToPpen = res_level.levelfile;
            goto PlayLevel;
        default:
            goto PlayWorldMap;
    }

    goto PlayLevel;
}


PlayWorldMap:
{

    goto MainMenu;
}


PlayLevel:
{
    bool playAgain = true;
    int entranceID = 0;
    while(playAgain)
    {
        int ExitCode=0;
            lScene = new LevelScene();

            lScene->setLoaderAnimation(62);
            lScene->drawLoader();
            glFlush();
            SDL_GL_SwapWindow(PGE_Window::window);
            while ( SDL_PollEvent(&event) )
            {}

            bool sceneResult=true;

            if(fileToPpen.isEmpty())
            {
                if(interprocessing && IntProc::isEnabled())
                {
                    sceneResult = lScene->loadFileIP();
                    if((!sceneResult) && (!lScene->doExit))
                    {
                        SDL_Delay(50);
                        PGE_MsgBox msgBox(NULL, QString("ERROR:\nFail to start level\n\n%1")
                                          .arg(lScene->getLastError()),
                                          PGE_MsgBox::msg_error);
                        msgBox.exec();
                    }
                }
                else
                {
                    sceneResult = false;
                    PGE_MsgBox msgBox(NULL, QString("No opened files"),
                                      PGE_MsgBox::msg_warn);
                    msgBox.exec();
                }
            }
            else
            {
                sceneResult = lScene->loadFile(fileToPpen);
                if(!sceneResult)
                {
                    SDL_Delay(50);
                    PGE_MsgBox msgBox(NULL, QString("ERROR:\nFail to start level\n\n"
                                                    "%1")
                                      .arg(lScene->getLastError()),
                                      PGE_MsgBox::msg_error);
                    msgBox.exec();
                }
            }

            if(sceneResult)
                sceneResult = lScene->setEntrance(entranceID);

            if(sceneResult)
                sceneResult = lScene->loadConfigs();

            if(sceneResult)
                sceneResult = lScene->init();
            lScene->stopLoaderAnimation();

            lScene->setFade(25, 0.0f, 0.05f);

            if(sceneResult)
                ExitCode = lScene->exec();

            switch(ExitCode)
            {
            case LevelScene::EXIT_Warp:
                {
                    fileToPpen = lScene->toAnotherLevel();
                    entranceID = lScene->toAnotherEntrance();
                    if(fileToPpen.isEmpty()) playAgain = false;

                    if(debugMode)
                    {
                        if(!fileToPpen.isEmpty())
                        {
                            PGE_MsgBox msgBox(NULL, QString("Warp exit\n\nExit to:\n%1\n\nEnter to: %2")
                                          .arg(fileToPpen).arg(entranceID),
                                          PGE_MsgBox::msg_warn);
                            msgBox.exec();
                        }
                        playAgain = false;
                    }
                }
                break;
            case LevelScene::EXIT_Closed:
                {
                    end_level_jump=RETURN_TO_EXIT;
                    playAgain = false;
                }
                break;
            case LevelScene::EXIT_MenuExit:
                {
                    if(!debugMode)
                        end_level_jump=RETURN_TO_MAIN_MENU;
                    else
                        end_level_jump=RETURN_TO_EXIT;
                    playAgain = false;
                }
                break;
            case LevelScene::EXIT_PlayerDeath:
                {
                }
                break;
            default:
                playAgain = false;
            }

            ConfigManager::unloadLevelConfigs();
            delete lScene;

            if(interprocessing)
                goto ExitFromApplication;
    }

    switch(end_level_jump)
    {
        case RETURN_TO_WORLDMAP:
            goto PlayWorldMap;
        case RETURN_TO_MAIN_MENU:
            goto MainMenu;
        case RETURN_TO_EXIT:
            goto ExitFromApplication;
        case RETURN_TO_GAMEOVER_SCREEN:
            goto GameOverScreen;
        case RETURN_TO_CREDITS_SCREEN:
            goto CreditsScreen;
    }
}
ExitFromApplication:

    if(IntProc::isEnabled()) IntProc::editor->shut();
    IntProc::quit();

    FontManager::quit();

    PGE_Window::uninit();
    return 0;
}

