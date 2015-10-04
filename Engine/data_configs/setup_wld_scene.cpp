
#include <QString>
#include <QSettings>

#include "setup_wld_scene.h"
#include "config_manager.h"
#include <fontman/font_manager.h>


WorldMapSetup ConfigManager::setup_WorldMap;

void WorldMapSetup::init(QSettings &engine_ini)
{
    int LoadScreenImages=0;
    engine_ini.beginGroup("world-map");
        backgroundImg = engine_ini.value("background", "").toString();
        ConfigManager::checkForImage(backgroundImg, ConfigManager::dirs.gcommon);
        viewport_x = engine_ini.value("viewport-x", "").toInt();
        viewport_y = engine_ini.value("viewport-y", "").toInt();
        viewport_w = engine_ini.value("viewport-width", "").toInt();
        viewport_h = engine_ini.value("viewport-height", "").toInt();

        title_x = engine_ini.value("level-title-x", "").toInt();
        title_y = engine_ini.value("level-title-y", "").toInt();
        title_w = engine_ini.value("level-title-w", "").toInt();
        title_font_name= engine_ini.value("level-title-font", "font3").toString();
        title_fontID=0;
        title_color = engine_ini.value("level-title-color", "#000000").toString();
        title_rgba.setRgba(title_color);

        QString ttlAlign = engine_ini.value("level-title-align", "left").toString();
        if(ttlAlign=="center")
            title_align = WorldMapSetup::align_center;
        else
        if(ttlAlign=="right")
            title_align = WorldMapSetup::align_right;
        else
            title_align = WorldMapSetup::align_left;

        points_en = engine_ini.value("points-counter", false).toBool();
        points_x = engine_ini.value("points-counter-x", "").toInt();
        points_y = engine_ini.value("points-counter-y", "").toInt();
        points_font_name= engine_ini.value("points-font", "numeric").toString();
        points_fontID=0;
        points_color = engine_ini.value("points-color", "#FFFFFF").toString();
        points_rgba.setRgba(points_color);

        health_en = engine_ini.value("health-counter", false).toBool();
        health_x = engine_ini.value("health-counter-x", "").toInt();
        health_y = engine_ini.value("health-counter-y", "").toInt();
        health_font_name= engine_ini.value("health-counter-font", "numeric").toString();
        health_fontID=0;
        health_color = engine_ini.value("health-counter-color", "#FFFFFF").toString();
        health_rgba.setRgba(health_color);

        lives_en = engine_ini.value("lives-counter", false).toBool();
        lives_x = engine_ini.value("lives-counter-x", "").toInt();
        lives_y = engine_ini.value("lives-counter-y", "").toInt();
        lives_font_name= engine_ini.value("lives-counter-font", "numeric").toString();
        lives_fontID=0;
        lives_color = engine_ini.value("lives-counter-color", "#FFFFFF").toString();
        lives_rgba.setRgba(health_color);

        star_en = engine_ini.value("star-counter", false).toBool();
        star_x = engine_ini.value("star-counter-x", "").toInt();
        star_y = engine_ini.value("star-counter-y", "").toInt();
        star_font_name= engine_ini.value("star-counter-font", "numeric").toString();
        star_fontID=0;
        star_color = engine_ini.value("star-counter-color", "#FFFFFF").toString();
        star_rgba.setRgba(star_color);

        coin_en = engine_ini.value("coin-counter", false).toBool();
        coin_x = engine_ini.value("coin-counter-x", "").toInt();
        coin_y = engine_ini.value("coin-counter-y", "").toInt();
        coin_font_name= engine_ini.value("coin-counter-font", "numeric").toString();
        coin_fontID=0;
        coin_color = engine_ini.value("coin-counter-color", "#FFFFFF").toString();
        coin_rgba.setRgba(coin_color);

        portrait_en = engine_ini.value("portrait", false).toBool();
        portrait_x = engine_ini.value("portrait-x", "").toInt();
        portrait_y = engine_ini.value("portrait-y", "").toInt();
        portrait_frame_delay = engine_ini.value("portrait-frame-delay", 64).toInt();
        portrait_animation = engine_ini.value("portrait-animation", "Run").toString();
        portrait_direction = engine_ini.value("portrait-direction", -1).toInt();

        luaFile = engine_ini.value("script", "main_world.lua").toString();

        LoadScreenImages = engine_ini.value("additional-images", 0).toInt();
    engine_ini.endGroup();

    AdditionalImages.clear();
    for(int i=1; i<=LoadScreenImages; i++)
    {
        engine_ini.beginGroup(QString("world-image-%1").arg(i));
            WorldAdditionalImage img;

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
            AdditionalImages.push_back(img);
        engine_ini.endGroup();
    }
}

void WorldMapSetup::initFonts()
{
    title_fontID=FontManager::getFontID(title_font_name);
    points_fontID=FontManager::getFontID(points_font_name);
    star_fontID=FontManager::getFontID(star_font_name);
    coin_fontID=FontManager::getFontID(coin_font_name);
    health_fontID=FontManager::getFontID(health_font_name);
}
