/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2015 Vitaly Novichkov <admin@wohlnet.ru>
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


#ifndef UTIL_H
#define UTIL_H

#include <qlineedit.h>
#include <qlistwidget.h>
#include <qcombobox.h>
#include <QListWidget>
#include <qtablewidget.h>

#include <luabind/luabind.hpp>
#include <lua_inclues/lua.hpp>

#ifdef Q_CC_GNU
#define gcc_force_inline __attribute__((always_inline, gnu_inline))
#else
#define gcc_force_inline
#endif

class util
{
public:
    static void updateFilter(QLineEdit* searchEdit, QListWidget* itemList, QComboBox* typeBox);
    static void memclear(QListWidget* wid);
    static void memclear(QTableWidget* wid);
    static void clearLayoutItems(QLayout* layout);
    static bool contains(const QComboBox* b, const QString &s);
    static QString filePath(QString s);
    static bool strempty(const char* str);
};

namespace varadic_util
{
    template<int ...>
    struct seq { };

    template<int N, int ...S>
    struct gens : gens<N-1, N-1, S...> { };

    template<int ...S>
    struct gens<0, S...> {
        typedef seq<S...> type;
    };
}



namespace luabind_utils {
    template<typename T>
    static inline gcc_force_inline QList<T> convArrayTo(luabind::object& obj){
        QList<T> container;
        for (luabind::iterator it(obj), end; it != end; ++it)
        {
            try{
                container << luabind::object_cast<T>(*it);
            } catch (luabind::cast_failed& e) { }
        }
        return container;
    }
}

#endif // UTIL_H
