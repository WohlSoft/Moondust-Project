/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2015 Vitaly Novichkov <admin@wohlnet.ru>
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
#ifndef BASICCOMMAND_H
#define BASICCOMMAND_H

#include <QObject>

#include "../../defines.h"

class BasicCommand : public QObject
{
    Q_OBJECT
public:
    explicit BasicCommand(QObject *parent = 0);

    virtual QString compileSegment(Script::CompilerType compiler, int tabOffset);
    virtual bool supportCompiler(Script::CompilerType compiler);

    ~BasicCommand();

    QString marker() const;
    void setMarker(const QString &marker);

protected:
    QString mkLine(int tab);

signals:

public slots:

private:
    //additional info (i.e. for the additional Settings dialog)
    QString m_marker;

};

#endif // BASICCOMMAND_H
