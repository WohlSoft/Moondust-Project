/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <QMessageBox>
#include <QFileDialog>

#include <common_features/app_path.h>
#include <dev_console/devconsole.h>
#include <data_configs/config_status/config_status.h>

#include "gifs2png_gui.h"
#include <ui_gifs2png_gui.h>

gifs2png_gui::gifs2png_gui(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::gifs2png_gui)
{
    ui->setupUi(this);
    connect(&proc, SIGNAL(readyReadStandardOutput()),this, SLOT(consoleMessage()) );
}

gifs2png_gui::~gifs2png_gui()
{
    disconnect(&proc, SIGNAL(readyReadStandardOutput()),this, SLOT(consoleMessage()) );
    delete ui;
}

void gifs2png_gui::on_BrowseInput_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Source Directory"),
                                                 (ui->inputDir->text().isEmpty() ? ApplicationPath : ui->inputDir->text()),
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
    if(dir.isEmpty()) return;

    ui->inputDir->setText(dir);

}

void gifs2png_gui::on_BrowseOutput_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Target Directory"),
                                                 (ui->outputDir->text().isEmpty() ? ApplicationPath:ui->outputDir->text() ),
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
    if(dir.isEmpty()) return;

    ui->outputDir->setText(dir);
}

void gifs2png_gui::on_startTool_clicked()
{
    if(ui->inputDir->text().isEmpty())
    {
        QMessageBox::warning(this, tr("Source directory is not set"), tr("Please, set the source directory"), QMessageBox::Ok);
        return;
    }

    QString command;

    #ifdef _WIN32
    command = ApplicationPath + "/GIFs2PNG.exe";
    #else
    command = ApplicationPath + "/GIFs2PNG";
    #endif

    if(proc.state()==QProcess::Running)
        return;

    if(!QFile(command).exists())
    {
        QMessageBox::warning(this, tr("Tool not found"), tr("Can't run application:\n%1\nPlease, check the application directory and make sure it is installed properly.").arg(command), QMessageBox::Ok);
        return;
    }


    QStringList args;

    args << "--config";
    args << ConfStatus::configPath;

    if(!ui->outputDir->text().isEmpty())
    {
        args << "--output";
        args << ui->outputDir->text();
    }

    if(ui->WalkSubDirs->isChecked())
        args << "-d";

    if(ui->RemoveSource->isChecked())
        args << "-r";

    if(ui->SkipBackgrounds->isChecked())
        args << "-b";

    args << "--";
    args << ui->inputDir->text();

    DevConsole::show();
    DevConsole::log("Ready>>>", "GIFs2PNG");
    DevConsole::log("----------------------------------", "GIFs2PNG", true);
    proc.waitForFinished(1);
    proc.start(command, args);
    //connect(proc, SIGNAL(readyReadStandardError()), this, SLOT(consoleMessage()) );
}

void gifs2png_gui::consoleMessage()
{
    QByteArray strdata = proc.readAllStandardOutput();
    QString out = QString::fromUtf8(strdata);
#ifdef Q_OS_WIN
    out.remove('\r');
#endif
    DevConsole::log(out, "GIFs2PNG");
}

void gifs2png_gui::on_close_clicked()
{
    this->close();
}
