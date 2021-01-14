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

#pragma once
#ifndef LEVELPROPS_H
#define LEVELPROPS_H

#include <memory>
#include <QDialog>
#include <QSpacerItem>

#include <PGE_File_Formats/lvl_filedata.h>

class MainWindow;
class JsonSettingsWidget;

namespace Ui {
class LevelProps;
}

class LevelProps : public QDialog
{
    Q_OBJECT

public:
    explicit LevelProps(LevelData &FileData,QWidget *parent = nullptr);
    ~LevelProps();
    QString m_levelTitle;
    QString m_customParams;

private slots:
    void on_LVLPropButtonBox_accepted();
    void on_LVLPropButtonBox_rejected();

private:
    void initAdvancedSettings();
    void onExtraSettingsChanged();

    std::unique_ptr<JsonSettingsWidget> m_extraSettings;
    std::unique_ptr<QSpacerItem> m_extraSettingsSpacer;

    Ui::LevelProps *ui = nullptr;
    LevelData *m_currentData = nullptr;
    MainWindow *m_mw = nullptr;
};

#endif // LEVELPROPS_H
