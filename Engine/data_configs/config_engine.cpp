

#include "config_manager.h"
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

    engineset.beginGroup("fonts");
        setup_fonts.fontname = engineset.value("font-file", "").toString();
        setup_fonts.double_pixled = engineset.value("double-pixled", false).toBool();
        setup_fonts.rasterFontsFile = engineset.value("raster-fonts", "").toString();
    engineset.endGroup();

    engineset.beginGroup("common");
        screen_width = engineset.value("screen-width", 800).toInt();
        screen_height = engineset.value("screen-height", 600).toInt();
        QString scrType = engineset.value("screen-type", "static").toString();

        if(scrType == "dynamic")
            screen_type = SCR_Dynamic;
        else
            screen_type = SCR_Static;

        setup_cursors.normal = engineset.value("cursor-image-normal", "").toString();
        checkForImage(setup_cursors.normal, dirs.gcommon);

        setup_cursors.rubber = engineset.value("cursor-image-rubber", "").toString();
        checkForImage(setup_cursors.rubber, dirs.gcommon);
    engineset.endGroup();



    engineset.beginGroup("message-box");
        setup_message_box.sprite = engineset.value("image", "").toString();
        checkForImage(setup_message_box.sprite, dirs.gcommon);
        setup_message_box.box_padding = (float)(unsigned)engineset.value("box-padding", 20).toInt();
        setup_message_box.borderWidth = (unsigned)engineset.value("border-width", 32).toInt();
        setup_message_box.font_name = engineset.value("font", "font2").toString();
        setup_message_box.font_color = engineset.value("font-color", "#FFFFFF").toString();
        setup_message_box.font_rgba.setRgba(setup_message_box.font_color);
        setup_message_box.font_id=0;
    engineset.endGroup();

    engineset.beginGroup("menu-box");
        setup_menu_box.sprite = engineset.value("image", "").toString();
        checkForImage(setup_menu_box.sprite, dirs.gcommon);
        setup_menu_box.box_padding = (float)(unsigned)engineset.value("box-padding", 20).toInt();
        setup_menu_box.borderWidth = (unsigned)engineset.value("border-width", 32).toInt();
        setup_menu_box.title_font_name = engineset.value("title-font", "font2").toString();
        setup_menu_box.title_font_color = engineset.value("title-font-color", "#FF0000").toString();
        setup_menu_box.title_font_rgba.setRgba(setup_menu_box.title_font_color);
        setup_menu_box.title_font_id=0;
    engineset.endGroup();

    engineset.beginGroup("menu");
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
    engineset.endGroup();

    ////// World map settings
    setup_WorldMap.init(engineset);

    ////////// Loading scene settings
    setup_LoadingScreen.init(engineset);

    ////////// Credits scene settings
    setup_CreditsScreen.init(engineset);

    ////////// Title screen (main menu) settings
    setup_TitleScreen.init(engineset);

    return true;
}
