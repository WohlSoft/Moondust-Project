#ifndef SETUP_TITLE_SCREEN_H
#define SETUP_TITLE_SCREEN_H

#include <QString>
#include <QColor>
#include <QList>

#include <string>
#include <vector>
#include <graphics/gl_color.h>

class IniProcessing;

struct TitleScreenAdditionalImage
{
    enum align
    {
        NO_ALIGN = 0,
        LEFT_ALIGN,
        TOP_ALIGN,
        RIGHT_ALIGN,
        BOTTOM_ALIGN,
        CENTER_ALIGN
    };

    std::string imgFile;
    bool animated;
    int frames;
    unsigned int framespeed;
    align align_to;
    int x;
    int y;
    bool center_x;
    bool center_y;
};

struct TitleScreenSetup
{
    void init(IniProcessing &engine_ini);
    std::string backgroundImg;
    GlColor backgroundColor;
    std::vector<TitleScreenAdditionalImage > AdditionalImages;
    std::string luaFile;
};

#endif // SETUP_TITLE_SCREEN_H
