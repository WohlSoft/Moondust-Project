#include "dir_copy.h"
#include <QQueue>
#include <QPair>
#include <QDir>
#include <QFile>

void DirCopy::copy(const QString& sourceFolder,const QString& destFolder)
{
    QQueue< QPair<QString, QString> > queue;

    queue.enqueue(qMakePair(sourceFolder, destFolder));

    while (!queue.isEmpty())
    {
        QPair<QString, QString> pair = queue.dequeue();
        QDir sourceDir(pair.first);
        QDir destDir(pair.second);

        if(!sourceDir.exists())
        continue;

        if(!destDir.exists())
        destDir.mkpath(pair.second);

        QStringList files = sourceDir.entryList(QDir::Files);
        for(int i = 0; i < files.count(); i++)
        {
            QString srcName = pair.first + "/" + files.at(i);
            QString destName = pair.second + "/" + files.at(i);
            QFile::copy(srcName, destName);
        }

        QStringList dirs = sourceDir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
        for(int i = 0; i < dirs.count(); i++)
        {
            QString srcName = pair.first + "/" + dirs.at(i);
            QString destName = pair.second + "/" + dirs.at(i);
            queue.enqueue(qMakePair(srcName, destName));
        }
    }
}
