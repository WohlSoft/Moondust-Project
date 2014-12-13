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

#ifndef AUTOCODECOMPILER_H
#define AUTOCODECOMPILER_H

#include "basiccompiler.h"

class AutocodeCompiler : public BasicCompiler
{
    Q_OBJECT
public:
    explicit AutocodeCompiler(QList<EventCommand *> commands, QObject *parent = 0);
    ~AutocodeCompiler();

    virtual QString compileCode();

signals:

public slots:

};

#endif // AUTOCODECOMPILER_H
