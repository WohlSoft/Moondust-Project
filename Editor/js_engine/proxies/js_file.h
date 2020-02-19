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
#ifndef PGE_JS_FILE_H
#define PGE_JS_FILE_H

#include <QString>
#include <QObject>

class PGE_JS_File : public QObject
{
    Q_OBJECT
    //! Path to the script file
    QString m_scriptPath;
public:
    explicit PGE_JS_File(QObject* parent = NULL);
    explicit PGE_JS_File(const QString& scriptPath, QObject* parent = NULL);
    virtual ~PGE_JS_File();

    void setScriptPath(QString scriptPath);
    Q_INVOKABLE QString scriptPath();

    Q_INVOKABLE QString appPath();
    Q_INVOKABLE QString getOpenFilePath(QString caption, QString dir = QString(), QString filter = QString("All files (*)"));
    Q_INVOKABLE QString getOpenDirPath(QString caption, QString dir);
    Q_INVOKABLE bool    isFileExists(QString filePath);
    Q_INVOKABLE bool    isDirExists(QString dirPath);

    Q_INVOKABLE bool    copy(QString source, QString target, bool override = false);
};

#endif // PGE_JS_FILE_H
