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

#ifndef MOONDUST_FILE_KEEPER_H
#define MOONDUST_FILE_KEEPER_H

#include <QFile>

/**
 * Renames file into temp name and, optionally, can remove it
 */
class FileKeeper
{
    //! Original file path
    QString m_origPath;
    //! New temp file path
    QString m_tempPath;
    //! Backup file path, creates after swap of original file with a temp file
    QString m_backupPath;
public:
    /**
     * \brief Keeps file about new temp file
     * @param path Path to original file
     */
    explicit FileKeeper(const QString &path);
    virtual ~FileKeeper();

    QString tempPath();

    /**
     * \brief Delete temp file
     */
    void remove();
    /**
     * \brief Restore file back
     */
    void restore();

    /**
     * @brief Remove all bak files immediately (call this when normally quit the Editor)
     */
    static void removeAllBaks();
};


#endif //MOONDUST_FILE_KEEPER_H
