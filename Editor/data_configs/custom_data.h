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

#pragma once
#ifndef CUSTOM_DATA_H
#define CUSTOM_DATA_H

#include <QPixmap>
#include <QImage>
#include <QList>

#include <PGE_File_Formats/npc_filedata.h>
#include <data_configs/obj_npc.h>
#include <Utils/dir_list_ci_qt.h>

struct UserIMGs
{
    QPixmap image;
    QImage mask;
    unsigned long id;
};

struct UserNPCs
{
    bool withImg;
    QPixmap image;
    QImage mask;
    unsigned long id;

    bool withTxt;
    NPCConfigFile sets;
    obj_npc merged;
};

struct UserBGs
{
    QPixmap image;
    QPixmap second_image;
    unsigned long id;
    unsigned int q;//0 - only first; 1 - only second; 2 - fitst and seconf
};

///
/// \brief The CustomDirManager class This is a capturer of custom files from level/world custom directories
///
class CustomDirManager
{
public:
    CustomDirManager();
    CustomDirManager(QString path, QString name);
    ///
    /// \brief getCustomFile Get custom file path if exist.
    /// \param name Target file name which need to found
    /// \return empty string if not exist
    ///
    QString getCustomFile(QString name, bool ignoreDefaultDirectory=false);
    QStringList getCustomFiles(QString nameBase, QStringList extensions, bool ignoreDefaultDirectory = false);
    void setCustomDirs(QString path, QString name);
    void setDefaultDir(QString dPath);
    void addExtraDir(QString dPath);
    void clearExtraDirs();
    QString findFileInExtraDirs(QString fPath);
    void createDirIfNotExsist();
    void import(QStringList &files, bool local = true);

private:
    DirListCIQt dirCiEpisode;
    DirListCIQt dirCiCustom;
    QString dirEpisode;
    QString dirCustom;
    QList<QString> dirsExtra;
    QString defaultDirectory;
};



#endif // CUSTOM_DATA_H
