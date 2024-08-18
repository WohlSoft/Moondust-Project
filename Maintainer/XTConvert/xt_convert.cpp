/*
 * XTConvert, a free tool for packaging asset packs and episodes for TheXTech game engine.
 *
 * This is a part of the Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2017-2024 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "xt_convert.h"
#include "ui_xt_convert.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <common_features/app_path.h>
#include <PGE_File_Formats/file_formats.h>
#include <QDirIterator>
#include <QtConcurrent>
#include <QStandardPaths>

#include <QtDebug>

#include "qfile_dialogs_default_options.hpp"


static QString strTail(QString str, int len)
{
    if(len <= 0)
        return "";
    if(str.size() <= len)
        return str;
    return "..." + str.mid(str.size() - (len + 1), -1);
}


XTConvert::XTConvert(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::XTConvert)
{
    ui->setupUi(this);

    connect(&converter, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(nextStep(int, QProcess::ExitStatus)));
    connect(&converter, SIGNAL(readyRead()), this, SLOT(consoleMessage()));
    converter.setTextModeEnabled(true);
    converter.setProcessChannelMode(QProcess::MergedChannels);

    connect(this, SIGNAL(doNextStep(int, int)), this, SLOT(nextStep_x(int, int)));
    connect(this, SIGNAL(gotMax(int)), ui->progress, SLOT(setMaximum(int)));

    inWork = false;
    isBackUp = false;
    madeJob = 0;
}

XTConvert::~XTConvert()
{
    delete ui;
}

void XTConvert::resetStat()
{
    stat_converted_files = 0;
    stat_overwritten_levels = 0;
    stat_overwritten_worlds = 0;
    stat_failed_levels = 0;
    stat_failed_files = 0;
    fails.clear();
}

void XTConvert::setEnableControls()
{
    // TODO: enable controls according to the current engine state
}

void XTConvert::start()
{
    QMessageBox::warning(this, tr("SoX error"), tr("SoX binary path is wrong.\nPlease set SoX path first"));
    return;

    resetStat();

    bool execFetchEpisodeData = false;

    ui->start->setText(tr("Stop"));
    setEnableControls();

    ui->progress->setMaximum(filesToConvert.size() * 100);
    ui->progress->setValue(-1);
    lastOutput.clear();
    inWork = true;
    madeJob = 0;

    ui->status->setText("Preparing...");

    if(execFetchEpisodeData)
    {
        // fetcher = QtConcurrent::run(this, &XTConvert::CollectEpisodeData);
        // ui->status->setText("Looking for a musics and level/world files...");
    }
    else
        nextStep(0, QProcess::NormalExit);
}

void XTConvert::stop(bool do_abort)
{
    if(fetcher.isRunning())
        fetcher.cancel();
    ui->start->setText(tr("Start"));
    ui->progress->setValue(0);
    ui->progress->setMaximum(10);
    setEnableControls();
    current_musFileNew.clear();
    current_musFileOld.clear();
    filesToConvert.clear();
    inWork = false;
    ui->status->setText("");

    //Restore backUP
    if(do_abort)
    {
        if(converter.state() == QProcess::Running)
            converter.terminate();

        if(isBackUp)
        {
            if(QFile(current_musFileOld).exists())
            {
                if(QFile(current_musFileNew).exists())
                    QFile(current_musFileNew).remove();
                QFile(current_musFileOld).rename(current_musFileNew);
            }
        }
        else
        {
            if(QFile(current_musFileNew).exists())
                QFile(current_musFileNew).remove();
        }

        QMessageBox::warning(this, tr("SoX error"), tr("Operation cancaled"));
    }
}

void XTConvert::nextStep_x(int retStatus, int exitStatus)
{
    nextStep(retStatus, (QProcess::ExitStatus)exitStatus);
}

void XTConvert::nextStep(int retStatus, QProcess::ExitStatus exitStatus)
{
    if(!inWork) return;
}

static QString leftBuffer;
void XTConvert::consoleMessage()
{
    QByteArray strdata = converter.readAllStandardOutput();
    int made = 0;
    bool found = false;
    qDebug() << "OUTPUT: " << strdata;
    QStringList acceptedData = QString::fromUtf8(strdata).split('\r');
    foreach(QString entry, acceptedData)
    {
        entry.remove('\n');
        qDebug() << "ENTRY" << entry;
        if(!leftBuffer.isEmpty())
        {
            entry = leftBuffer + entry;
            leftBuffer.clear();
        }
        if(entry.size() < 9)
        {
            leftBuffer = entry;
            continue;
        }
        if(entry.startsWith("In:"))
        {
            made = qRound(entry.mid(3, 6).trimmed().remove('%').toDouble());
            found = true;
            qDebug() << "PROGRESS: " << made;
            break;
        }
    }

    if(found)
    {
        qDebug() << "APPLIED: " << ((madeJob - 1) * 100 + made);
        ui->progress->setValue((madeJob - 1) * 100 + made);
    }
    //    lastOutput.append(strdata);
    //    lastOutput.append("\n");
    //    //DevConsole::log(strdata, "SoX");
}

void XTConvert::on_start_clicked()
{
    if(inWork)
        stop(true);
    else
        start();
}

void XTConvert::closeEvent(QCloseEvent *e)
{
    if(inWork)
        e->ignore();
    else
        e->accept();
}

void XTConvert::on_browse_asset_pack_clicked()
{
    QString filter;
    filter = "Asset pack archives and info files (gameinfo.ini *.xta)";
    QString file = QFileDialog::getOpenFileName(this, tr("Selecting a level file to convert custom music"),
                   (ui->asset_pack_path->text().isEmpty() ? ApplicationPath : ui->asset_pack_path->text()),
                   filter, nullptr, c_fileDialogOptions);
    if(file.isEmpty()) return;

    ui->asset_pack_path->setText(file);
}

void XTConvert::on_browse_episode_clicked()
{
    QString filter;
    filter = "Episode archives and world files (*.wld *.wldx *.xte)";
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open episode path to convert entire music set"),
                  (ui->episode_path->text().isEmpty() ? ApplicationPath : ui->episode_path->text()), c_dirDialogOptions);
    if(dir.isEmpty()) return;

    ui->episode_path->setText(dir);
}
