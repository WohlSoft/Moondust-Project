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

#include <QtConcurrent>
#include <QStyle>
#include <QApplication>
#include <pge_qt_compat.h>

#include <common_features/util.h>
#include "asyncstarcounter.h"


AsyncStarCounter::AsyncStarCounter(const QString &dir, const QVector<WorldLevelTile> &levels, const QString &introLevel, DataConfig* configs, QObject *parent) :
    QObject(parent),
    m_dir(dir),
    m_levels(levels),
    m_intro(introLevel),
    m_configs(configs),
    m_count(0),
    m_wasCancelled(false),
    m_mutex()
{}

void AsyncStarCounter::startAndShowProgress()
{
    bool introCounted=false;
    bool firstRun = true;
again:;

    QProgressDialog progress(tr("Calculating total star count in accessible levels"), tr("Abort"), 0, m_levels.size(), qobject_cast<QWidget*>(parent()));
    progress.setWindowTitle(tr("Counting stars..."));
    progress.setWindowModality(Qt::WindowModal);
    progress.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    progress.setFixedSize(progress.size());
    progress.setGeometry(util::alignToScreenCenter(progress.size()));
    progress.setMinimumDuration(0);


    QList<QPair<QString, int> > allLevels;

    if(firstRun){
        for(int i = 0; i < m_levels.size() || !introCounted; i++){
            QString FilePath;

            if(introCounted)
            {
                FilePath = m_dir+"/"+m_levels[i].lvlfile;
                if(m_levels[i].lvlfile.isEmpty()) continue;
            }
            else
            {
                FilePath = m_dir+"/"+m_intro;
                i--;
                introCounted=true;
                if(FilePath.isEmpty()) continue;
            }

            if(!FilePath.endsWith(".lvl", Qt::CaseInsensitive)&&
               !FilePath.endsWith(".lvlx", Qt::CaseInsensitive))
               FilePath.append(".lvl");

            allLevels << qMakePair(FilePath, 0);
        }
        firstRun = false;
    }else{
        foreach(QString nextFile, m_toDoFiles){
            allLevels << qMakePair(nextFile, 0);
        }
    }



    QFutureWatcher<void> watcher;

    connect(&progress, SIGNAL(canceled()), &watcher, SLOT(cancel()));
    connect(&watcher, SIGNAL(progressRangeChanged(int,int)), &progress, SLOT(setRange(int,int)));
    connect(&watcher, SIGNAL(progressValueChanged(int)), &progress, SLOT(setValue(int)));

    watcher.setFuture(QtConcurrent::map(allLevels, [this] (QPair<QString, int> &nextLevel){ nextLevel.second = checkNextLevel(nextLevel.first); }));

    progress.exec();
    watcher.waitForFinished();

    m_wasCancelled = watcher.isCanceled();
    if(!m_wasCancelled){
        for(int i = 0; i < allLevels.size(); i++){
            m_count += allLevels[i].second;
        }
    }

    for(const QString &doneFile : m_doneFiles)
        m_toDoFiles.removeAll(doneFile);

    if(!m_toDoFiles.isEmpty())
        goto again;

}

bool AsyncStarCounter::isCancelled() const
{
    return m_wasCancelled;
}

int AsyncStarCounter::countedStars() const
{
    return m_count;
}

int AsyncStarCounter::checkNextLevel(QString FilePath)
{
    addToDoneFiles(FilePath);
    qDebug() << "Scanning";

    Q_QRegExp lvlext = Q_QRegExp(".*\\.(lvl|lvlx)$");
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    lvlext.setCaseSensitivity(Qt::CaseInsensitive);
#else
    lvlext.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
#endif

    LevelData getLevelHead;
    long starCount = 0;
    getLevelHead.stars = 0;

    if(lvlext.Q_QRegExpMatch(FilePath))
    {
        if(!FileFormats::OpenLevelFile(FilePath, getLevelHead))
            return 0;

        //qDebug() << "world "<< getLevelHead.stars << getLevelHead.filename;

        int foundStars=0;
        //Mark all stars
        for(int q = 0; q < getLevelHead.npc.size(); ++q)
        {
           if(!m_configs->main_npc.contains(getLevelHead.npc[q].id)) continue;
           getLevelHead.npc[q].is_star = m_configs->main_npc[getLevelHead.npc[q].id].setup.is_star;
           if((getLevelHead.npc[q].is_star)&&(!getLevelHead.npc[q].friendly))
                foundStars++;
        }

        starCount += foundStars;//getLevelHead.stars;

        for(int i = 0; i < getLevelHead.doors.size(); ++i)
        {
            if(!getLevelHead.doors[i].lname.isEmpty())
            {
                QString FilePath_W = getLevelHead.meta.path+"/"+getLevelHead.doors[i].lname;

                if(!FilePath_W.endsWith(".lvl", Qt::CaseInsensitive)&&
                   !FilePath_W.endsWith(".lvlx", Qt::CaseInsensitive))
                   FilePath_W.append(".lvl");

                if(!QFileInfo(FilePath_W).exists()) continue;

                if(!alreadyContainsInDoneFiles(FilePath_W))
                {
                    addToDoFiles(FilePath_W);
                }
            }
        }
    }
    return starCount;
}

bool AsyncStarCounter::alreadyContainsInDoneFiles(const QString &path)
{
    QMutexLocker locker(&m_mutex);

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    for(const auto &q : m_doneFiles)
    {
        if(q.compare(path, Qt::CaseInsensitive) == 0)
            return true;
    }

    return false;
#else
    return m_doneFiles.contains(path, Qt::CaseInsensitive);
#endif
}

void AsyncStarCounter::addToDoneFiles(const QString &path)
{
    QMutexLocker locker(&m_mutex);
    m_doneFiles << path;
}

void AsyncStarCounter::addToDoFiles(const QString &path)
{
    QMutexLocker locker(&m_mutex);
    m_toDoFiles << path;
}
