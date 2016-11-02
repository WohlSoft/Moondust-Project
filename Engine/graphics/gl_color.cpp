#include "gl_color.h"

#include <QColor>
#include <cmath>

GlColor::GlColor()
{
    m_r = 1.0;
    m_g = 1.0;
    m_b = 1.0;
    m_a = 1.0;
}

GlColor::GlColor(const GlColor &clr)
{
    this->m_r = clr.m_r;
    this->m_g = clr.m_g;
    this->m_b = clr.m_b;
    this->m_a = clr.m_a;
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

void GlColor::setRgb(GLuint rgb)
{
    m_r = static_cast<double>((rgb & 0xFF0000) >> 16) / 255.0;
    m_g = static_cast<double>((rgb & 0x00FF00) >> 8) / 255.0;
    m_b = static_cast<double>((rgb & 0x0000FF) >> 0) / 255.0;
}

void GlColor::setRgba(GLuint rgba)
{
    m_r = static_cast<double>((rgba & 0xFF000000) >> 24) / 255.0;
    m_g = static_cast<double>((rgba & 0x00FF0000) >> 16) / 255.0;
    m_b = static_cast<double>((rgba & 0x0000FF00) >> 8) / 255.0;
    m_a = static_cast<double>((rgba & 0x000000FF)) / 255.0;
}

void GlColor::setRgba(QString rgba)
{
    QColor color;
    color.setNamedColor(rgba);
    m_r = color.redF();
    m_g = color.greenF();
    m_b = color.blueF();
    m_a = color.alphaF();
}

void GlColor::setRgba(double _r, double _g, double _b, double _a)
{
    m_r = _r;
    m_g = _g;
    m_b = _b;
    m_a = _a;
}

void GlColor::setHsv(double _h, double _s, double _v)
{
    double      hh, p, q, t, ff;
    long        i;

    if(_s <= 0.0)         // < is bogus, just shuts up warnings
    {
        m_r = _v;
        m_g = _v;
        m_b = _v;
        return;
    }

    hh = _h;

    if(hh >= 360.0) hh = 0.0;

    hh /= 60.0;
    i = static_cast<long>(hh);
    ff = hh - i;
    p = _v * (1.0 - _s);
    q = _v * (1.0 - (_s * ff));
    t = _v * (1.0 - (_s * (1.0 - ff)));

    switch(i)
    {
    case 0:
        m_r = _v;
        m_g = t;
        m_b = p;
        break;

    case 1:
        m_r = q;
        m_g = _v;
        m_b = p;
        break;

    case 2:
        m_r = p;
        m_g = _v;
        m_b = t;
        break;

    case 3:
        m_r = p;
        m_g = q;
        m_b = _v;
        break;

    case 4:
        m_r = t;
        m_g = p;
        m_b = _v;
        break;

    case 5:
    default:
        m_r = _v;
        m_g = p;
        m_b = q;
        break;
    }
}

void GlColor::setHsva(double _h, double _s, double _v, double _a)
{
    m_a = _a;
    setHsv(_h, _s, _v);
}

void GlColor::setRed(double _r)
{
    m_r = _r;
}

void GlColor::setGreen(double _g)
{
    m_g = _g;
}

void GlColor::setBlue(double _b)
{
    m_b = _b;
}

void GlColor::setAlpha(double _a)
{
    m_a = _a;
}

void GlColor::setRedI(double _r)
{
    m_r = _r / 255.0;
}

void GlColor::setGreenI(double _g)
{
    m_g = _g / 255.0;
}

void GlColor::setBlueI(double _b)
{
    m_b = _b / 255.0;
}

void GlColor::setAlphaI(double _a)
{
    m_a = _a / 255.0;
}

float GlColor::Red()
{
    return float(m_r);
}

float GlColor::Green()
{
    return float(m_g);
}

float GlColor::Blue()
{
    return float(m_b);
}

float GlColor::Alpha()
{
    return float(m_a);
}

void GlColor::getHsv(double &h, double &s, double &v)
{
    double      min, max, delta;
    min = m_r < m_g ? m_r : m_g;
    min = min  < m_b ? min  : m_b;
    max = m_r > m_g ? m_r : m_g;
    max = max  > m_b ? max  : m_b;
    v = max;                                // v
    delta = max - min;

    if(delta < 0.00001)
    {
        s = 0;
        h = 0; // undefined, maybe nan?
        return;
    }

    if(max > 0.0)     // NOTE: if Max is == 0, this divide would cause a crash
    {
        s = (delta / max);                  // s
    }
    else
    {
        // if max is 0, then r = g = b = 0
        // s = 0, v is undefined
        s = 0.0;
        h = static_cast<double>(NAN);                            // its now undefined
        return;
    }

    if(m_r >= max)                             // > is bogus, just keeps compilor happy
        h = (m_g - m_b) / delta;          // between yellow & magenta
    else if(m_g >= max)
        h = 2.0 + (m_b - m_r) / delta;    // between cyan & yellow
    else
        h = 4.0 + (m_r - m_g) / delta;    // between magenta & cyan

    h *= 60.0;                              // degrees

    if(h < 0.0)
        h += 360.0;
}
