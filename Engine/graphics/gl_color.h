/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2021 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef GLCOLOR_H
#define GLCOLOR_H

#include <SDL2/SDL_opengl.h>
#include <string>

class GlColor
{
public:
    GlColor();
    GlColor(const GlColor &clr);
    GlColor &operator=(const GlColor &clr);
    GlColor(GLuint rgba);
    GlColor(double _r, double _g, double _b);
    GlColor(double _r, double _g, double _b, double _a);
    GlColor(std::string rgba);
    ~GlColor();
    void setRgb(GLuint rgb);
    void setRgba(GLuint rgba);
    void setRgba(std::string rgba);
    void setRgba(double _r, double _g, double _b, double _a);
    void setHsv(double _h, double _s, double _v);
    void setHsva(double _h, double _s, double _v, double _a);
    /**
     * @brief Set red from 0.0 to 1.0
     * @param _r
     */
    void setRed(double _r);
    /**
     * @brief Set green from 0.0 to 1.0
     * @param _g
     */
    void setGreen(double _g);
    /**
     * @brief Set blue from from 0.0 to 1.0
     * @param _b
     */
    void setBlue(double _b);
    /**
     * @brief Set alpha from 0.0 to 1.0
     * @param _a
     */
    void setAlpha(double _a);
    /**
     * @brief Set red from 0.0 to 255.0
     * @param _r
     */
    void setRedI(double _r);
    /**
     * @brief Set green from 0.0 to 255.0
     * @param _g
     */
    void setGreenI(double _g);
    /**
     * @brief Set blue from 0.0 to 255.0
     * @param _b
     */
    void setBlueI(double _b);
    /**
     * @brief Set alpha from 0.0 to 255.0
     * @param _a
     */
    void setAlphaI(double _a);

    float Red();
    float Green();
    float Blue();
    float Alpha();
    void getHsv(double &h, double &s, double &v);

private:
    double m_r;
    double m_g;
    double m_b;
    double m_a;
};

#endif // GLCOLOR_H
