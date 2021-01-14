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
#ifndef EDIT_BASE_HHHH
#define EDIT_BASE_HHHH

#include <QWidget>
#include <data_configs/data_configs.h>
#include <main_window/global_settings.h>

class MainWindow;

/**
 * @brief Generic interface class over file editing
 */
class EditBase : public QWidget
{
    Q_OBJECT

public:
    explicit EditBase(MainWindow *mw, QWidget *parent = nullptr);
    virtual ~EditBase();

    MainWindow *getMainWindow();

    virtual bool save(bool savOptionsDialog = false) = 0;

    virtual bool saveAs(bool savOptionsDialog = false) = 0;

    /**
     * @brief If file was modified, ask user for save.
     * @return true when user have confirmed saving by yes or no. false when user has cancelled saving.
     */
    virtual bool trySave() = 0;

    virtual QString userFriendlyCurrentFile() = 0;
    virtual QString currentFile() = 0;

    virtual bool    isUntitled() = 0;
    virtual bool    isModified() = 0;

    /**
     * @brief Clear untitled and modified states to permit closing
     */
    virtual void    markForForceClose() = 0;

    /*TODO:
      - Implement "isXxxSupported" for most of generic actions and toolboxes
      - Implement switchable proxy that will synchronize state between main window frontend and subwindow's backend
      - More generic actions like history manager, zoom, etc!
     */

protected:
    MainWindow *m_mw = nullptr;
};

#endif //EDIT_BASE_HHHH
