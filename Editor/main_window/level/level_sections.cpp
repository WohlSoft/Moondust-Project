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

#include <editing/_scenes/level/lvl_scene.h>
#include <main_window/dock/lvl_sctc_props.h>

#include <ui_mainwindow.h>
#include "mainwindow.h"
#include <ui_lvl_sctc_props.h>

void MainWindow::setCurrentLevelSection(int sectionId, int open)
{
    dock_LvlSectionProps->m_externalLock = true;

    int dstSectionId = sectionId;
    int winType = activeChildWindow();
    LevelEdit *e = nullptr;
    if(winType == WND_Level)
        e = activeLvlEditWin();

    LogDebug("Set Current Section");
    if((open == 1) && e) // Only Set Checked section number without section select
    {
        LogDebug("get Current Section");
        dstSectionId = e->LvlData.CurSection;
    }

    /*FIXME:
     * Generate section buttons dynamically and access them through an array by index!
     */
    LogDebug("Set checkbox to");
    for(size_t i = 0; i < m_sectionButtonsCount; i++)
    {
        Q_ASSERT(m_sectionButtons[i]);
        m_sectionButtons[i]->setChecked(static_cast<size_t>(dstSectionId) == i);
    }
    ui->actionSectionMore->setChecked((dstSectionId > 20));

    if(e)
    {
        if(open == 0)
        {
            LogDebug("Call to setCurrentSection()");
            e->setCurrentSection(dstSectionId);
        }
        dock_LvlSectionProps->refreshFileData();
    }

    dock_LvlSectionProps->m_externalLock = false;
}


void MainWindow::on_actionGotoLeftBottom_triggered()
{
    if(activeChildWindow() == WND_Level)
    {
        auto *edit = activeLvlEditWin();
        if(!edit)
            return;
        int SectionId = edit->LvlData.CurSection;
        int xb = edit->LvlData.sections[SectionId].size_left;
        int yb = edit->LvlData.sections[SectionId].size_bottom;
        edit->goTo(xb, yb, false, QPoint(-10, 10 - edit->scene->m_viewPort->viewport()->height()));
    }
    else if(activeChildWindow() == WND_World)
    {
        WorldEdit *e = activeWldEditWin();
        if(e) e->ResetPosition();
    }
}

void MainWindow::on_actionGotoLeftTop_triggered()
{
    qApp->setActiveWindow(this);
    int winType = activeChildWindow();
    if(winType == WND_Level)
    {
        auto *edit = activeLvlEditWin();
        if(!edit)
            return;
        int SectionId = edit->LvlData.CurSection;
        int xb = edit->LvlData.sections[SectionId].size_left;
        int yb = edit->LvlData.sections[SectionId].size_top;
        edit->goTo(xb, yb, false, QPoint(-10, -10));
    }
}

void MainWindow::on_actionGotoTopRight_triggered()
{
    qApp->setActiveWindow(this);
    int winType = activeChildWindow();
    if(winType == WND_Level)
    {
        auto *edit = activeLvlEditWin();
        if(!edit)
            return;
        int SectionId = edit->LvlData.CurSection;
        int xb = edit->LvlData.sections[SectionId].size_right;
        int yb = edit->LvlData.sections[SectionId].size_top;
        edit->goTo(xb, yb, false, QPoint(10 - edit->scene->m_viewPort->viewport()->width(), -10));
    }
}

void MainWindow::on_actionGotoRightBottom_triggered()
{
    qApp->setActiveWindow(this);
    int winType = activeChildWindow();
    if(winType == WND_Level)
    {
        auto *edit = activeLvlEditWin();
        if(!edit)
            return;
        int SectionId = edit->LvlData.CurSection;
        int xb = edit->LvlData.sections[SectionId].size_right;
        int yb = edit->LvlData.sections[SectionId].size_bottom;
        edit->goTo(xb, yb, false, QPoint(10 - edit->scene->m_viewPort->viewport()->width(), 10 - edit->scene->m_viewPort->viewport()->height()));
    }
}



bool MainWindow::getCurrentSceneCoordinates(qreal &x, qreal &y)
{
    if(activeChildWindow() == WND_Level)
    {
        auto *edit = activeLvlEditWin();
        if(!edit)
            return false;
        QPointF coor = edit->getGraphicsView()->mapToScene(0, 0);
        x = coor.x();
        y = coor.y();
        return true;
    }
    else if(activeChildWindow() == WND_World)
    {
        auto *edit = activeWldEditWin();
        if(!edit)
            return false;
        QPointF coor = edit->getGraphicsView()->mapToScene(0, 0);
        x = coor.x();
        y = coor.y();
        return true;
    }
    return false;
}


// //////////////////////////////// GoTo Section  ////////////////////////////////
void MainWindow::on_actionSection_1_triggered()
{
    setCurrentLevelSection(0);
}

void MainWindow::on_actionSection_2_triggered()
{
    setCurrentLevelSection(1);
}

void MainWindow::on_actionSection_3_triggered()
{
    setCurrentLevelSection(2);
}

void MainWindow::on_actionSection_4_triggered()
{
    setCurrentLevelSection(3);
}

void MainWindow::on_actionSection_5_triggered()
{
    setCurrentLevelSection(4);
}

void MainWindow::on_actionSection_6_triggered()
{
    setCurrentLevelSection(5);
}

void MainWindow::on_actionSection_7_triggered()
{
    setCurrentLevelSection(6);
}

void MainWindow::on_actionSection_8_triggered()
{
    setCurrentLevelSection(7);
}

void MainWindow::on_actionSection_9_triggered()
{
    setCurrentLevelSection(8);
}

void MainWindow::on_actionSection_10_triggered()
{
    setCurrentLevelSection(9);
}

void MainWindow::on_actionSection_11_triggered()
{
    setCurrentLevelSection(10);
}

void MainWindow::on_actionSection_12_triggered()
{
    setCurrentLevelSection(11);
}


void MainWindow::on_actionSection_13_triggered()
{
    setCurrentLevelSection(12);
}

void MainWindow::on_actionSection_14_triggered()
{
    setCurrentLevelSection(13);
}

void MainWindow::on_actionSection_15_triggered()
{
    setCurrentLevelSection(14);
}

void MainWindow::on_actionSection_16_triggered()
{
    setCurrentLevelSection(15);
}

void MainWindow::on_actionSection_17_triggered()
{
    setCurrentLevelSection(16);
}

void MainWindow::on_actionSection_18_triggered()
{
    setCurrentLevelSection(17);
}

void MainWindow::on_actionSection_19_triggered()
{
    setCurrentLevelSection(18);
}

void MainWindow::on_actionSection_20_triggered()
{
    setCurrentLevelSection(19);
}

void MainWindow::on_actionSection_21_triggered()
{
    setCurrentLevelSection(20);
}

void MainWindow::on_actionSectionMore_triggered()
{
    if(activeChildWindow() != WND_Level)
        return;

    auto *edit = activeLvlEditWin();
    QMenu section;
    for(int i = 21; i < edit->LvlData.sections.size(); i++)
    {
        QAction *item = section.addAction(tr("Section %1").arg(i));
        if(edit->LvlData.CurSection == i)
        {
            item->setCheckable(true);
            item->setChecked(true);
        }
        item->setData(i);
    }
    auto *newSection = section.addAction(tr("Initialize section %1...").arg(edit->LvlData.sections.size()));
    newSection->setData(edit->LvlData.sections.size());
    auto *answer = section.exec(QCursor::pos());
    if(!answer)
        return;

    setCurrentLevelSection(answer->data().toInt());
}
