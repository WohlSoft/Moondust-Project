#ifndef SETUP_WLD_SCENE_H
#define SETUP_WLD_SCENE_H

#include <QString>
#include <graphics/gl_color.h>
class QSettings;

struct WorldAdditionalImage
{
    QString imgFile;
    bool animated;
    int frames;
    int framedelay;
    int x;
    int y;
};

struct WorldMapSetup
{
    void init(QSettings &engine_ini);
    void initFonts();
    QString backgroundImg;
    int viewport_x; //World map view port
    int viewport_y;
    int viewport_w;
    int viewport_h;
    enum titleAlign{
        align_left=0,
        align_center,
        align_right
    };

    int         title_x; //Title of level
    int         title_y;
    int         title_w; //max width of title
    QString     title_color;
    GlColor     title_rgba;
    titleAlign  title_align;
    QString     title_font_name;
    int         title_fontID;

    bool    points_en;
    int     points_x;
    int     points_y;
    QString points_font_name;
    int     points_fontID;
    QString points_color;
    GlColor points_rgba;

    bool    health_en;
    int     health_x;
    int     health_y;
    QString health_font_name;
    int     health_fontID;
    QString health_color;
    GlColor health_rgba;

    bool    lives_en;
    int     lives_x;
    int     lives_y;
    QString lives_font_name;
    int     lives_fontID;
    QString lives_color;
    GlColor lives_rgba;

    bool    star_en;
    int     star_x;
    int     star_y;
    QString star_font_name;
    int     star_fontID;
    QString star_color;
    GlColor star_rgba;

    bool    coin_en;
    int     coin_x;
    int     coin_y;
    QString coin_font_name;
    int     coin_fontID;
    QString coin_color;
    GlColor coin_rgba;

    bool    portrait_en;
    int     portrait_x;
    int     portrait_y;
    int     portrait_frame_delay;
    QString portrait_animation;
    int     portrait_direction;
    QList<WorldAdditionalImage > AdditionalImages;

    QString luaFile;
};

#endif // SETUP_WLD_SCENE_H

