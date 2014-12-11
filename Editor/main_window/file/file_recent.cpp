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

#include <ui_mainwindow.h>
#include <mainwindow.h>

void MainWindow::SyncRecentFiles()
{
    ui->action_recent1->setText(QString(recentOpen[0]).replace("&", "&&&"));
    ui->action_recent2->setText(QString(recentOpen[1]).replace("&", "&&&"));
    ui->action_recent3->setText(QString(recentOpen[2]).replace("&", "&&&"));
    ui->action_recent4->setText(QString(recentOpen[3]).replace("&", "&&&"));
    ui->action_recent5->setText(QString(recentOpen[4]).replace("&", "&&&"));
    ui->action_recent6->setText(QString(recentOpen[5]).replace("&", "&&&"));
    ui->action_recent7->setText(QString(recentOpen[6]).replace("&", "&&&"));
    ui->action_recent8->setText(QString(recentOpen[7]).replace("&", "&&&"));
    ui->action_recent9->setText(QString(recentOpen[8]).replace("&", "&&&"));
    ui->action_recent10->setText(QString(recentOpen[9]).replace("&", "&&&"));

    ui->action_recent1->setEnabled(!(recentOpen[0]=="<empty>"));
    ui->action_recent2->setEnabled(!(recentOpen[1]=="<empty>"));
    ui->action_recent3->setEnabled(!(recentOpen[2]=="<empty>"));
    ui->action_recent4->setEnabled(!(recentOpen[3]=="<empty>"));
    ui->action_recent5->setEnabled(!(recentOpen[4]=="<empty>"));
    ui->action_recent6->setEnabled(!(recentOpen[5]=="<empty>"));
    ui->action_recent7->setEnabled(!(recentOpen[6]=="<empty>"));
    ui->action_recent8->setEnabled(!(recentOpen[7]=="<empty>"));
    ui->action_recent9->setEnabled(!(recentOpen[8]=="<empty>"));
    ui->action_recent10->setEnabled(!(recentOpen[9]=="<empty>"));
}

void MainWindow::AddToRecentFiles(QString FilePath)
{
    int index;
    FilePath.replace('\\', '/');

    if((index = recentOpen.indexOf(FilePath))!=-1){
        recentOpen.removeAt(index);
        recentOpen.push_front(FilePath);
    }else{
        //just in case
        if(recentOpen.size() >= 10){
            recentOpen.pop_back();
        }
        recentOpen.push_front(FilePath);
    }
}


void MainWindow::on_action_recent1_triggered()
{
    OpenFile(QString(ui->action_recent1->text()).replace("&&&", "&"));
}

void MainWindow::on_action_recent2_triggered()
{
    OpenFile(QString(ui->action_recent2->text()).replace("&&&", "&"));
}

void MainWindow::on_action_recent3_triggered()
{
    OpenFile(QString(ui->action_recent3->text()).replace("&&&", "&"));
}

void MainWindow::on_action_recent4_triggered()
{
    OpenFile(QString(ui->action_recent4->text()).replace("&&&", "&"));
}

void MainWindow::on_action_recent5_triggered()
{
    OpenFile(QString(ui->action_recent5->text()).replace("&&&", "&"));
}

void MainWindow::on_action_recent6_triggered()
{
    OpenFile(QString(ui->action_recent6->text()).replace("&&&", "&"));
}

void MainWindow::on_action_recent7_triggered()
{
    OpenFile(QString(ui->action_recent7->text()).replace("&&&", "&"));
}

void MainWindow::on_action_recent8_triggered()
{
    OpenFile(QString(ui->action_recent8->text()).replace("&&&", "&"));
}

void MainWindow::on_action_recent9_triggered()
{
    OpenFile(QString(ui->action_recent9->text()).replace("&&&", "&"));
}

void MainWindow::on_action_recent10_triggered()
{
    OpenFile(QString(ui->action_recent10->text()).replace("&&&", "&"));
}



