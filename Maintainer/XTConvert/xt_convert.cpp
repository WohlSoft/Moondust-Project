/*
 * XTConvert, a tool to package asset packs and episodes for TheXTech game engine.
 * Copyright (c) 2024 ds-sloth
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

#include "libxtconvert.h"
#include "xt_convert.h"
#include "ui_xt_convert.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <common_features/app_path.h>
#include <QtConcurrent>
#include <QStandardPaths>

#include <QtDebug>

#include "qfile_dialogs_default_options.hpp"


XTConvertUI::XTConvertUI(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::XTConvertUI)
{
    ui->setupUi(this);

    m_in_progress = false;
    m_recent_content_path = ApplicationPath;
    m_recent_output_path = ApplicationPath;

    updateControls();
}

XTConvertUI::~XTConvertUI()
{
    delete ui;
}

void XTConvertUI::updateControls()
{
    bool content_ready = !ui->content_path->text().isEmpty();
    bool output_ready = !ui->output_path->text().isEmpty();

    if(m_in_progress)
        ui->start->setText(tr("Stop"));
    else
        ui->start->setText(tr("Start"));

    ui->progress->setEnabled(m_in_progress);

    ui->browse_content->setEnabled(!m_in_progress);
    ui->target_platform->setEnabled(!m_in_progress);
    ui->browse_output->setEnabled(!m_in_progress && content_ready);

    bool convert_ready = content_ready && output_ready;
    ui->start->setEnabled(m_in_progress || convert_ready);
}

void XTConvertUI::start()
{
    logs.clear();

    m_in_progress = true;

    updateControls();

    ui->progress->setMaximum(100);
    ui->progress->setValue(-1);

    ui->progress->setFormat("Preparing...");
}

void XTConvertUI::stop(bool do_abort)
{
    if(process_thread.isRunning())
        process_thread.cancel();

    m_in_progress = false;

    ui->progress->setValue(0);
    ui->progress->setMaximum(10);

    ui->progress->setFormat("");

    updateControls();

    if(do_abort)
    {
        // if(converter.state() == QProcess::Running)
        //     converter.terminate();

        // QMessageBox::warning(this, tr("SoX error"), tr("Operation cancaled"));
    }
}

void XTConvertUI::on_start_clicked()
{
    if(m_in_progress)
        stop(true);
    else
        start();
}

void XTConvertUI::closeEvent(QCloseEvent *e)
{
    if(m_in_progress)
        e->ignore();
    else
        e->accept();
}

void XTConvertUI::on_browse_content_clicked()
{
    QString selected_filter;

    QString filter = "Episode archives and world files (*.wld *.wldx *.xte);;Compressed episode archives (*.zip *.rar *.7z);;Asset pack archives and info files (gameinfo.ini *.xta)";

    QString file = QFileDialog::getOpenFileName(this, tr("Select content to convert and package"),
                   m_recent_content_path,
                   filter, &selected_filter, c_fileDialogOptions);

    QString filelower = file.toLower();

    if(filelower.endsWith("gameinfo.ini") || filelower.endsWith(".xta"))
    {
        m_target_asset_pack = true;
        ui->content_type->setText(tr("Asset Pack"));
    }
    else if(filelower.endsWith(".zip") || filelower.endsWith(".rar") || filelower.endsWith(".7z"))
    {
        m_target_asset_pack = false;
        ui->content_type->setText(tr("Compressed episode"));
    }
    else if(filelower.endsWith(".wld") || filelower.endsWith(".wldx") || filelower.endsWith(".xte"))
    {
        m_target_asset_pack = false;
        ui->content_type->setText(tr("Episode"));
    }
    else
        return;

    m_recent_content_path = file;

    ui->content_path->setText(file);
    ui->output_path->setText("");

    updateControls();
}

void XTConvertUI::on_browse_output_clicked()
{
    QString filter;
    if(m_target_asset_pack)
        filter = "TheXTech asset pack archives (*.xta)";
    else
        filter = "TheXTech episode archives (*.xte)";

    QString out = QFileDialog::getSaveFileName(this, tr("Save a package file"),
                  m_recent_output_path,
                  filter, nullptr, c_fileDialogOptions);
    if(out.isEmpty()) return;

    QString needs_ext = (m_target_asset_pack) ? ".xta" : ".xte";

    if(!out.endsWith(needs_ext))
        out += needs_ext;

    m_recent_output_path = out;

    ui->output_path->setText(out);

    updateControls();
}
