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

#ifndef LVL_CLONE_SECTION_H
#define LVL_CLONE_SECTION_H

#include <QDialog>

#include "level_edit.h"

namespace Ui {
class LvlCloneSection;
}

class LvlCloneSection : public QDialog
{
    Q_OBJECT

public:
    explicit LvlCloneSection(QWidget *parent = 0);
    ~LvlCloneSection();
    void addLevelList(QList<LevelEdit* > _levels, LevelEdit *active = NULL);

    int clone_margin;
    LevelEdit* clone_source;
    int clone_source_id;
    LevelEdit* clone_target;
    int clone_target_id;

private slots:
    void on_FileList_src_currentIndexChanged(int index);
    void on_FileList_dst_currentIndexChanged(int index);
    void on_buttonBox_accepted();

private:
    QList<LevelEdit* > levels;
    Ui::LvlCloneSection *ui;
};

#endif // LVL_CLONE_SECTION_H
