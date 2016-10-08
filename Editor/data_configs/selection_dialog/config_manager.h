/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2016 Vitaly Novichkov <admin@wohlnet.ru>
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
#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <QDialog>
#include <QListWidget>
#include <QSettings>
#include <QPixmap>

namespace Ui {
class ConfigManager;
}

class ConfigManager : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigManager(QWidget *parent = 0);

    ~ConfigManager();
    /*!
     * \brief Checks availability of configuration packages and spawns message box if config packs are not presented
     * \return true if config packs are presented, false if no config packs are found
     */
    bool hasConfigPacks();
    //! Currently selected configuration package name
    QString m_currentConfig;
    //! Path to currently selected configuration package
    QString m_currentConfigPath;
    //! Default theme pack associate with this configuration package
    QString m_themePackName;

    //! Returns preloaded configuration package
    QString loadConfigs();
    /**
     * @brief Is necessary to ask for configuration package or load already preloaded config pack
     * @param _x state of "Ask again" flag
     */
    void setAskAgain(bool _x);

    //! "Ask again" flag which defines necessary to show config selection dialog every launch of the editor
    bool m_doAskAgain;

    /**
     * @brief Checks is current config pack configured (or non-configurable)
     * @return true if config pack is non-configurable or already configured
     */
    bool isConfigured();
    /**
     * @brief Starts configure tool if available
     * @return true on success configuring, false if no config tool found, rejected or script was been errored
     */
    bool runConfigureTool();

private slots:

    void on_configList_itemDoubleClicked(QListWidgetItem *item);

    void on_buttonBox_accepted();

    /**
     * @brief Loads configuration packs list
     */
    void loadConfigPackList();
    /**
     * @brief Saves settings of selected configuration package
     */
    void saveCurrentSettings();

private:
    /**
     * @brief Process auto-configuring
     * @return true if non-configurable config pack or successfully configured
     */
    bool checkForConfigureTool();

    Ui::ConfigManager *ui;
};

#endif // CONFIG_MANAGER_H
