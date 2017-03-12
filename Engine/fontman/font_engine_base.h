#ifndef FONT_ENGINE_BASE_H
#define FONT_ENGINE_BASE_H

#include <common_features/size.h>
#include <string>

class BaseFontEngine
{
public:
    virtual ~BaseFontEngine();

    virtual PGE_Size textSize(std::string &text,
                              uint32_t max_line_lenght = 0,
                              bool cut = false, uint32_t fontSize = 14) = 0;

    virtual void printText(const std::string &text,
                           int32_t x, int32_t y,
                           float Red = 1.f, float Green = 1.f, float Blue = 1.f, float Alpha = 1.f,
                           uint32_t fontSize = 14) = 0;

    virtual bool isLoaded() = 0;

    virtual std::string getFontName() = 0;
};

#endif // FONT_ENGINE_BASE_H
