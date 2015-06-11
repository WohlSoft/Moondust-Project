
#include <QString>
#include <QSettings>

#include "setup_lvl_scene.h"
#include "config_manager.h"
#include <fontman/font_manager.h>

LevelSetup ConfigManager::setup_Level;

void LevelSetup::init(QSettings &engine_ini)
{
    int LoadScreenImages=0;
    engine_ini.beginGroup("level");
        points_en = engine_ini.value("points-counter", false).toBool();
        points_x = engine_ini.value("points-counter-x", "").toInt();
        points_y = engine_ini.value("points-counter-y", "").toInt();
        points_font_name= engine_ini.value("points-font", "numeric").toString();
        points_fontID=0;
        points_color = engine_ini.value("points-color", "#FFFFFF").toString();
        points_rgba.setRgba(points_color);
        points_from_center = engine_ini.value("points-relative-from-center", false).toBool();

        health_en = engine_ini.value("health-counter", false).toBool();
        health_x = engine_ini.value("health-counter-x", "").toInt();
        health_y = engine_ini.value("health-counter-y", "").toInt();
        health_font_name= engine_ini.value("health-counter-font", "numeric").toString();
        health_fontID=0;
        health_color = engine_ini.value("health-counter-color", "#FFFFFF").toString();
        health_rgba.setRgba(health_color);
        health_from_center = engine_ini.value("health-relative-from-center", false).toBool();

        star_en = engine_ini.value("star-counter", false).toBool();
        star_x = engine_ini.value("star-counter-x", "").toInt();
        star_y = engine_ini.value("star-counter-y", "").toInt();
        star_font_name= engine_ini.value("star-counter-font", "numeric").toString();
        star_fontID=0;
        star_color = engine_ini.value("star-counter-color", "#FFFFFF").toString();
        star_rgba.setRgba(star_color);
        star_from_center = engine_ini.value("star-relative-from-center", false).toBool();

        coin_en = engine_ini.value("coin-counter", false).toBool();
        coin_x = engine_ini.value("coin-counter-x", "").toInt();
        coin_y = engine_ini.value("coin-counter-y", "").toInt();
        coin_font_name= engine_ini.value("coin-counter-font", "numeric").toString();
        coin_fontID=0;
        coin_color = engine_ini.value("coin-counter-color", "#FFFFFF").toString();
        coin_rgba.setRgba(coin_color);
        coin_from_center = engine_ini.value("star-relative-from-center", false).toBool();

        itemslot_en = engine_ini.value("itemslot", false).toBool();
        itemslot_x = engine_ini.value("itemslot-x", "").toInt();
        itemslot_y = engine_ini.value("itemslot-y", "").toInt();
        itemslot_w = engine_ini.value("itemslot-w", "").toInt();
        itemslot_h = engine_ini.value("itemslot-h", "").toInt();

        luaFile = engine_ini.value("script", "main_level.lua").toString();

        LoadScreenImages = engine_ini.value("additional-images", 0).toInt();
    engine_ini.endGroup();

    AdditionalImages.clear();
    for(int i=1; i<=LoadScreenImages; i++)
    {
        engine_ini.beginGroup(QString("world-image-%1").arg(i));
            LevelAdditionalImage img;

            img.imgFile = engine_ini.value("image", "").toString();
            ConfigManager::checkForImage(img.imgFile, ConfigManager::dirs.gcommon);

            img.animated = engine_ini.value("animated", false).toBool();
            if(img.animated)
            {
                img.frames = engine_ini.value("frames", 1).toInt();
                img.framedelay = engine_ini.value("frames-delay", 128).toInt();
            } else {
                img.frames = 1;
                img.framedelay = 128;
            }
            if(img.frames<=0) img.frames = 1;

            img.x =  engine_ini.value("pos-x", 1).toInt();
            img.y =  engine_ini.value("pos-y", 1).toInt();
            img.from_center =  engine_ini.value("relative-from-center", false).toBool();
            AdditionalImages.push_back(img);
        engine_ini.endGroup();
    }
}

void LevelSetup::initFonts()
{
    points_fontID=FontManager::getFontID(points_font_name);
    star_fontID=FontManager::getFontID(star_font_name);
    coin_fontID=FontManager::getFontID(coin_font_name);
    health_fontID=FontManager::getFontID(health_font_name);
}

