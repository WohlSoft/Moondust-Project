/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2025 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef GROWINGTEXTEDIT_H
#define GROWINGTEXTEDIT_H

#include <QPlainTextEdit>

class GrowingTextEdit : public QPlainTextEdit
{
    Q_OBJECT

    void init();

    int m_heightMin = 0;
    int m_heightMax = 65000;

public:
    explicit GrowingTextEdit(QWidget *parent = nullptr);
    explicit GrowingTextEdit(const QString &text, QWidget *parent = nullptr);
    virtual ~GrowingTextEdit();

private slots:
    void sizeChanged(const QSizeF &newSize);


};

#endif // GROWINGTEXTEDIT_H
