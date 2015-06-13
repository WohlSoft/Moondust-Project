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

#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QDialog>
#include <QMdiArea>

namespace Ui {
class AppSettings;
}

class AppSettings : public QDialog
{
    Q_OBJECT

public:
    explicit AppSettings(QWidget *parent = 0);
    ~AppSettings();

    bool    autoPlayMusic;
    bool    Animation;
    long    AnimationItemLimit;

    bool    Collisions;

    bool    logEnabled;
    int     loglevel;

    bool    midmouse_allowDupe;
    bool    midmouse_allowPlace;
    bool    midmouse_allowDragMode;

    bool    placing_dont_show_props_box;

    int     historyLimit;

    QMdiArea::ViewMode   MainWindowView;
    QTabWidget::TabPosition LVLToolboxPos;
    QTabWidget::TabPosition WLDToolboxPos;
    QTabWidget::TabPosition TSTToolboxPos;

    QString logfile;
    QString selectedTheme;

    void applySettings();
    static bool associateFiles();

private slots:
    void on_setLogFile_clicked();
    void on_buttonBox_accepted();

    void on_AssociateFiles_clicked();

private:
    Ui::AppSettings *ui;
};

#endif // APPSETTINGS_H
