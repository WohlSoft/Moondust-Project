/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2021 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <QDir>

#include <tools/tilesets/tilesetgroupeditor.h>

#include "data_configs.h"

void DataConfig::loadTilesets()
{
    main_tilesets.clear();
    main_tilesets_grp.clear();
    main_tileset_categogies.clear();

    QString tilesetDirPath = config_dir + "tilesets/";
    QString tilesetGrpDirPath = config_dir + "group_tilesets/";
    QStringList filters;

    QSet<QString> tilesetCategoryNames;
    QSettings categories(tilesetGrpDirPath + "/categories.ini", QSettings::IniFormat);

    if(QDir(tilesetDirPath).exists())
    {
        emit progressPartNumber(10);
        emit progressMax(100);
        emit progressValue(0);
        emit progressTitle(QObject::tr("Loading Tilesets..."));

        filters.clear();
        filters << "*.tileset.ini";
        QDir tilesetDir(tilesetDirPath);
        tilesetDir.setSorting(QDir::Name);
        tilesetDir.setNameFilters(filters);
        QStringList files = tilesetDir.entryList(filters);

        emit progressMax(files.size());
        main_tilesets.reserve(files.size());
        for(int i = 0; i < files.size(); i++)
        {
            emit progressValue(i);
            SimpleTileset xxx;
            if(tileset::OpenSimpleTileset(tilesetDirPath + files[i], xxx))
            {
                main_tilesets.push_back(xxx);
            }
        }
    }

    if(QDir(tilesetGrpDirPath).exists())
    {
        emit progressPartNumber(11);
        emit progressMax(100);
        emit progressValue(0);
        emit progressTitle(QObject::tr("Loading Tileset groups..."));

        filters.clear();
        filters << "*.tsgrp.ini";
        QDir tilesetDir(tilesetGrpDirPath);
        tilesetDir.setSorting(QDir::Name);
        tilesetDir.setNameFilters(filters);

        QStringList files = tilesetDir.entryList(filters);
        emit progressMax(files.size());
        main_tilesets_grp.reserve(files.size());
        for(int i = 0; i < files.size(); i++)
        {
            emit progressValue(i);
            SimpleTilesetGroup xxx;
            if(TilesetGroupEditor::OpenSimpleTilesetGroup(tilesetGrpDirPath + files[i], xxx))
            {
                tilesetCategoryNames.insert(xxx.groupCat);
                main_tilesets_grp.push_back(xxx);
            }
        }
    }

    // Sort all groups in the list
    std::sort(main_tilesets_grp.begin(), main_tilesets_grp.end());

    emit progressTitle(QObject::tr("Initializing tileset categories..."));
    main_tileset_categogies.reserve(tilesetCategoryNames.size());
    for(const QString &cat : tilesetCategoryNames)
    {
        SimpleTilesetCachedCategory category;
        categories.beginGroup(TilesetGroupEditor::categoryName(cat));
        category.name = cat;
        category.weight = categories.value("weight", -1).toInt();
        categories.endGroup();
        main_tileset_categogies.push_back(category);
    }

    // Sort all categories in the list
    std::sort(main_tileset_categogies.begin(), main_tileset_categogies.end());
}
