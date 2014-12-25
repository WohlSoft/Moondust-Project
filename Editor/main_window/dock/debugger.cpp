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

#include <QSettings>

#include <common_features/app_path.h>
#include <common_features/util.h>
#include <tools/debugger/custom_counter_gui.h>

#include "../../mainwindow.h"
#include <ui_mainwindow.h>


void MainWindow::on_debuggerBox_visibilityChanged(bool visible)
{
    ui->actionDebugger->setChecked(visible);
}

void MainWindow::on_actionDebugger_triggered(bool checked)
{
    ui->debuggerBox->setVisible(checked);
    if(checked) ui->debuggerBox->raise();
}

void MainWindow::Debugger_UpdateMousePosition(QPoint p, bool isOffScreen)
{
    if(!ui->actionDebugger->isVisible()) return; //SpeedUp

    if(isOffScreen)
    {
        ui->DEBUG_MouseX->setText("");
        ui->DEBUG_MouseY->setText("");
    }
    else
    {
        ui->DEBUG_MouseX->setText(QString::number(p.x()));
        ui->DEBUG_MouseY->setText(QString::number(p.y()));
    }
}

void MainWindow::Debugger_UpdateItemList(QString list)
{
    if(!ui->actionDebugger->isVisible()) return; //SpeedUp

    ui->DEBUG_Items->setText(list);
}



void MainWindow::on_DEBUG_GotoPoint_clicked()
{
    if(ui->DEBUG_GotoX->text().isEmpty()) return;
    if(ui->DEBUG_GotoY->text().isEmpty()) return;

    if(activeChildWindow()==1)
    {
        activeLvlEditWin()->goTo(
                    ui->DEBUG_GotoX->text().toInt(),
                    ui->DEBUG_GotoY->text().toInt(), true, QPoint(0,0), true);
    }
    else if(activeChildWindow()==3)
    {
        activeWldEditWin()->goTo(
                    ui->DEBUG_GotoX->text().toInt(),
                    ui->DEBUG_GotoY->text().toInt(), true, QPoint(0,0), true);

    }

    ui->DEBUG_GotoX->setText("");
    ui->DEBUG_GotoY->setText("");
}

namespace mainwindow_debugger_box
{
    QVector<CustomCounter > customCounters;
    bool isLoaded=false;
}


void MainWindow::Debugger_loadCustomCounters()
{
    using namespace mainwindow_debugger_box;
    QSettings cCounters(ApplicationPath+"/pge_editor.ini", QSettings::IniFormat);
    cCounters.setIniCodec("UTF-8");

    cCounters.beginGroup("custom-counters");
        int cntn = cCounters.value("total", 0).toInt();
    cCounters.endGroup();

    customCounters.clear();

    for(;cntn>0;cntn--)
    {
        bool valid=true;
        CustomCounter counter;
        QStringList items;
        cCounters.beginGroup(QString("custom-counter-%1").arg(cntn));
            counter.name = cCounters.value("name", "").toString();
                if(!counter.name.isEmpty()) valid=false;
            counter.type = (ItemTypes::itemTypes)cCounters.value("type", 0).toInt();
            counter.windowType = cCounters.value("window-type", 0).toInt();
            QString tmp = cCounters.value("items", "").toString();
                if(!tmp.isEmpty()) {valid=false; goto skip;}
                items = tmp.split(',');
                foreach(QString x, items)
                {
                    counter.items.push_back(x.toLong());
                }
                skip:
        cCounters.endGroup();
        if(valid) customCounters.push_back(counter);
    }
}


void MainWindow::Debugger_saveCustomCounters()
{
    using namespace mainwindow_debugger_box;
    int win = activeChildWindow();

    util::memclear(ui->DEBUG_CustomCountersList);
    for(int i=0;i<customCounters.size();i++)
    {
        if(customCounters[i].windowType!=win) continue;
        QListWidgetItem *item;
        item = new QListWidgetItem(ui->DEBUG_CustomCountersList);
        item->setData(Qt::UserRole, QVariant(i));
        item->setText(QString("%1\t%2").arg(0).arg(customCounters[i].name));
        ui->DEBUG_CustomCountersList->addItem(item);
    }

}



void MainWindow::on_DEBUG_AddCustomCounter_clicked()
{
    using namespace mainwindow_debugger_box;
    CustomCounterGUI customCounterGui;
    customCounterGui.setWindowFlags (Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    customCounterGui.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, customCounterGui.size(), qApp->desktop()->availableGeometry()));

    if(customCounterGui.exec()==QDialog::Accepted)
    {
        qApp->setActiveWindow(this);
        customCounterGui.counterData.windowType = activeChildWindow();
        customCounters.push_back(customCounterGui.counterData);
        on_DEBUG_RefreshCoutners_clicked();
    }
}


void MainWindow::on_DEBUG_RefreshCoutners_clicked()
{
    using namespace mainwindow_debugger_box;
    if(!isLoaded)
        Debugger_loadCustomCounters();

}


void MainWindow::on_DEBUG_CustomCountersList_itemClicked(QListWidgetItem *item)
{
    using namespace mainwindow_debugger_box;

}


void MainWindow::on_DEBUG_CustomCountersList_customContextMenuRequested(const QPoint &pos)
{
    using namespace mainwindow_debugger_box;

}

