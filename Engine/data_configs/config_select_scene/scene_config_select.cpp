/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2016 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "scene_config_select.h"

#include <graphics/gl_renderer.h>
#include <graphics/graphics.h>
#include <graphics/window.h>
#include <gui/pge_questionbox.h>
#include <data_configs/config_manager.h>
#include <gui/pge_menubox.h>
#include <fontman/font_manager.h>
#include <settings/global_settings.h>

#include <QDir>
#include <QFileInfo>

#include <QDesktopServices>

#include <common_features/app_path.h>

#include <common_features/util.h>
#include <common_features/graphics_funcs.h>


ConfigSelectScene::ConfigSelectScene():
    Scene(ConfigSelect)
{
    ret = 0;
    bgcolor.r = 0.0f;
    bgcolor.g = 0.0f;
    bgcolor.b = 0.1f;
    //% "Choose a game:"
    m_label = qtTrId("CONFIG_SELECT");
    m_waterMark = "WohlSoft team 2016 by Wohlstand (http://wohlsoft.ru)";
    m_waterMarkRect.setPos(200, PGE_Window::Height - 50);
    m_waterMarkFontSize = 20;
    PGE_Size s = FontManager::textSize(m_waterMark, -2, 10000, false, m_waterMarkFontSize);
    m_waterMarkRect.setSize(s.w(), s.h());
    m_waterMarkColor.setRgba(0.5, 0.5, 1.0, 1.0);
    mousePos.setPoint(-1000, -1000);
    GlRenderer::loadTextureP(cursor, ":cursor.png");
    controller = g_AppSettings.openController(1);
    currentConfig   = "";
    themePack       = "";
    typedef QPair<QString, QString > configPackPair;
    QList<configPackPair > config_paths;
    QString configPath(ApplicationPath + "/configs/");
    QDir    configDir(configPath);
    QStringList configs = configDir.entryList(QDir::AllDirs);

    for(QString &c : configs)
    {
        QString config_dir = configPath + c + "/";
        configPackPair path;
        path.first = c;//Full path
        path.second = config_dir;//name of config dir
        config_paths << path;
    }

    if(AppPathManager::userDirIsAvailable())
    {
        QString configPath_user = AppPathManager::userAppDir() + "/configs/"; //!< User additional folder
        QDir configUserDir(configPath_user);
        configs = configUserDir.entryList(QDir::AllDirs);

        for(QString &c : configs)
        {
            QString config_dir = configPath_user + c + "/";
            configPackPair path;
            path.first  = c;//Full path
            path.second = config_dir;//name of config dir
            config_paths << path;
        }
    }

    for(configPackPair &confD : config_paths)
    {
        QString c = confD.first;
        QString config_dir = confD.second;
        QString configName;
        QString data_dir;
        QString splash_logo;
        QString description;
        QString gui_ini = config_dir + "main.ini";

        if(!QFileInfo(gui_ini).exists()) continue; //Skip if it is not a config

        QSettings guiset(gui_ini, QSettings::IniFormat);
        guiset.setIniCodec("UTF-8");
        guiset.beginGroup("gui");
        splash_logo = guiset.value("editor-splash", "").toString();
        splash_logo = guiset.value("engine-icon", /* show splash if alternate icon is not defined */ splash_logo).toString();
        themePack = guiset.value("default-theme", "").toString();
        guiset.endGroup();
        guiset.beginGroup("main");
        data_dir = (guiset.value("application-dir", "0").toBool() ?
                    ApplicationPath + "/" : config_dir + "data/");
        configName = guiset.value("config_name", QDir(config_dir).dirName()).toString();
        description = guiset.value("config_desc", config_dir).toString();
        guiset.endGroup();

        //Default splash image
        if(splash_logo.isEmpty())
            splash_logo = ":cat_256.png";
        else
        {
            splash_logo = data_dir + splash_logo;

            if(QPixmap(splash_logo).isNull())
                splash_logo = ":cat_256.png";
        }

        ConfigPackEntry item;
        item.fullname = configName;
        item.key  = c;
        item.path = config_dir;
        FontManager::optimizeText(description, 28);
        item.description = description;
        GlRenderer::loadTextureP(item.image, splash_logo);
        //item = new QListWidgetItem( configName );
        //QPixmap iconImg(200,114);
        //iconImg.fill(QColor(Qt::white));
        //iconImg = squareImage(QPixmap(splash_logo), QSize(200,114));
        //item->setIcon( QIcon( iconImg ) );
        //item->setData(Qt::ToolTipRole, description);
        //item->setData(Qt::UserRole, c);
        //item->setData(Qt::UserRole+4, config_dir);
        //item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );
        //ui->configList->addItem( item );
        m_availablePacks.push_back(item);
    }

    //m_availablePacks
}

