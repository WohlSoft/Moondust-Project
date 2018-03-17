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

#include <gui/pge_msgbox.h>
#include <IniProcessor/ini_processing.h>
#include <Utils/files.h>

bool ConfigManager::loadEngineSettings()
{
    std::string engine_ini = config_dirSTD + "engine.ini";

    if(!Files::fileExists(engine_ini))
    {
        PGE_MsgBox::error("Config error!\nCan't open the 'engine.ini' config file!");
        return false;
    }

    IniProcessing engineset(engine_ini);

    engineset.beginGroup("window");
    {
        engineset.read("title", config_name, config_name);
    }
    engineset.endGroup();

    engineset.beginGroup("fonts");
    {
        size_t ttfFontsCount = 0;
        std::string ttfFontFile;

        engineset.read("ttf-font-default", setup_fonts.fontname, "");
        engineset.read("ttf-double-pixel", setup_fonts.double_pixled, false);
        //DEPRECATIONS
        engineset.read("font-file", setup_fonts.fontname, setup_fonts.fontname);//DEPRECATED
        engineset.read("double-pixled", setup_fonts.double_pixled, setup_fonts.double_pixled);//DEPRECATED
        //DEPRECATIONS END

        engineset.read("ttf-fonts-count", ttfFontsCount, 0);
        setup_fonts.ttfFonts.clear();
        setup_fonts.ttfFonts.reserve(ttfFontsCount);
        for(size_t i = 0; i < ttfFontsCount; i++)
        {
            engineset.read(fmt::format_ne("ttf-font-{0}", i).c_str(), ttfFontFile, "");
            if(!ttfFontFile.empty())
                setup_fonts.ttfFonts.push_back(ttfFontFile);
        }
    }
    engineset.endGroup();
    engineset.beginGroup("common");
    {
        engineset.read("screen-width", screen_width, 800);
        engineset.read("screen-height", screen_height, 600);
        std::string scrType;
        engineset.read("screen-type", scrType, "static");
        if(scrType == "dynamic")
            screen_type = SCR_Dynamic;
        else
            screen_type = SCR_Static;
        engineset.read("cursor-image-normal", setup_cursors.normal, "");
        checkForImage(setup_cursors.normal, dirs.gcommon);
        engineset.read("cursor-image-rubber", setup_cursors.rubber, "");
        checkForImage(setup_cursors.rubber, dirs.gcommon);
    }
    engineset.endGroup();
    engineset.beginGroup("message-box");
    {
        engineset.read("image", setup_message_box.sprite, "");
        checkForImage(setup_message_box.sprite, dirs.gcommon);
        setup_message_box.box_padding = static_cast<double>(engineset.value("box-padding", 20).toUInt());
        setup_message_box.borderWidth = static_cast<int>(engineset.value("border-width", 32).toUInt());
        engineset.read("font", setup_message_box.font_name, "font2");
        engineset.read("font-color", setup_message_box.font_color, "#FFFFFF");
        setup_message_box.font_rgba.setRgba(setup_message_box.font_color);
        setup_message_box.font_id = 0;
    }
    engineset.endGroup();
    engineset.beginGroup("menu-box");
    {
        engineset.read("image", setup_menu_box.sprite, "");
        checkForImage(setup_menu_box.sprite, dirs.gcommon);
        setup_menu_box.box_padding = static_cast<double>(engineset.value("box-padding", 20).toULongLong());
        setup_menu_box.borderWidth = static_cast<int>(engineset.value("border-width", 32).toUInt());
        engineset.read("title-font", setup_menu_box.title_font_name, "font2");
        engineset.read("title-font-color", setup_menu_box.title_font_color, "#FF0000");
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
