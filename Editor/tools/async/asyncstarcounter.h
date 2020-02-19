/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
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
#ifndef ASYNCSTARCOUNTER_H
#define ASYNCSTARCOUNTER_H

#include <QObject>
#include <QMutex>
#include <QMutexLocker>
#include <QProgressDialog>
#include <data_configs/data_configs.h>
#include <PGE_File_Formats/file_formats.h>

class AsyncStarCounter : public QObject
{
    Q_OBJECT
public:
    explicit AsyncStarCounter(const QString& dir, const QVector<WorldLevelTile>& levels, const QString& introLevel, dataconfigs* configs, QObject *parent = 0);

    void startAndShowProgress();

    bool isCancelled() const;
    int countedStars() const;

private:

    int checkNextLevel(QString FilePath);

    //Locked functions
    //void addToCount(int value);

    bool alreadyContainsInDoneFiles(const QString& path);
    void addToDoneFiles(const QString& path);

    void addToDoFiles(const QString& path);

    //Input
    QString m_dir;
    QVector<WorldLevelTile> m_levels;
    QString m_intro;
    dataconfigs* m_configs;

    //Check data
    QStringList m_doneFiles;
    QStringList m_toDoFiles;

    //Output
    int m_count;
    bool m_wasCancelled;

    //Locker
    QMutex m_mutex;
};

#endif // ASYNCSTARCOUNTER_H
