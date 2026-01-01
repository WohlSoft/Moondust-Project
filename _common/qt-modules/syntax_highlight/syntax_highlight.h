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

#include <QSyntaxHighlighter>
#include "../compat/pge_qt_compat.h"
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#   include <QRegExp>
#else
#   include <QRegularExpression>
#endif

class SimpleHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    SimpleHighlighter(QTextDocument *parent = 0);

    void clearRules();
    void addRule(const Q_QRegExp &pattern, const QTextCharFormat &format);

protected:
    void highlightBlock(const QString &text) Q_DECL_OVERRIDE;

protected:
    struct HighlightingRule
    {
        Q_QRegExp pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> m_highlightingRules;
};
