/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2019 Vitaly Novichkov <admin@wohlnet.ru>
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

#pragma once
#ifndef MWDOCK_BASE_H
#define MWDOCK_BASE_H

class MainWindow;
class QObject;

class MWDock_Base
{
public:
    explicit MWDock_Base(QObject *parent = nullptr);
    virtual ~MWDock_Base();
    MainWindow *mw();

    bool m_lastVisibilityState = false;

protected:
    virtual void construct(MainWindow *ParentMW);

private:
    void setParentMW(MainWindow *ParentMW);
    MainWindow *m_mw;
};

#endif // MWDOCK_BASE_H
