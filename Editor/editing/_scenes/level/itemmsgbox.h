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

#pragma once
#ifndef ITEMMSGBOX_H
#define ITEMMSGBOX_H

#include <QDialog>

namespace Ui
{
    class ItemMsgBox;
}

enum Opened_By { NPC, EVENT, BLOCK };

class ItemMsgBox : public QDialog
{
    Q_OBJECT

public:
    Opened_By whoOpened;
    QString currentText;
    explicit ItemMsgBox(Opened_By openedBy, QString text = QString(),
                        bool isFriendly = false, QString label = QString(),
                        QString title = QString(), QWidget *parent = nullptr);
    ~ItemMsgBox();

    bool isFriendlyChecked();

private slots:
    void on_buttonBox_accepted();
    void updateDialogueSize(bool showPreview);
    void updateEngineDesc();

private:
    Ui::ItemMsgBox *ui;

    void loadSetup();
    void saveSetup();
};

#endif // ITEMMSGBOX_H
