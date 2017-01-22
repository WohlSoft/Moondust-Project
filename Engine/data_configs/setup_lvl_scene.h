#ifndef SETUP_LVL_SCENE_H
#define SETUP_LVL_SCENE_H

#include <vector>
#include <string>
#include <graphics/gl_color.h>
class IniProcessing;

struct LevelAdditionalImage
{
    std::string imgFile;
    bool animated;
    int frames;
    int framedelay;
    int x;
    int y;
    bool from_center;
};

struct LevelSetup
{
    void init(IniProcessing& engine_ini);
    void initFonts();

    bool        points_en;
    int         points_x;
    int         points_y;
    std::string points_font_name;
    int         points_fontID;
    std::string points_color;
    GlColor     points_rgba;
    bool        points_from_center;

    bool        health_en;
    int         health_x;
    int         health_y;
    std::string health_font_name;
    int         health_fontID;
    std::string health_color;
    GlColor     health_rgba;
    bool        health_from_center;

    bool        star_en;
    int         star_x;
    int         star_y;
    std::string star_font_name;
    int         star_fontID;
    std::string star_color;
    GlColor     star_rgba;
    bool        star_from_center;

    bool        coin_en;
    int         coin_x;
    int         coin_y;
    std::string coin_font_name;
    int         coin_fontID;
    std::string coin_color;
    GlColor     coin_rgba;
    bool        coin_from_center;

    bool        itemslot_en;
    int         itemslot_x;
    int         itemslot_y;
    int         itemslot_w;
    int         itemslot_h;
    bool        itemslot_to_center;
    std::vector<LevelAdditionalImage > AdditionalImages;

    std::string luaFile;
};


#endif // SETUP_LVL_SCENE_H

