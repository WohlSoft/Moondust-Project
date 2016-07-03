/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2016 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "mwdock_base.h"
#include <mainwindow.h>

MWDock_Base::MWDock_Base(QObject *parent)//: QObject(parent)
{
    if(parent)
    {
        if(strcmp(parent->metaObject()->className(), "MainWindow")==0)
            _mw = qobject_cast<MainWindow *>(parent);
        else
            _mw = nullptr;
    }
}

MWDock_Base::~MWDock_Base()
{}

MainWindow *MWDock_Base::mw()
{
    return _mw;
}

//void MWDock_Base::re_translate()
//{}

void MWDock_Base::construct(MainWindow *ParentMW)
{
    setParentMW(ParentMW);
}

void MWDock_Base::setParentMW(MainWindow *ParentMW)
{
    _mw = ParentMW;
}
