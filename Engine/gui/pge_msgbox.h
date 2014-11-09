/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <QString>

class PGE_MsgBox : public PGE_BoxBase
{
public:
    enum msgType
    {
        msg_info=0,
        msg_warn,
        msg_error,
        msg_fatal
    };

    PGE_MsgBox();
    PGE_MsgBox(Scene * _parentScene=NULL, QString msg="Message box is works!", msgType _type=msg_info);
    ~PGE_MsgBox();


    void exec();

private:
    Scene * parentScene;
    msgType type;
    QString message;
};

#endif // PGE_MSGBOX_H
