/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2023 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This software is licensed under a dual license system (MIT or GPL version 3 or later).
 * This means you are free to choose with which of both licenses (MIT or GPL version 3 or later)
 * you want to use this software.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can see text of MIT license in the LICENSE.mit file you can see in Engine folder,
 * or see https://mit-license.org/.
 *
 * You can see text of GPLv3 license in the LICENSE.gpl3 file you can see in Engine folder,
 * or see <http://www.gnu.org/licenses/>.
 */

#include "gl_color.h"

#include <cmath>
#include <cstdlib>
#include <unordered_map>
#include <algorithm>
#include <Logger/logger.h>

struct gl_RGBA
{
    double R;
    double G;
    double B;
    double A;
};
typedef std::unordered_map<std::string, gl_RGBA> ColorNamesHash;
static const ColorNamesHash s_ColorNames =
{
    {"transparent", {0.0, 0.0, 0.0, 0.0}},
    {"alphablack",  {0.0, 0.0, 0.0, 0.0}},

    {"white",       {1.0, 1.0, 1.0, 1.0}},
    {"black",       {0.0, 0.0, 0.0, 1.0}},
    {"red",         {1.0, 0.0, 0.0, 1.0}},
    {"green",       {0.0, 1.0, 0.0, 1.0}},
    {"blue",        {0.0, 0.0, 1.0, 1.0}},
    {"cyan",        {0.0, 1.0, 1.0, 1.0}},
    {"magenta",     {1.0, 0.0, 1.0, 1.0}},

    {"yellow",      {1.0, 1.0, 0.0, 1.0}},
    {"gray",        {0.625, 0.625, 0.625, 1.0}},
    {"grey",        {1.0, 0.95, 0.4, 1.0}},
    {"pink",        {1.0, 0.45, 0.67, 1.0}},
    {"canary",      {1.0, 0.45, 0.67, 1.0}},
    {"purple",      {0.67, 0.4, 0.67, 1.0}},
    {"orange",      {1.0, 0.55, 0.33, 1.0}},
    {"teal",        {0.0, 0.67, 0.6,  1.0}},
    {"maroon",      {0.45, 0.0, 0.0,  1.0}},
    {"brown",       {0.5, 0.3, 0.0,  1.0}},

    {"darkred",     {0.5, 0.0, 0.0, 1.0}},
    {"darkcyan",    {0.0, 0.5, 0.5, 1.0}},
    {"darkmagenta", {0.5, 0.0, 0.5, 1.0}},
    {"darkyellow",  {0.5, 0.5, 0.0, 1.0}},
    {"darkgray",    {0.5, 0.5, 0.5, 1.0}},
    {"darkgrey",    {0.5, 0.5, 0.5, 1.0}},
    {"darkblue",    {0.0, 0.0, 0.5, 1.0}},
    {"darkgreen",   {0.0, 0.5, 0.0, 1.0}},
    {"darkbrown",   {0.3, 0.25, 0.25, 1.0}},

    {"lightred",    {1.0, 0.5, 0.5, 1.0}},
    {"lightcyan",   {0.5, 1.0, 1.0, 1.0}},
    {"lightmagenta", {1.0, 0.5, 1.0, 1.0}},
    {"lightyellow", {1.0, 1.0, 0.5, 1.0}},
    {"lightgray",   {0.75, 0.75, 0.75, 1.0}},
    {"lightgrey",   {0.75, 0.75, 0.75, 1.0}},
    {"lightblue",   {0.2, 0.8, 1.0, 1.0}},
    {"lightgreen",  {0.5, 0.8, 0.6, 1.0}},
    {"lightbrown",  {0.5, 0.3, 0.0, 1.0}}
};

GlColor::GlColor()
{
    m_r = 1.0;
    m_g = 1.0;
    m_b = 1.0;
    m_a = 1.0;
}

GlColor::GlColor(const GlColor &clr)
{
    operator=(clr);
}

GlColor &GlColor::operator=(const GlColor &clr)
{
    m_r = clr.m_r;
    m_g = clr.m_g;
    m_b = clr.m_b;
    m_a = clr.m_a;
    return *this;
}

GlColor::GlColor(GLuint rgba)
{
    setRgba(rgba);
}

