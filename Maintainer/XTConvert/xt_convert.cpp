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
#include <QStandardPaths>

#include <QtDebug>

#include "qfile_dialogs_default_options.hpp"

static void log_file_callback_ext(void* userdata, XTConvert::LogCategory log_category, const std::string& filename)
{
    ((XTConvert_Worker*)userdata)->log_file_callback(log_category, filename);
}

static int progress_callback_ext(void* userdata, int cur_stage, int stage_count, const std::string& stage_name, int cur_file, int file_count, const std::string& file_name)
{
    return ((XTConvert_Worker*)userdata)->progress_callback(cur_stage, stage_count, stage_name, cur_file, file_count, file_name);
}

void XTConvert_Worker::log_file_callback(XTConvert::LogCategory log_category, const std::string& filename)
{
    XTConvertUpdate res;
    res.log_category = log_category;
    res.file_name = QString::fromStdString(filename);

    qInfo() << res.file_name;

    emit status_update(res);
}

int XTConvert_Worker::progress_callback(int cur_stage, int stage_count, const std::string& stage_name, int cur_file, int file_count, const std::string& file_name)
{
    XTConvertUpdate res;

    res.cur_stage = cur_stage;
    res.stage_count = stage_count;
    res.stage_name = QString::fromStdString(stage_name);
    res.cur_file = cur_file;
    res.file_count = file_count;
    res.file_name = QString::fromStdString(file_name);

    emit status_update(res);

    QEventLoop loop;
    loop.processEvents();

    if(is_cancelled)
        return -1;

    return 0;
}

void XTConvert_Worker::do_cancel()
{
    if(is_running)
        is_cancelled = true;
    else
        emit finish();
}

void XTConvert_Worker::do_run(XTConvert::Spec spec)
{
    if(is_running)
        return;

    is_running = true;
    is_cancelled = false;

    spec.log_file_callback = log_file_callback_ext;
    spec.progress_callback = progress_callback_ext;
    spec.callback_userdata = this;

    XTConvert::Convert(spec);

    emit finish();

    is_running = false;
}

XTConvertUI::XTConvertUI(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::XTConvertUI)
{
    ui->setupUi(this);

    auto worker_p = new XTConvert_Worker();
    worker.reset(worker_p);

    qRegisterMetaType<XTConvert::Spec>();
    qRegisterMetaType<XTConvertUpdate>();

    connect(this, &XTConvertUI::do_run, worker_p, &XTConvert_Worker::do_run);
    connect(this, &XTConvertUI::do_cancel, worker_p, &XTConvert_Worker::do_cancel);

    connect(worker_p, &XTConvert_Worker::status_update, this, &XTConvertUI::on_status_update);
    connect(worker_p, &XTConvert_Worker::finish, this, &XTConvertUI::on_finish);

    worker->moveToThread(&process_thread);
    process_thread.start();

    m_in_progress = false;
    m_recent_content_path = ApplicationPath;
    m_recent_output_path = ApplicationPath;

    updateControls();
}

XTConvertUI::~XTConvertUI()
{
    delete ui;

    process_thread.quit();
    process_thread.wait();

    worker.reset();
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

    XTConvert::Spec spec;

    QString content_path = ui->content_path->text();

    if(content_path.endsWith("gameinfo.ini") || content_path.endsWith(".wld") || content_path.endsWith(".wldx"))
        content_path = QFileInfo(content_path).dir().absolutePath();

    spec.input_path = content_path.toStdString();
    spec.destination = ui->output_path->text().toStdString();

    if(m_target_asset_pack)
        spec.package_type = XTConvert::PackageType::AssetPack;
    else
        spec.package_type = XTConvert::PackageType::Episode;

    if(ui->target_platform->currentText() == "Mainline")
        spec.target_platform = XTConvert::TargetPlatform::Desktop;
    else if(ui->target_platform->currentText() == "3DS")
        spec.target_platform = XTConvert::TargetPlatform::T3X;
    else if(ui->target_platform->currentText() == "Wii")
        spec.target_platform = XTConvert::TargetPlatform::TPL;
    else if(ui->target_platform->currentText() == "DSi")
        spec.target_platform = XTConvert::TargetPlatform::DSG;

    // need to add to UI, and update default filenames...
    spec.legacy_archives = true;

    // need to add to UI...
    spec.base_assets_path.clear();

    emit do_run(spec);
}

void XTConvertUI::on_finish()
{
    m_in_progress = false;

    ui->progress->setValue(0);
    ui->progress->setMaximum(10);

    ui->progress->setFormat("");

    updateControls();
}

void XTConvertUI::on_status_update(XTConvertUpdate update)
{
    if(update.log_category == XTConvert::LogCategory::ErrorMessage)
    {
        QMessageBox::warning(this, "XTConvert", update.file_name);
        return;
    }

    if(update.log_category == XTConvert::LogCategory::Category_Count)
    {
        double progress = (double)update.cur_stage / update.stage_count + (double)update.cur_file / (update.file_count * update.stage_count);
        ui->progress->setValue((int)(progress * 100));

        QString format = update.stage_name;
        if(!update.file_name.isEmpty())
        {
            format += " ";
            format += update.file_name;
        }

        ui->progress->setFormat(format);

        return;
    }

    // update file table
    ui->textBrowser_3->append(XTConvert::log_category[(int)update.log_category] + (" - " + update.file_name));
}

void XTConvertUI::on_start_clicked()
{
    if(m_in_progress)
        emit do_cancel();
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
