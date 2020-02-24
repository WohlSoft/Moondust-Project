/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef PGE_BOXBASE_H
#define PGE_BOXBASE_H

#include "../scenes/scene.h"
#include <string>

#include <graphics/gl_color.h>
#include <common_features/rect.h>
#include <common_features/rectf.h>
#include <common_features/pge_texture.h>
#include <controls/control_keys.h>

class Controller;

///
/// \brief The PGE_BoxBase class
///
/// This class provides the basis of message boxes, menus, questions, etc.
class PGE_BoxBase
{
public:
    enum msgType
    {
        msg_info = 0,
        msg_info_light,
        msg_warn,
        msg_error,
        msg_fatal
    };

    explicit PGE_BoxBase(Scene *parentScene = nullptr);
    PGE_BoxBase(const PGE_BoxBase &bb);
    virtual ~PGE_BoxBase();

    virtual void setParentScene(Scene *_parentScene);

    void setType(msgType type);
    void setBoxSize(double width, double height, double padding);

    virtual void processLoader(double tickDelay);
    virtual void processBox(double tickDelay);
    virtual void processUnLoader(double tickDelay);

    virtual void restart();
    void nextPage();
    bool isRunning();

    virtual void exec();
    virtual void update(double tickTime);
    virtual void render();

    /**************Fader**************/
    double  m_faderOpacity;
    double  m_targetOpacity;
    double  m_fadeStep;
    int     m_fadeSpeed;
    double  m_manualTicks;
    void    setFade(int speed, double target, double step);
    bool    tickFader(double ticks);
    void    fadeStep();
    /**************Fader**************/

    void loadTexture(std::string path);

    void updateTickValue();

protected:
    Scene * m_parentScene;
    void construct(Scene *parentScene = nullptr);
    enum BoxPages
    {
        PageStart = 0,
        PageLoading,
        PageRunning,
        PageUnLoading,
        PageClose,
    };
    int     m_page = PageStart;
    bool    m_running = false;

    int     m_fontID = -1;
    GlColor m_fontRgba;
    int     m_borderWidth = 32;

    GlColor m_bgColor;

    msgType m_type = msg_info;
    PGE_Rect m_sizeRect;

    double  m_width = 32.0;
    double  m_height = 32.0;
    double  m_padding = 2.0;

    double m_uTickf = 14.0;
    int    m_uTick = 14;

    void drawTexture(int left, int top, int right, int bottom, int border=32, float opacity=1.0f);
    void drawTexture(PGE_Rect _rect, int border=32, float opacity=1.0f);
    bool m_textureUsed;
    void drawPiece(PGE_RectF target, PGE_RectF block, PGE_RectF texture);
    PGE_Texture m_styleTexture;

    //! Controller of a first player
    Controller *m_ctrl1 = nullptr;
    //! Controller of a second player
    Controller *m_ctrl2 = nullptr;

    //! Control keys state
    controller_keys m_keys;

    void initControllers();
    void updateControllers();
};

#endif // PGE_BOXBASE_H
