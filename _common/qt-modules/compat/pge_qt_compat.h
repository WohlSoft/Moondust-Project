/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2023 Vitaly Novichkov <admin@wohlnet.ru>
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
#ifndef COMPAT_H
#define COMPAT_H

#include <QtGlobal>
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
#   include <QRandomGenerator>
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#   include <QRegularExpression>
#endif

/** This is a small module that helps to maintain compatibility between
 *  different versions of Qt.
 *
 *  Use these macros instead of direct methods if you want to avoid warnings
 *  or even errors when building on different Qt versions.
 */


// QString::SplitBehaviour
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
#   define QSTRING_SPLIT_BEHAVIOUR(x) QString::x
#else
#   define QSTRING_SPLIT_BEHAVIOUR(x) Qt::x
#endif

// QStextStream::endl
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
#   define QT_ENDL Qt::endl;
#else
#   define QT_ENDL endl;
#endif

// QFontMetrics::width
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
#   define Q_FontMetricWidthArg(x) horizontalAdvance(x)
#else
#   define Q_FontMetricWidthArg(x) width(x)
#endif

// qrand()
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
#   define Q_RANDNEW QRandomGenerator::system()->generate
#else
#   define Q_RANDNEW qrand
#endif

// QList::swap(x1, x2)
#if QT_VERSION >= QT_VERSION_CHECK(5, 13, 0)
#   define Q_ListSwap(x1, x2) swapItemsAt(x1, x2)
#else
#   define Q_ListSwap(x1, x2) swap(x1, x2)
#endif


// QEvent::localPos
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#   define Q_EventLocalPos position
#else
#   define Q_EventLocalPos localPos
#endif

// QEvent::windowPos
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#   define Q_EventScreenPos globalPosition
#else
#   define Q_EventScreenPos screenPos
#endif

// QEvent::screenPos
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#   define Q_EventWindowPos scenePosition
#else
#   define Q_EventWindowPos windowPos
#endif


// QEvent::pos
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#   define Q_EventPos position().toPoint
#else
#   define Q_EventPos pos
#endif

// QEvent::globalPos
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#   define Q_EventGlobalPos globalPosition().toPoint
#else
#   define Q_EventGlobalPos globalPos
#endif

// QRegExp
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#   define  Q_QRegExp QRegularExpression
#   define  Q_QRegExpMatch(x) match(x).hasMatch()
#else
#   define  Q_QRegExp QRegExp
#   define  Q_QRegExpMatch(x) exactMatch(x)
#endif

// QRegExpValidator
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#   define  Q_QRegExpValidator QRegularExpressionValidator
#else
#   define  Q_QRegExpValidator QRegExpValidator
#endif


#endif // COMPAT_H
