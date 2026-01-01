/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2026 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifdef Q_OS_MACX
#undef Q_OS_MACX//Workaround for AutoMOC, avoiding of re-definiton warnings
#endif

#include <ui_mainwindow.h>
#include <mainwindow.h>

void MainWindow::SyncRecentFiles()
{
    const QString emptyLabel = QStringLiteral("<empty>");

    ui->action_recent1->setText(util::str2ui3(recentOpen[0]));
    ui->action_recent2->setText(util::str2ui3(recentOpen[1]));
    ui->action_recent3->setText(util::str2ui3(recentOpen[2]));
    ui->action_recent4->setText(util::str2ui3(recentOpen[3]));
    ui->action_recent5->setText(util::str2ui3(recentOpen[4]));
    ui->action_recent6->setText(util::str2ui3(recentOpen[5]));
    ui->action_recent7->setText(util::str2ui3(recentOpen[6]));
    ui->action_recent8->setText(util::str2ui3(recentOpen[7]));
    ui->action_recent9->setText(util::str2ui3(recentOpen[8]));
    ui->action_recent10->setText(util::str2ui3(recentOpen[9]));

    ui->action_recent1->setEnabled(recentOpen[0] != emptyLabel);
    ui->action_recent2->setEnabled(recentOpen[1] != emptyLabel);
    ui->action_recent3->setEnabled(recentOpen[2] != emptyLabel);
    ui->action_recent4->setEnabled(recentOpen[3] != emptyLabel);
    ui->action_recent5->setEnabled(recentOpen[4] != emptyLabel);
    ui->action_recent6->setEnabled(recentOpen[5] != emptyLabel);
    ui->action_recent7->setEnabled(recentOpen[6] != emptyLabel);
    ui->action_recent8->setEnabled(recentOpen[7] != emptyLabel);
    ui->action_recent9->setEnabled(recentOpen[8] != emptyLabel);
    ui->action_recent10->setEnabled(recentOpen[9] != emptyLabel);
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
    OpenFile(util::ui32str(ui->action_recent1->text()));
}

void MainWindow::on_action_recent2_triggered()
{
    OpenFile(util::ui32str(ui->action_recent2->text()));
}

void MainWindow::on_action_recent3_triggered()
{
    OpenFile(util::ui32str(ui->action_recent3->text()));
}

void MainWindow::on_action_recent4_triggered()
{
    OpenFile(util::ui32str(ui->action_recent4->text()));
}

void MainWindow::on_action_recent5_triggered()
{
    OpenFile(util::ui32str(ui->action_recent5->text()));
}

void MainWindow::on_action_recent6_triggered()
{
    OpenFile(util::ui32str(ui->action_recent6->text()));
}

void MainWindow::on_action_recent7_triggered()
{
    OpenFile(util::ui32str(ui->action_recent7->text()));
}

void MainWindow::on_action_recent8_triggered()
{
    OpenFile(util::ui32str(ui->action_recent8->text()));
}

void MainWindow::on_action_recent9_triggered()
{
    OpenFile(util::ui32str(ui->action_recent9->text()));
}

void MainWindow::on_action_recent10_triggered()
{
    OpenFile(util::ui32str(ui->action_recent10->text()));
}
