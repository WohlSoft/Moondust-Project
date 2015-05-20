#include "gl_color.h"

#include <QColor>

GlColor::GlColor()
{
    r=1.0;
    g=1.0;
    b=1.0;
    a=1.0;
}

GlColor::GlColor(const GlColor &clr)
{
    this->r=clr.r;
    this->g=clr.g;
    this->b=clr.b;
    this->a=clr.a;
}

GlColor::GlColor(GLuint rgba)
{
    setRgba(rgba);
}

GlColor::GlColor(QString rgba)
{
    setRgba(rgba);
}

GlColor::~GlColor()
{}

void GlColor::setRgba(GLuint rgba)
{
    r = (float)((rgba & 0xFF000000)>>24)/255.0;
    g = (float)((rgba & 0x00FF0000)>>16)/255.0;
    b = (float)((rgba & 0x0000FF00)>>8)/255.0;
    a = (float)((rgba & 0x000000FF))/255.0;
}

void GlColor::setRgba(QString rgba)
{
    QColor color;
    color.setNamedColor(rgba);
    r=color.redF();
    g=color.greenF();
    b=color.blueF();
    a=color.alphaF();
}

void GlColor::setRed(float _r)
{
    r=_r;
}

void GlColor::setGreen(float _g)
{
    g=_g;
}

void GlColor::setBlue(float _b)
{
    b=_b;
}

void GlColor::setAlpha(float _a)
{
    a=_a;
}

void GlColor::setRedI(float _r)
{
    r=_r/255.0;
}

void GlColor::setGreenI(float _g)
{
    g=_g/255.0;
}

void GlColor::setBlueI(float _b)
{
    b=_b/255.0;
}

void GlColor::setAlphaI(float _a)
{
    a=_a/255.0;
}

float GlColor::Red()
{
    return r;
}

float GlColor::Green()
{
    return g;
}

float GlColor::Blue()
{
    return b;
}

float GlColor::Alpha()
{
    return a;
}

