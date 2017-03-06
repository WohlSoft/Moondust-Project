#ifndef SETUP_WLD_SCENE_H
#define SETUP_WLD_SCENE_H

#include <vector>
#include <graphics/gl_color.h>

class IniProcessing;

struct WorldAdditionalImage
{
    std::string imgFile;
    bool animated;
    int frames;
    int framedelay;
    int x;
    int y;
};

struct WorldMapSetup
{
    void init(IniProcessing &engine_ini);
    void initFonts();
    std::string backgroundImg;
    int viewport_x; //World map view port
    int viewport_y;
    int viewport_w;
    int viewport_h;
    enum titleAlign
    {
        align_left=0,
        align_center,
        align_right
    };

    int         title_x; //Title of level
    int         title_y;
    int         title_w; //max width of title
    std::string title_color;
    GlColor     title_rgba;
    titleAlign  title_align;
    std::string title_font_name;
    int         title_fontID;

    bool    points_en;
    int     points_x;
    int     points_y;
    std::string points_font_name;
    int     points_fontID;
    std::string points_color;
    GlColor points_rgba;

    bool    health_en;
    int     health_x;
    int     health_y;
    std::string health_font_name;
    int     health_fontID;
    std::string health_color;
    GlColor health_rgba;

    bool    lives_en;
    int     lives_x;
    int     lives_y;
    std::string lives_font_name;
    int     lives_fontID;
    std::string lives_color;
    GlColor lives_rgba;

    bool    star_en;
    int     star_x;
    int     star_y;
    std::string star_font_name;
    int     star_fontID;
    std::string star_color;
    GlColor star_rgba;

    bool    coin_en;
    int     coin_x;
    int     coin_y;
    std::string coin_font_name;
    int     coin_fontID;
    std::string coin_color;
    GlColor coin_rgba;

    bool    portrait_en;
    int     portrait_x;
    int     portrait_y;
    int     portrait_frame_delay;
    std::string portrait_animation;
    int     portrait_direction;
    std::vector<WorldAdditionalImage > AdditionalImages;

    std::string luaFile;
};

#endif // SETUP_WLD_SCENE_H

