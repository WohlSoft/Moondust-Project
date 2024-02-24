/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2024 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "growing_text_edit.h"
#include <QFontMetrics>
#include <QtDebug>


void GrowingTextEdit::init()
{
    QObject::connect(this->document()->documentLayout(), &QAbstractTextDocumentLayout::documentSizeChanged,
                     this, &GrowingTextEdit::sizeChanged);
}

GrowingTextEdit::GrowingTextEdit(QWidget *parent) :
    QPlainTextEdit(parent)
{
    init();
}

GrowingTextEdit::GrowingTextEdit(const QString &text, QWidget *parent) :
    QPlainTextEdit(text, parent)
{
    init();
}

GrowingTextEdit::~GrowingTextEdit()
{}

void GrowingTextEdit::sizeChanged(const QSizeF &newSize)
{
    QFontMetrics m(this->font());
    int docHeight = newSize.toSize().height();

    if(m_heightMin < docHeight && docHeight < m_heightMax)
        setMinimumHeight((docHeight * m.height()) + 10);
}