ConfigSelectScene::~ConfigSelectScene()
{
    GlRenderer::clearScreen();
    GlRenderer::deleteTexture(cursor);

    for(ConfigPackEntry &item : m_availablePacks)
        GlRenderer::deleteTexture(item.image);

    if(controller)
        delete controller;
}

QString ConfigSelectScene::isPreLoaded(QString openConfig)
{
    QString configPath = openConfig;

    if(!configPath.isEmpty())
    {
        if(QFileInfo(openConfig + "/main.ini").exists())
            currentConfig = configPath;
    }
    else
    {
        if(m_availablePacks.size() == 1)
        {
            ConfigPackEntry &item = m_availablePacks[0];
            currentConfig       = item.key;
            currentConfigPath   = item.path;
        }
    }

    return currentConfig;
}

void ConfigSelectScene::onKeyboardPressedSDL(SDL_Keycode sdl_key, Uint16)
{
    if(m_doExit) return;

    if(controller->keys.up)
        menu.selectUp();
    else if(controller->keys.down)
        menu.selectDown();
    else if(controller->keys.left)
        menu.selectLeft();
    else if(controller->keys.right)
        menu.selectRight();
    else if(controller->keys.jump)
        menu.acceptItem();
    else if(controller->keys.alt_jump)
        menu.acceptItem();
    else if(controller->keys.run)
        menu.rejectItem();
    else
        switch(sdl_key)
        {
        case SDLK_UP:
            menu.selectUp();
            break;

        case SDLK_DOWN:
            menu.selectDown();
            break;

        case SDLK_LEFT:
            menu.selectLeft();
            break;

        case SDLK_RIGHT:
            menu.selectRight();
            break;

        case SDLK_RETURN:
            menu.acceptItem();
            break;

        case SDLK_ESCAPE:
            menu.rejectItem();
            break;

        default:
            break;
        }
}

void ConfigSelectScene::onMouseMoved(SDL_MouseMotionEvent &mmevent)
{
    mousePos = GlRenderer::MapToScr(mmevent.x, mmevent.y);

    if(!m_doExit)
        menu.setMouseHoverPos(mousePos.x(), mousePos.y());

    if(m_waterMarkRect.collidePoint(mousePos.x(), mousePos.y()))
        m_waterMarkColor.setRgba(1.0, 0.0, 1.0, 1.0);
    else
        m_waterMarkColor.setRgba(0.5, 0.5, 1.0, 1.0);
}

void ConfigSelectScene::onMousePressed(SDL_MouseButtonEvent &mbevent)
{
    if(m_doExit) return;

    switch(mbevent.button)
    {
    case SDL_BUTTON_LEFT:
        mousePos = GlRenderer::MapToScr(mbevent.x, mbevent.y);
        menu.setMouseClickPos(mousePos.x(), mousePos.y());

        if(m_waterMarkRect.collidePoint(mousePos.x(), mousePos.y()))
            QDesktopServices::openUrl(QUrl("http://wohlsoft.ru/PGE"));

        break;

    case SDL_BUTTON_RIGHT:
        menu.rejectItem();
        break;

    default:
        break;
    }
}

void ConfigSelectScene::onMouseWheel(SDL_MouseWheelEvent &wheelevent)
{
    if(!m_doExit)
    {
        if(wheelevent.y > 0)
            menu.scrollUp();
        else
            menu.scrollDown();
    }
}

void ConfigSelectScene::update()
{
    if(m_doExit)
    {
        if(m_fader.isFull())
        {
            m_isRunning = false;
            return;
        }
    }

    Scene::update();
}

