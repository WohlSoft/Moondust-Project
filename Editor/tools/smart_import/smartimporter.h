/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2015 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef SMARTIMPORTER_H
#define SMARTIMPORTER_H

#include <QObject>

#include <editing/edit_level/level_edit.h>
#include <editing/edit_world/world_edit.h>

///
/// \brief The SmartImporter is a class which imports custom graphics by dragging a folder into the level window.
///
class SmartImporter : public QObject
{
    Q_OBJECT
public:
    explicit SmartImporter(QObject *parent = 0);
    explicit SmartImporter(QWidget *parentWid, const QString& importPath, QWidget* targetImport, QObject *parent = 0);

    bool isValid();
    bool attemptFastImport();

    QString getImportPath() const;
    void setImportPath(const QString &value);

signals:

public slots:

private:

    LevelEdit* targetLevelWindow;
    WorldEdit* targetWorldWindow;
    QString importPath;
    QWidget *parentWid;
};

#endif // SMARTIMPORTER_H
