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


SimpleHighlighter::SimpleHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{}

void SimpleHighlighter::clearRules()
{
    m_highlightingRules.clear();
}

void SimpleHighlighter::addRule(const Q_QRegExp &pattern, const QTextCharFormat &format)
{
    m_highlightingRules.push_back({pattern, format});
}

void SimpleHighlighter::highlightBlock(const QString &text)
{
    for(const HighlightingRule &rule : m_highlightingRules)
    {
        Q_QRegExp expression(rule.pattern);

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        int index = expression.indexIn(text);
        while(index >= 0)
        {
            int length = expression.matchedLength();
            if(expression.captureCount() == 0)
                setFormat(index, length, rule.format);
            else for(int i = 1; i <= expression.captureCount(); ++i)
            {
                QString c = expression.cap(i);
                setFormat(expression.pos(i), c.size(), rule.format);
            }

            index = expression.indexIn(text, index + length);
        }
#else
        if(expression.captureCount() == 0)
        {
            QRegularExpressionMatchIterator index = expression.globalMatch(text);
            while(index.hasNext())
            {
                QRegularExpressionMatch match = index.next();
                setFormat(match.capturedStart(), match.capturedLength(), rule.format);
            }
        }
        else for(int i = 1; i <= expression.captureCount(); ++i)
        {
            QRegularExpressionMatchIterator index = expression.globalMatch(text);
            while(index.hasNext())
            {
                QRegularExpressionMatch match = index.next();
                setFormat(match.capturedStart(i), match.capturedLength(i), rule.format);
            }
        }
#endif
    }
}