void ConfigSelectScene::render()
{
    GlRenderer::clearScreen();
    static double hue = 0.0;
    static double brightness = 0.0;
    static double brightnessDelta = +0.003;
    GlColor c;
    c.setHsva(hue, 1.0, 1.0, 1.0);
    GlRenderer::renderRect(0, 0, PGE_Window::Width, PGE_Window::Height, float(brightness), float(brightness), float(brightness)/*bgcolor.b*/, 1.0);
    int padding = 20;
    GlRenderer::renderRect(padding, padding,
                           PGE_Window::Width - padding * 2,
                           PGE_Window::Height - padding * 2,
                           0.0f, 0.0f, 0.2f, 1.0f);
    padding = 15;
    GlRenderer::renderRect(padding, padding,
                           PGE_Window::Width - padding * 2,
                           PGE_Window::Height - padding * 2,
                           c.Red(), c.Green(), c.Blue(), c.Alpha(), false);
    padding = 13;
    GlRenderer::renderRect(padding, padding,
                           PGE_Window::Width - padding * 2,
                           PGE_Window::Height - padding * 2,
                           c.Red(), c.Green(), c.Blue(), c.Alpha(), false);
    padding = 12;
    GlRenderer::renderRect(padding, padding,
                           PGE_Window::Width - padding * 2,
                           PGE_Window::Height - padding * 2,
                           c.Red(), c.Green(), c.Blue(), c.Alpha(), false);
    hue += 2.0;

    while(hue >= 360.0)
        hue -= 360.0;

    brightness += brightnessDelta;

    if((brightness >= 0.2) || (brightness <= 0.0))
        brightnessDelta *= -1.0;

    {
        int key = menu.currentItem().item_key.toInt();
        ConfigPackEntry &item = m_availablePacks[key];
        currentConfig       = item.key;
        currentConfigPath   = item.path;
        float w = float(item.image.w) * (150.0f / float(item.image.h));
        float h = 150;

        if(w > 200)
        {
            h = h * (200 / w);
            w = 200;
        }

        GlRenderer::renderTexture(&item.image, 30, 30, w, h);
        FontManager::printText(item.description, 30, 170, -2, 1.0, 1.0, 1.0, 1.0, 18);
    }
    FontManager::printText(m_label, 240, 80, -2, 1.0, 1.0, 1.0, 1.0, 48);
    FontManager::printText(m_waterMark,
                           m_waterMarkRect.x(), m_waterMarkRect.y(), -2,
                           m_waterMarkColor.Red(),
                           m_waterMarkColor.Green(),
                           m_waterMarkColor.Blue(),
                           m_waterMarkColor.Alpha(), m_waterMarkFontSize);

    if(PGE_Window::showPhysicsDebug)
    {
        GlRenderer::renderRect(float(m_waterMarkRect.x()),
                               float(m_waterMarkRect.y()),
                               float(m_waterMarkRect.width()),
                               float(m_waterMarkRect.height()),
                               1.0f, 1.0f, 0.2f, 1.0f, false);
    }

    menu.render();

    if(PGE_Window::showDebugInfo)
    {
        FontManager::printText(QString("Graphical engine: %1").arg(GlRenderer::engineName()),
                               500, 30, -2, 1.0, 1.0, 1.0, 0.5, 18);
    }

    Scene::render();
}

void ConfigSelectScene::renderMouse()
{
    int posX = mousePos.x();
    int posY = mousePos.y();
    GlRenderer::setTextureColor(1.0f, 1.0f, 1.0f, 1.0f);
    GlRenderer::renderTexture(&cursor, posX, posY);
}

int ConfigSelectScene::exec()
{
    m_doExit = false;
    m_isRunning = true;
    m_fader.setRatio(0.0);
    GlRenderer::setClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black background color
    menu.clear();
    menu.setTextLenLimit(22);
    menu.setPos(280, 140);
    menu.setItemsNumber(10);

    for(int i = 0; i < m_availablePacks.size(); i++)
        menu.addMenuItem(QString::number(i), m_availablePacks[i].fullname);

    //continueOrQuit.exec();
    LoopTiming times;
    times.start_common = SDL_GetTicks();
    bool frameSkip = g_AppSettings.frameSkip;
    //Hide mouse cursor
    PGE_Window::setCursorVisibly(false);

    while(m_isRunning)
    {
        times.start_common = SDL_GetTicks();
        processEvents();
        processMenu();
        update();
        times.stop_render  = 0;
        times.start_render = 0;

        /**********************Process rendering of stuff****************************/
        if((PGE_Window::vsync) || (times.doUpdate_render <= 0.0))
        {
            times.start_render = SDL_GetTicks();
            /**********************Render everything***********************/
            render();

            if(!m_doExit) renderMouse();

            GlRenderer::flush();
            GlRenderer::repaint();
            times.stop_render = SDL_GetTicks();
            times.doUpdate_render = frameSkip ? uTickf + (times.stop_render - times.start_render) : 0;
        }

        times.doUpdate_render -= uTickf;

        if(times.stop_render < times.start_render)
        {
            times.stop_render = 0;
            times.start_render = 0;
        }

        /****************************************************************************/
        if((!PGE_Window::vsync) && (uTick > times.passedCommonTime()))
            SDL_Delay(uTick - times.passedCommonTime());
    }

    PGE_Window::setCursorVisibly(true);
    return ret;
}

void ConfigSelectScene::processMenu()
{
    if(!menu.isSelected())
        return;

    if(menu.isAccepted())
    {
        int key = menu.currentItem().item_key.toInt();
        ConfigPackEntry &item = m_availablePacks[key];
        currentConfig       = item.key;
        currentConfigPath   = item.path;
        ret = 1;
    }
    else
        ret = 0;

    m_doExit = true;
    m_fader.setFade(10, 1.0, 0.05);
}

void ConfigSelectScene::processEvents()
{
    SDL_PumpEvents();
    controller->update();
    Scene::processEvents();
}

void ConfigSelectScene::setLabel(QString label)
{
    m_label = label;
}
