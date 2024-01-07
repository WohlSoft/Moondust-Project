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

#include <QDir>

#include "smartimporter.h"

SmartImporter::SmartImporter(QObject *parent) :
    QObject(parent)
{}

SmartImporter::SmartImporter(QWidget *parentWid, const QString &importPath, QWidget *targetImport, QObject *parent) :
    QObject(parent)
{
    this->parentWid = parentWid;

    if(importPath.endsWith("/") || importPath.endsWith("\\"))
        this->importPath = importPath;
    else
        this->importPath = importPath + QString("/");

    targetLevelWindow = nullptr;
    targetWorldWindow = nullptr;
    if(QString(targetImport->metaObject()->className()) == LEVEL_EDIT_CLASS)
        targetLevelWindow = qobject_cast<LevelEdit *>(targetImport);
    else if(QString(targetImport->metaObject()->className()) == WORLD_EDIT_CLASS)
        targetWorldWindow = qobject_cast<WorldEdit *>(targetImport);
}

bool SmartImporter::isValid()
{
    return targetLevelWindow || targetWorldWindow;
}

static bool testCanImport(const QString &fName, const QString &prefix)
{
    return fName.startsWith(prefix, Qt::CaseInsensitive) &&
           (fName.endsWith(".gif", Qt::CaseInsensitive) ||
            fName.endsWith(".png", Qt::CaseInsensitive) ||
            fName.endsWith(".ini", Qt::CaseInsensitive) ||
            fName.endsWith(".txt", Qt::CaseInsensitive) ||
            fName.endsWith(".lua", Qt::CaseInsensitive));
}

static void findFilesInScript(const QString &importPath, const QStringList &allFiles, QStringList &outFiles)
{
    outFiles.clear();

    // HACKY, LAZILY AND TEMPORARY SOLUTION: just find any filename mentioned in the script or text file explicitly.
    // This thing does NOT finds any static filename menitoned in the script, but it does NOT builds filenames
    // based on strings computed by the code
    for(const QString &tarFile : allFiles)
    {
        if(tarFile.endsWith(".lua", Qt::CaseInsensitive) ||
           tarFile.endsWith(".txt", Qt::CaseInsensitive) ||
           tarFile.endsWith(".ini", Qt::CaseInsensitive))
        {
            QFile x(importPath + tarFile);

            if(x.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                QString content = QString::fromUtf8(x.readAll());
                x.close();

                for(const QString &subFile : allFiles)
                {
                    if(content.contains(subFile, Qt::CaseInsensitive))
                        outFiles << subFile;
                }
            }
        }
    }
}

bool SmartImporter::attemptFastImport()
{
    if(targetLevelWindow)
    {
        if(targetLevelWindow->isUntitled())
        {
            QMessageBox::warning(parentWid, tr("File not saved"), tr("You need to save the level, so you can import custom graphics!"), QMessageBox::Ok);
            return false;
        }

        CustomDirManager uLVL(targetLevelWindow->LvlData.meta.path, targetLevelWindow->LvlData.meta.filename);
        QDir sourceDir(importPath);
        if(!sourceDir.exists())
            return false;

        QStringList allFiles = sourceDir.entryList(QDir::Files | QDir::Readable, QDir::Name);

        QStringList extra_referred_files;
        findFilesInScript(importPath, allFiles, extra_referred_files);

        QStringList filteredFiles;
        for(const QString &tarFile : allFiles)
        {
            if(extra_referred_files.contains(tarFile)) // Any file mentioned in lua/txt/ini file
                filteredFiles << importPath + tarFile;
            else if(tarFile.endsWith(".tileset.ini", Qt::CaseInsensitive))
                filteredFiles << importPath + tarFile;
            else if(testCanImport(tarFile, "block-"))
                filteredFiles << importPath + tarFile;
            else if(testCanImport(tarFile, "background-"))
                filteredFiles << importPath + tarFile;
            else if(testCanImport(tarFile, "background2-"))
                filteredFiles << importPath + tarFile;
            else if(testCanImport(tarFile, "npc-"))
                filteredFiles << importPath + tarFile;
            else if(testCanImport(tarFile, "effect-"))
                filteredFiles << importPath + tarFile;
            else if(testCanImport(tarFile, "yoshib-"))
                filteredFiles << importPath + tarFile;
            else if(testCanImport(tarFile, "yoshit-"))
                filteredFiles << importPath + tarFile;
            else if(testCanImport(tarFile, "mario-"))
                filteredFiles << importPath + tarFile;
            else if(testCanImport(tarFile, "luigi-"))
                filteredFiles << importPath + tarFile;
            else if(testCanImport(tarFile, "peach-"))
                filteredFiles << importPath + tarFile;
            else if(testCanImport(tarFile, "toat-"))
                filteredFiles << importPath + tarFile;
            else if(testCanImport(tarFile, "link-"))
                filteredFiles << importPath + tarFile;
        }

        uLVL.createDirIfNotExsist();
        uLVL.import(filteredFiles);
        return true;
    }
    else if(targetWorldWindow)
    {
        if(targetWorldWindow->isUntitled())
        {
            QMessageBox::warning(parentWid, tr("File not saved"), tr("You need to save the world, so you can import custom graphics!"), QMessageBox::Ok);
            return false;
        }

        CustomDirManager uWLD(targetWorldWindow->WldData.meta.path, targetWorldWindow->WldData.meta.filename);
        QDir sourceDir(importPath);
        if(!sourceDir.exists())
            return false;

        QStringList allFiles = sourceDir.entryList(QDir::Files | QDir::Readable, QDir::Name);

        QStringList extra_referred_files;
        findFilesInScript(importPath, allFiles, extra_referred_files);

        QStringList filteredFiles;
        for(const QString &tarFile : allFiles)
        {
            //Also import global used custom level data
            if(extra_referred_files.contains(tarFile)) // Any file mentioned in lua/txt/ini file
                filteredFiles << importPath + tarFile;
            else if(testCanImport(tarFile, "player-"))
                filteredFiles << importPath + tarFile;
            else if(testCanImport(tarFile, "tile-"))
                filteredFiles << importPath + tarFile;
            else if(testCanImport(tarFile, "path-"))
                filteredFiles << importPath + tarFile;
            else if(testCanImport(tarFile, "level-"))
                filteredFiles << importPath + tarFile;
            else if(testCanImport(tarFile, "scene-"))
                filteredFiles << importPath + tarFile;
        }

        uWLD.createDirIfNotExsist();
        uWLD.import(filteredFiles, false);
        return true;
    }
    return false;
}

QString SmartImporter::getImportPath() const
{
    return importPath;
}

void SmartImporter::setImportPath(const QString &value)
{
    importPath = value;
}
