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

/*!
 * \file file_strlist.h
 *
 * \brief Contains defition of a FileStringList class - a string container
 *        which automatically splits lines by line feeds
 */

#ifndef FILE_STRLIST_H
#define FILE_STRLIST_H

#ifdef PGE_FILES_QT
#include <QString>
#include <QStringList>
#include <QObject>
#define PGESTRING QString
#define PGESTRINGList QStringList
#else
#include <vector>
#include <string>
#define PGESTRING std::string
#define PGESTRINGList std::vector<std::string >
#endif

/*!
 * \brief Provides string container automatically splited from entire file data
 */
#ifdef PGE_FILES_QT
class FileStringList:public QObject
{
    Q_OBJECT
#else
class FileStringList
{
#endif

public:
    /*!
     * \brief Constructor
     */
    FileStringList();

    /*!
     * \brief Constructor with pre-set data
     * \param fileData file data which will be splited by line-feeds
     */
    FileStringList(PGESTRING fileData);

    /*!
     * Destructor
     */
    ~FileStringList();

    /*!
     * \brief Changes filedata and rebuilds list of lines
     * \param fileData file data which will be splited by line-feeds
     */
    void addData(PGESTRING fileData);

    /*!
     * \brief Returns current line contents and incements internal line counter
     * \return contents of current line
     */
    PGESTRING readLine();

    /*!
     * \brief Are all lines was gotten?
     * \return true if internal line counter is equal or more than total number of lines
     */
    bool isEOF();

    /*!
     * \brief Are all lines was gotten?
     * \return true if internal line counter is equal or more than total number of lines
     */
    bool atEnd();
private:
    /*!
     * \brief Contains splited list of string lines
     */
    PGESTRINGList buffer;

    /*!
     * \brief Internal counter of gotten lines
     */
    long lineID;
};

#endif // FILE_STRLIST_H

