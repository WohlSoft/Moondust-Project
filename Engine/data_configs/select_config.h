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

#ifndef SELECT_CONFIG_H
#define SELECT_CONFIG_H

#include <QDialog>
#include <QListWidget>
#include <QSettings>
#include <QPixmap>


namespace Ui {
class SelectConfig;
}

class SelectConfig : public QDialog
{
    Q_OBJECT

public:
    explicit SelectConfig(QWidget *parent = 0);
    ~SelectConfig();
    QString currentConfig;
    QString currentConfigPath;
    QString themePack;
    QString isPreLoaded(QString openConfig="");

private slots:
    void on_configList_itemDoubleClicked(QListWidgetItem *item);
    void on_buttonBox_accepted();

private:
    Ui::SelectConfig *ui;
};

#endif // SELECT_CONFIG_H
