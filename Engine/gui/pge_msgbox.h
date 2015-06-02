/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2015 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PGE_MSGBOX_H
#define PGE_MSGBOX_H

#include "pge_boxbase.h"
#include "../scenes/scene.h"

#undef main
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_types.h>
#undef main

#include <QString>
#include <common_features/rect.h>
#include <common_features/point.h>
#include <common_features/pointf.h>
#include <common_features/size.h>
#include <common_features/sizef.h>
#include <QColor>

class PGE_MsgBox : public PGE_BoxBase
{
public:
    enum msgType
    {
        msg_info=0,
        msg_info_light,
        msg_warn,
        msg_error,
        msg_fatal
    };

    PGE_MsgBox();
    PGE_MsgBox(Scene * _parentScene=NULL, QString msg="Message box is works!",
               msgType _type=msg_info, PGE_Point boxCenterPos=PGE_Point(-1,-1), PGE_SizeF boxSize=PGE_SizeF(0,0), float _padding=20, QString texture="");
    ~PGE_MsgBox();

    void setBoxSize(float _Width, float _Height, float _padding);
    void exec();

    void buildBox(bool centered=false);

    static void info(QString msg);
    //static void info(std::string msg);
    static void warn(QString msg);
    //static void warn(std::string msg);
    static void error(QString msg);
    //static void error(std::string msg);
    static void fatal(QString msg);
    //static void fatal(std::string msg);

private:
    void construct(QString msg="Message box is works!",
                    msgType _type=msg_info, bool autosize=true, PGE_SizeF boxSize=PGE_SizeF(0,0), PGE_Point pos=PGE_Point(-1,-1), float _padding=20, QString texture="");
    msgType type;
    PGE_Rect _sizeRect;
    QString message;
    GLuint textTexture;
    float width;
    float height;
    float padding;
    QColor bg_color;
    void updateControllers();
};

#endif // PGE_MSGBOX_H
