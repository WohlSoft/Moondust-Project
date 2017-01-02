/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2017 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <QString>
#include <QVector>
#include <QList>
#include <string>

#include <luabind/luabind.hpp>
#include <lua_inclues/lua.hpp>

#ifdef Q_CC_GNU
#define gcc_force_inline __attribute__((always_inline, gnu_inline))
#else
#define gcc_force_inline
#endif

namespace util
{
    QString filePath(QString s);
    bool strempty(const char *str);

    QString resolveRelativeOrAbsolute(const QString &path, const QStringList &relativeLookup);

    void CSV2IntArr(QString source, QList<int> &dest);
    void CSV2IntArr(QString source, QVector<int> &dest);
    void CSV2DoubleArr(QString source, QList<double> &dest);
    void CSV2DoubleArr(QString source, QVector<double> &dest);
    void base64_encode(std::string &ret, const unsigned char *bytes_to_encode, size_t in_len);
    void base64_decode(std::string &ret, std::string const &encoded_string);

};

namespace varadic_util
{
    template<int ...>
    struct seq { };

    template<int N, int ...S>
    struct gens : gens < N - 1, N - 1, S... > { };

    template<int ...S>
    struct gens<0, S...>
    {
        typedef seq<S...> type;
    };
}



namespace luabind_utils
{
    template<typename T>
    static inline gcc_force_inline QList<T> convArrayTo(luabind::object &obj)
    {
        QList<T> container;

        for(luabind::iterator it(obj), end; it != end; ++it)
        {
            try
            {
                container << luabind::object_cast<T>(*it);
            }
            catch(luabind::cast_failed & /*e*/) { }
        }

        return container;
    }
}

namespace charsets_utils
{
    /*!
     * \brief returns length of UTF8 string line
     * \param Input 8-bit string in UTF8 codepage
     * \return number of characters (not a bytes!)
     */
    size_t utf8len(const char *s);
    int UTF8Str_To_WStr(std::wstring &dest, const std::string  &source);
    int WStr_To_UTF8Str(std::string  &dest, const std::wstring &source);
}

#endif // UTIL_H
