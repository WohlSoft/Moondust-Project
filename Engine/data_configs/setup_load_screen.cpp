#include <QSettings>

#include "setup_load_screen.h"
#include "config_manager.h"

LoadingScreenSetup ConfigManager::setup_LoadingScreen;

void LoadingScreenSetup::init(QSettings &engine_ini)
{
    int LoadScreenImages=0;
    engine_ini.beginGroup("loading-scene");
        backgroundColor.setNamedColor(engine_ini.value("bg-color", "#000000").toString());
        backgroundImg = engine_ini.value("background", "").toString();
        ConfigManager::checkForImage(backgroundImg, ConfigManager::dirs.gcommon);

        updateDelay = engine_ini.value("updating-time", 128).toInt();
        LoadScreenImages = engine_ini.value("additional-images", 0).toInt();
    engine_ini.endGroup();


    AdditionalImages.clear();
    for(int i=1; i<=LoadScreenImages; i++)
    {
        engine_ini.beginGroup(QString("loading-image-%1").arg(i));
            LoadingScreenAdditionalImage img;

            img.imgFile = engine_ini.value("image", "").toString();
            ConfigManager::checkForImage(img.imgFile, ConfigManager::dirs.gcommon);

            img.animated = engine_ini.value("animated", false).toBool();
            if(img.animated)
                img.frames = engine_ini.value("frames", 1).toInt();
            else
                img.frames = 1;
            if(img.frames<=0) img.frames = 1;

            img.x =  engine_ini.value("pos-x", 1).toInt();
            img.y =  engine_ini.value("pos-y", 1).toInt();
            AdditionalImages.push_back(img);
        engine_ini.endGroup();
    }

}