GlColor::GlColor(double _r, double _g, double _b)
{
    m_r = _r;
    m_g = _g;
    m_b = _b;
    m_a = 1.0;
}

GlColor::GlColor(double _r, double _g, double _b, double _a)
{
    m_r = _r;
    m_g = _g;
    m_b = _b;
    m_a = _a;
}

GlColor::GlColor(std::string rgba)
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
    m_a = 1.0;
}

void GlColor::setRgba(GLuint rgba)
{
    m_r = static_cast<double>((rgba & 0xFF000000) >> 24) / 255.0;
    m_g = static_cast<double>((rgba & 0x00FF0000) >> 16) / 255.0;
    m_b = static_cast<double>((rgba & 0x0000FF00) >> 8) / 255.0;
    m_a = static_cast<double>((rgba & 0x000000FF)) / 255.0;
}

void GlColor::setRgba(std::string rgba)
{
    if(rgba.empty())
    {
        setRgba(0.0, 0.0, 0.0, 1.0);
        return;
    }

    std::transform(rgba.begin(), rgba.end(), rgba.begin(), ::tolower);

    try
    {
        if(rgba[0] == '#')
        {
            m_a = 1.0;

            if(rgba.size() == 4)
            {
                // 0 123
                m_r = static_cast<double>(std::strtol(rgba.substr(1, 1).c_str(), nullptr, 16)) / 15.0;
                m_g = static_cast<double>(std::strtol(rgba.substr(2, 1).c_str(), nullptr, 16)) / 15.0;
                m_b = static_cast<double>(std::strtol(rgba.substr(3, 1).c_str(), nullptr, 16)) / 15.0;
                return;
            }

            if(rgba.size() == 9)
            {
                // 0 12 34 56 78
                m_a = static_cast<double>(std::strtol(rgba.substr(7, 2).c_str(), nullptr, 16)) / 255.0;
            }

            if((rgba.size() == 7) || (rgba.size() == 9))
            {
                // 0 12 34 56
                m_r = static_cast<double>(std::strtol(rgba.substr(1, 2).c_str(), nullptr, 16)) / 255.0;
                m_g = static_cast<double>(std::strtol(rgba.substr(3, 2).c_str(), nullptr, 16)) / 255.0;
                m_b = static_cast<double>(std::strtol(rgba.substr(5, 2).c_str(), nullptr, 16)) / 255.0;
                return;
            }

            setRgba(0.0, 0.0, 0.0, 1.0);
            return;
        }
        else if((rgba.size() > 2) && (rgba[0] == '0') && (rgba[1] == 'x'))
        {
            m_a = 1.0;

            if(rgba.size() == 10)
            {
                // 01 23 45 67 89
                m_a = static_cast<double>(std::strtol(rgba.substr(8, 2).c_str(), nullptr, 16)) / 255.0;
            }

            if((rgba.size() == 8) || (rgba.size() == 10))
            {
                // 01 23 45 67
                m_r = static_cast<double>(std::strtol(rgba.substr(2, 2).c_str(), nullptr, 16)) / 255.0;
                m_g = static_cast<double>(std::strtol(rgba.substr(4, 2).c_str(), nullptr, 16)) / 255.0;
                m_b = static_cast<double>(std::strtol(rgba.substr(6, 2).c_str(), nullptr, 16)) / 255.0;
                return;
            }

            setRgba(0.0, 0.0, 0.0, 1.0);
            return;
        }
        else
        {
            ColorNamesHash::const_iterator color = s_ColorNames.find(rgba);
            if(color == s_ColorNames.end())
            {
                pLogWarning("Unknown color name \"%s\"", rgba.c_str());
                setRgba(0.0, 0.0, 0.0, 1.0);
                return;
            }
            else
            {
                const gl_RGBA &c = color->second;
                m_r = c.R;
                m_g = c.G;
                m_b = c.B;
                m_a = c.A;
                return;
            }
        }
    }
    catch(std::exception &e)
    {
        pLogWarning("Invalid color code \"%s\" because of %s", rgba.c_str(), e.what());
    }
    catch(...)
    {
        pLogWarning("Invalid color code \"%s\"", rgba.c_str());
    }
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
