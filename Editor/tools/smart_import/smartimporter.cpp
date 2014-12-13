/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
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

    targetLevelWindow = qobject_cast<LevelEdit*>(targetImport);
    targetWorldWindow = qobject_cast<WorldEdit*>(targetImport);
}

bool SmartImporter::isValid()
{
    return targetLevelWindow || targetWorldWindow;
}

bool SmartImporter::attemptFastImport()
{
    if(targetLevelWindow){
        if(targetLevelWindow->isUntitled){
            QMessageBox::warning(parentWid, tr("File not saved"), tr("You need to save the level, so you can import custom graphics!"), QMessageBox::Ok);
            return false;
        }
        CustomDirManager uLVL(targetLevelWindow->LvlData.path, targetLevelWindow->LvlData.filename);
        QDir sourceDir(importPath);
        if(!sourceDir.exists())
            return false;

        QStringList allFiles = sourceDir.entryList(QDir::Files | QDir::Readable, QDir::Name);
        QStringList filteredFiles;
        foreach (QString tarFile, allFiles) {
            if(tarFile.endsWith(".tileset.ini", Qt::CaseInsensitive))
                filteredFiles << importPath + tarFile;
            if(tarFile.startsWith("block-", Qt::CaseInsensitive) && tarFile.endsWith(".gif", Qt::CaseInsensitive))
                filteredFiles << importPath + tarFile;
            if(tarFile.startsWith("background-", Qt::CaseInsensitive) && tarFile.endsWith(".gif", Qt::CaseInsensitive))
                filteredFiles << importPath + tarFile;
            if(tarFile.startsWith("background2-", Qt::CaseInsensitive) && tarFile.endsWith(".gif", Qt::CaseInsensitive))
                filteredFiles << importPath + tarFile;
            if(tarFile.startsWith("npc-", Qt::CaseInsensitive) &&
                    (tarFile.endsWith(".gif", Qt::CaseInsensitive)||
                    tarFile.endsWith(".txt", Qt::CaseInsensitive)))
                filteredFiles << importPath + tarFile;
            if(tarFile.startsWith("effect-", Qt::CaseInsensitive) && tarFile.endsWith(".gif", Qt::CaseInsensitive))
                filteredFiles << importPath + tarFile;
            if(tarFile.startsWith("yoshib-", Qt::CaseInsensitive) && tarFile.endsWith(".gif", Qt::CaseInsensitive))
                filteredFiles << importPath + tarFile;
            if(tarFile.startsWith("yoshit-", Qt::CaseInsensitive) && tarFile.endsWith(".gif", Qt::CaseInsensitive))
                filteredFiles << importPath + tarFile;
            if(tarFile.startsWith("mario-", Qt::CaseInsensitive) && tarFile.endsWith(".gif", Qt::CaseInsensitive))
                filteredFiles << importPath + tarFile;
            if(tarFile.startsWith("luigi-", Qt::CaseInsensitive) && tarFile.endsWith(".gif", Qt::CaseInsensitive))
                filteredFiles << importPath + tarFile;
            if(tarFile.startsWith("peach-", Qt::CaseInsensitive) && tarFile.endsWith(".gif", Qt::CaseInsensitive))
                filteredFiles << importPath + tarFile;
            if(tarFile.startsWith("toad-", Qt::CaseInsensitive) && tarFile.endsWith(".gif", Qt::CaseInsensitive))
                filteredFiles << importPath + tarFile;
            if(tarFile.startsWith("link-", Qt::CaseInsensitive) && tarFile.endsWith(".gif", Qt::CaseInsensitive))
                filteredFiles << importPath + tarFile;
        }

        uLVL.createDirIfNotExsist();
        uLVL.import(filteredFiles);
        return true;
    }else if(targetWorldWindow){
        if(targetWorldWindow->isUntitled){
            QMessageBox::warning(parentWid, tr("File not saved"), tr("You need to save the world, so you can import custom graphics!"), QMessageBox::Ok);
            return false;
        }

        CustomDirManager uLVL(targetLevelWindow->LvlData.path, targetLevelWindow->LvlData.filename);
        QDir sourceDir(importPath);
        if(!sourceDir.exists())
            return false;

        QStringList allFiles = sourceDir.entryList(QDir::Files | QDir::Readable, QDir::Name);
        QStringList filteredFiles;
        foreach (QString tarFile, allFiles) {
            //Also import global used custom level data
            if(tarFile.startsWith("block-", Qt::CaseInsensitive) && tarFile.endsWith(".gif", Qt::CaseInsensitive))
                filteredFiles << importPath + tarFile;
            if(tarFile.startsWith("background-", Qt::CaseInsensitive) && tarFile.endsWith(".gif", Qt::CaseInsensitive))
                filteredFiles << importPath + tarFile;
            if(tarFile.startsWith("background2-", Qt::CaseInsensitive) && tarFile.endsWith(".gif", Qt::CaseInsensitive))
                filteredFiles << importPath + tarFile;
            if(tarFile.startsWith("npc-", Qt::CaseInsensitive) &&
                    (tarFile.endsWith(".gif", Qt::CaseInsensitive)||
                    tarFile.endsWith(".txt", Qt::CaseInsensitive)))
                filteredFiles << importPath + tarFile;
            if(tarFile.startsWith("effect-", Qt::CaseInsensitive) && tarFile.endsWith(".gif", Qt::CaseInsensitive))
                filteredFiles << importPath + tarFile;
            if(tarFile.startsWith("yoshib-", Qt::CaseInsensitive) && tarFile.endsWith(".gif", Qt::CaseInsensitive))
                filteredFiles << importPath + tarFile;
            if(tarFile.startsWith("yoshit-", Qt::CaseInsensitive) && tarFile.endsWith(".gif", Qt::CaseInsensitive))
                filteredFiles << importPath + tarFile;
            if(tarFile.startsWith("mario-", Qt::CaseInsensitive) && tarFile.endsWith(".gif", Qt::CaseInsensitive))
                filteredFiles << importPath + tarFile;
            if(tarFile.startsWith("luigi-", Qt::CaseInsensitive) && tarFile.endsWith(".gif", Qt::CaseInsensitive))
                filteredFiles << importPath + tarFile;
            if(tarFile.startsWith("peach-", Qt::CaseInsensitive) && tarFile.endsWith(".gif", Qt::CaseInsensitive))
                filteredFiles << importPath + tarFile;
            if(tarFile.startsWith("toad-", Qt::CaseInsensitive) && tarFile.endsWith(".gif", Qt::CaseInsensitive))
                filteredFiles << importPath + tarFile;
            if(tarFile.startsWith("link-", Qt::CaseInsensitive) && tarFile.endsWith(".gif", Qt::CaseInsensitive))
                filteredFiles << importPath + tarFile;
            if(tarFile.startsWith("tile-", Qt::CaseInsensitive) && tarFile.endsWith(".gif", Qt::CaseInsensitive))
                filteredFiles << importPath + tarFile;
            if(tarFile.startsWith("path-", Qt::CaseInsensitive) && tarFile.endsWith(".gif", Qt::CaseInsensitive))
                filteredFiles << importPath + tarFile;
            if(tarFile.startsWith("level-", Qt::CaseInsensitive) && tarFile.endsWith(".gif", Qt::CaseInsensitive))
                filteredFiles << importPath + tarFile;
            if(tarFile.startsWith("scene-", Qt::CaseInsensitive) && tarFile.endsWith(".gif", Qt::CaseInsensitive))
                filteredFiles << importPath + tarFile;
        }

        uLVL.createDirIfNotExsist();
        uLVL.import(filteredFiles, false);
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

