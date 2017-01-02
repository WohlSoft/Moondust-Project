/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2017 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "config_manager.h"
#include "config_manager_private.h"
#include <QFileInfo>
#include <gui/pge_msgbox.h>

bool ConfigManager::loadEngineSettings()
{
    QString engine_ini = config_dir + "engine.ini";

    if(!QFileInfo(engine_ini).exists())
    {
        PGE_MsgBox::error(QString("Config error!\nCan't open the 'engine.ini' config file!"));
        return false;
    }

    QSettings engineset(engine_ini, QSettings::IniFormat);
    engineset.setIniCodec("UTF-8");
    engineset.beginGroup("window");
    {
        config_name = engineset.value("title", QString::fromStdString(config_name)).toString().toStdString();
    }
    engineset.endGroup();
    engineset.beginGroup("fonts");
    {
        setup_fonts.fontname = engineset.value("font-file", "").toString();
        setup_fonts.double_pixled = engineset.value("double-pixled", false).toBool();
        setup_fonts.rasterFontsFile = engineset.value("raster-fonts", "").toString();
    }
    engineset.endGroup();
    engineset.beginGroup("common");
    {
        screen_width    = engineset.value("screen-width",  800).toUInt();
        screen_height   = engineset.value("screen-height", 600).toUInt();
        QString scrType = engineset.value("screen-type", "static").toString();

        if(scrType == "dynamic")
            screen_type = SCR_Dynamic;
        else
            screen_type = SCR_Static;

        setup_cursors.normal = engineset.value("cursor-image-normal", "").toString();
        checkForImage(setup_cursors.normal, dirs.gcommon);
        setup_cursors.rubber = engineset.value("cursor-image-rubber", "").toString();
        checkForImage(setup_cursors.rubber, dirs.gcommon);
    }
    engineset.endGroup();
    engineset.beginGroup("message-box");
    {
        setup_message_box.sprite = engineset.value("image", "").toString();
        checkForImage(setup_message_box.sprite, dirs.gcommon);
        setup_message_box.box_padding = static_cast<double>(engineset.value("box-padding", 20).toUInt());
        setup_message_box.borderWidth = static_cast<int>(engineset.value("border-width", 32).toUInt());
        setup_message_box.font_name = engineset.value("font", "font2").toString();
        setup_message_box.font_color = engineset.value("font-color", "#FFFFFF").toString();
        setup_message_box.font_rgba.setRgba(setup_message_box.font_color);
        setup_message_box.font_id = 0;
    }
    engineset.endGroup();
    engineset.beginGroup("menu-box");
    {
        setup_menu_box.sprite = engineset.value("image", "").toString();
        checkForImage(setup_menu_box.sprite, dirs.gcommon);
        setup_menu_box.box_padding = static_cast<double>(engineset.value("box-padding", 20).toULongLong());
        setup_menu_box.borderWidth = static_cast<int>(engineset.value("border-width", 32).toUInt());
        setup_menu_box.title_font_name = engineset.value("title-font", "font2").toString();
        setup_menu_box.title_font_color = engineset.value("title-font-color", "#FF0000").toString();
        setup_menu_box.title_font_rgba.setRgba(setup_menu_box.title_font_color);
        setup_menu_box.title_font_id = 0;
    }
    engineset.endGroup();
    engineset.beginGroup("menu");
    {
        setup_menus.selector = engineset.value("selector", "").toString();
        checkForImage(setup_menus.selector, dirs.gcommon);
        setup_menus.scrollerUp = engineset.value("scroll-up", "").toString();
        checkForImage(setup_menus.scrollerUp, dirs.gcommon);
        setup_menus.scrollerDown = engineset.value("scroll-down", "").toString();
        checkForImage(setup_menus.scrollerDown, dirs.gcommon);
        setup_menus.item_height = abs(engineset.value("item-height", 32).toInt());
        setup_menus.font_offset = abs(engineset.value("font-offset", 10).toInt());
        setup_menus.font_name = engineset.value("font", "font1").toString();
        setup_menus.font_id = 0;
    }
    engineset.endGroup();
    ////// World map settings
    setup_WorldMap.init(engineset);
    ////// Level settings
    setup_Level.init(engineset);
    ////////// Loading scene settings
    setup_LoadingScreen.init(engineset);
    ////////// Credits scene settings
    setup_CreditsScreen.init(engineset);
    ////////// Title screen (main menu) settings
    setup_TitleScreen.init(engineset);
    return true;
}
