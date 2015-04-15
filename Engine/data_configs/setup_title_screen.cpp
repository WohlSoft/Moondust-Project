
#include "setup_title_screen.h"
#include "config_manager.h"

#include <QSettings>

TitleScreenSetup ConfigManager::setup_TitleScreen;

void TitleScreenSetup::init(QSettings &engine_ini)
{
    int TitleScreenImages=0;
    engine_ini.beginGroup("title-screen");
        backgroundImg = engine_ini.value("background", "").toString();
        backgroundColor.setNamedColor(engine_ini.value("bg-color", "#000000").toString());
        ConfigManager::checkForImage(backgroundImg, ConfigManager::dirs.gcommon);
        TitleScreenImages = engine_ini.value("additional-images", 0).toInt();
    engine_ini.endGroup();


    AdditionalImages.clear();
    for(int i=1; i<=TitleScreenImages; i++)
    {
        engine_ini.beginGroup(QString("title-image-%1").arg(i));
            TitleScreenAdditionalImage img;

            img.imgFile = engine_ini.value("image", "").toString();
            ConfigManager::checkForImage(img.imgFile, ConfigManager::dirs.gcommon);

            img.animated = engine_ini.value("animated", false).toBool();
            if(img.animated)
            {
                img.frames = engine_ini.value("frames", 1).toInt();
                img.framespeed = engine_ini.value("framespeed", 128).toInt();
            }
            else
            {
                img.frames = 1;
                img.framespeed = 128;
            }
            if(img.frames<=0) img.frames = 1;

            img.x =  engine_ini.value("pos-x", 0).toInt();
            img.y =  engine_ini.value("pos-y", 0).toInt();

            img.center_x =  engine_ini.value("center-x", false).toBool();
            img.center_y =  engine_ini.value("center-y", false).toBool();

            QString align =   engine_ini.value("align", "none").toString();

            if(align=="left")
                img.align_to = TitleScreenAdditionalImage::LEFT_ALIGN;
            else
            if(align=="top")
                img.align_to = TitleScreenAdditionalImage::TOP_ALIGN;
            else
            if(align=="right")
                img.align_to = TitleScreenAdditionalImage::RIGHT_ALIGN;
            else
            if(align=="bottom")
                img.align_to = TitleScreenAdditionalImage::BOTTOM_ALIGN;
            else
            if(align=="center")
                img.align_to = TitleScreenAdditionalImage::CENTER_ALIGN;
            else
            img.align_to = TitleScreenAdditionalImage::NO_ALIGN;

            AdditionalImages.push_back(img);
        engine_ini.endGroup();
    }
}
