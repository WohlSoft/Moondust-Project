
#include <QString>
#include <QSettings>

#include "setup_wld_scene.h"
#include "config_manager.h"

WorldMapSetup ConfigManager::setup_WorldMap;

void WorldMapSetup::init(QSettings &engine_ini)
{
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

        health_en = engine_ini.value("health-counter", false).toBool();
        health_x = engine_ini.value("health-counter-x", "").toInt();
        health_y = engine_ini.value("health-counter-y", "").toInt();

        star_en = engine_ini.value("star-counter", false).toBool();
        star_x = engine_ini.value("star-counter-x", "").toInt();
        star_y = engine_ini.value("star-counter-y", "").toInt();

        coin_en = engine_ini.value("coin-counter", false).toBool();
        coin_x = engine_ini.value("coin-counter-x", "").toInt();
        coin_y = engine_ini.value("coin-counter-y", "").toInt();

        portrait_en = engine_ini.value("portrait", false).toBool();
        portrait_x = engine_ini.value("portrait-x", "").toInt();
        portrait_y = engine_ini.value("portrait-y", "").toInt();
        portrait_frame_delay = engine_ini.value("portrait-frame-delay", 64).toInt();
        portrait_animation = engine_ini.value("portrait-animation", "Run").toString();
        portrait_direction = engine_ini.value("portrait-direction", -1).toInt();
    engine_ini.endGroup();
}
