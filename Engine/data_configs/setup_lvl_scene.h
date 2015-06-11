#ifndef SETUP_LVL_SCENE_H
#define SETUP_LVL_SCENE_H

#include <QString>
#include <graphics/gl_color.h>
class QSettings;

struct LevelAdditionalImage
{
    QString imgFile;
    bool animated;
    int frames;
    int framedelay;
    int x;
    int y;
    bool from_center;
};

struct LevelSetup
{
    void init(QSettings &engine_ini);
    void initFonts();

    bool    points_en;
    int     points_x;
    int     points_y;
    QString points_font_name;
    int     points_fontID;
    QString points_color;
    GlColor points_rgba;
    bool    points_from_center;

    bool    health_en;
    int     health_x;
    int     health_y;
    QString health_font_name;
    int     health_fontID;
    QString health_color;
    GlColor health_rgba;
    bool    health_from_center;

    bool    star_en;
    int     star_x;
    int     star_y;
    QString star_font_name;
    int     star_fontID;
    QString star_color;
    GlColor star_rgba;
    bool    star_from_center;

    bool    coin_en;
    int     coin_x;
    int     coin_y;
    QString coin_font_name;
    int     coin_fontID;
    QString coin_color;
    GlColor coin_rgba;
    bool    coin_from_center;

    bool    itemslot_en;
    int     itemslot_x;
    int     itemslot_y;
    int     itemslot_w;
    int     itemslot_h;
    bool    itemslot_to_center;
    QList<LevelAdditionalImage > AdditionalImages;

    QString luaFile;
};


#endif // SETUP_LVL_SCENE_H

