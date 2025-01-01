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

#include "js_file.h"
#include "js_utils.h"
#include <QFile>
#include <QDir>
#include <QFileDialog>
#include <common_features/app_path.h>
#include <mainwindow.h>

#include "qfile_dialogs_default_options.hpp"


PGE_JS_File::PGE_JS_File(QObject *parent)
    : QObject(parent)
{}

PGE_JS_File::PGE_JS_File(const QString &scriptPath, const QString &cpSettingsFile, QObject *parent)
    : QObject(parent),
      m_scriptPath(scriptPath),
      m_configSettingsFile(cpSettingsFile)
{}

PGE_JS_File::~PGE_JS_File() {}


void PGE_JS_File::setScriptPath(QString scriptPath)
{
    m_scriptPath = scriptPath;
}

QString PGE_JS_File::scriptPath()
{
    return m_scriptPath;
}

void PGE_JS_File::setConfigSettingsFile(QString filePath)
{
    m_configSettingsFile = filePath;
}

QString PGE_JS_File::configSettingFile()
{
    return m_configSettingsFile;
}

QString PGE_JS_File::appPath()
{
    return ApplicationPath;
}

QString PGE_JS_File::getOpenFilePath(QString caption, QString dir, QString filter)
{
    return QFileDialog::getOpenFileName(getWidgetParentOrNullptr(this), caption, dir, filter, nullptr, c_fileDialogOptions);
}

QString PGE_JS_File::getOpenDirPath(QString caption, QString dir)
{
    return QFileDialog::getExistingDirectory(getWidgetParentOrNullptr(this), caption, dir, c_dirDialogOptions);
}

bool PGE_JS_File::isFileExists(QString filePath)
{
    return QFile::exists(filePath);
}

bool PGE_JS_File::isDirExists(QString dirPath)
{
    return QDir(dirPath).exists();
}

QString PGE_JS_File::getBundleName(QString appPath)
{
    QFileInfo f(appPath);
    return f.bundleName();
}

bool PGE_JS_File::copy(QString source, QString target, bool override)
{
    if(QFile::exists(source))
    {
        if(QDir(target).exists())//Is target a directory
            target += "/" + QFile(source).fileName();
    }
    if(override)
    {
        if(QFile::exists(target))
            QFile::remove(target);
    }
    return QFile::copy(source, target);
}
