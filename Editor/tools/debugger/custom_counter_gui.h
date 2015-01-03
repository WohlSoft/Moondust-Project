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

#ifndef CUSTOM_COUNTER_GUI_H
#define CUSTOM_COUNTER_GUI_H

#include <QDialog>

#include "custom_counter.h"

namespace Ui {
class CustomCounterGUI;
}

class CustomCounterGUI : public QDialog
{
    Q_OBJECT

public:
    explicit CustomCounterGUI(QWidget *parent = 0);
    ~CustomCounterGUI();
    void setCounterData(CustomCounter &data);
    void addCntItem(QString name, long id);
    void replaceItem(long what, long with);
    void removeItem(long what);
    void addCItem(long item, QString name="");
    QString makeItemName(long item);
    CustomCounter counterData;

private slots:
    void on_ItemType_currentIndexChanged(int);
    void on_ItemList_customContextMenuRequested(const QPoint &pos);
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

    void on_AddItem_clicked();

    void on_RemoveItem_clicked();

private:
    Ui::CustomCounterGUI *ui;
    bool lockCombobox;
};

#endif // CUSTOM_COUNTER_GUI_H
