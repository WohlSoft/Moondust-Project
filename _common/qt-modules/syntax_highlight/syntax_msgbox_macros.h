/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2026 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "syntax_highlight.h"
#include "../compat/pge_qt_compat.h"


/**
 * @brief Creates the copy of the Simple Syntax Highligter filled by rules of message box macros syntax
 * @param parent The `->document()` pointer retrieved from the QTextEdit / QPlainTextEdit instance
 * @return Syntax highlighet ready for a work
 */

static inline SimpleHighlighter *s_makeMsgBoxMacrosHighlighter(QTextDocument *parent = 0)
{
    SimpleHighlighter* highLighter = new SimpleHighlighter(parent);

    QTextCharFormat c_fmt;
    c_fmt.setForeground(Qt::blue);
    c_fmt.setFontWeight(QFont::Bold);
    highLighter->addRule(Q_QRegExp("^# *if"), c_fmt);
    highLighter->addRule(Q_QRegExp("^# *if_iw"), c_fmt);
    highLighter->addRule(Q_QRegExp("^# *if_in"), c_fmt);
    highLighter->addRule(Q_QRegExp("^# *elif"), c_fmt);
    highLighter->addRule(Q_QRegExp("^# *else"), c_fmt);
    highLighter->addRule(Q_QRegExp("^# *endif$"), c_fmt);
    highLighter->addRule(Q_QRegExp("^# *endif_iw$"), c_fmt);
    highLighter->addRule(Q_QRegExp("^# *endif_in$"), c_fmt);

    c_fmt.setForeground(Qt::cyan);
    c_fmt.setFontWeight(QFont::Bold);
    highLighter->addRule(Q_QRegExp("^# *if +(\\w+\\(.*\\))$"), c_fmt);
    highLighter->addRule(Q_QRegExp("^# *if_iw +(\\w+\\(.*\\))$"), c_fmt);
    highLighter->addRule(Q_QRegExp("^# *if_in +(\\w+\\(.*\\))$"), c_fmt);
    highLighter->addRule(Q_QRegExp("^# *elif +(\\w+\\(.*\\))$"), c_fmt);

    c_fmt.setForeground(Qt::magenta);
    c_fmt.setFontWeight(QFont::Normal);
    highLighter->addRule(Q_QRegExp("^# *if +\\w+\\((.*)\\)$"), c_fmt);
    highLighter->addRule(Q_QRegExp("^# *if_iw +\\w+\\((.*)\\)$"), c_fmt);
    highLighter->addRule(Q_QRegExp("^# *if_in +\\w+\\((.*)\\)$"), c_fmt);
    highLighter->addRule(Q_QRegExp("^# *elif +\\w+\\((.*)\\)$"), c_fmt);

    return highLighter;
}
