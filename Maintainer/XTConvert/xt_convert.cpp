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
    if(m_target_asset_pack)
        ui->content_type->setCurrentIndex(1);
    else
        ui->content_type->setCurrentIndex(0);

    bool content_ready = !ui->content_path->text().isEmpty();
    bool output_ready = !ui->output_path->text().isEmpty();

    if(m_in_progress)
        ui->start->setText(tr("Stop"));
    else
        ui->start->setText(tr("Start"));

    ui->browse_content->setEnabled(!m_in_progress);
    ui->target_platform->setEnabled(!m_in_progress);
    ui->target_version->setEnabled(!m_in_progress);
    ui->browse_output->setEnabled(!m_in_progress && content_ready);
    ui->content_type->setEnabled(!m_in_progress && m_target_archive);

    bool convert_ready = content_ready && output_ready;
    ui->start->setEnabled(m_in_progress || convert_ready);
}

void XTConvertUI::start()
{
    logs.clear();

    m_in_progress = true;

    updateControls();

    ui->progress_big->setValue(-1);

    ui->progress_big->setFormat("Preparing...");

    ui->progress_sub->setValue(-1);

    ui->progress_sub->setFormat("");

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

    spec.target_platform = m_target_platform;
    spec.legacy_archives = m_target_legacy;

    // need to add to UI...
    spec.base_assets_path.clear();

    m_had_error = false;

    for(auto*& log : m_log_entries)
        log = nullptr;

    ui->logs->clear();

    emit do_run(spec);
}

void XTConvertUI::on_finish()
{
    m_in_progress = false;

    if(m_had_error)
    {
        ui->progress_big->setValue(0);
        ui->progress_big->setFormat(tr("Error"));
    }
    else
    {
        ui->progress_big->setValue(100);
        ui->progress_big->setFormat(tr("Finished"));
    }

    ui->progress_sub->setValue(0);
    ui->progress_sub->setFormat("");

    updateControls();
}

void XTConvertUI::on_status_update(XTConvertUpdate update)
{
    if(update.log_category == XTConvert::LogCategory::ErrorMessage)
    {
        m_had_error = true;

        if(m_had_error)
        {
            ui->progress_big->setValue(0);
            ui->progress_big->setFormat(tr("Error"));
        }

        QMessageBox::warning(this, "XTConvert", update.file_name);

        return;
    }

    if(update.log_category == XTConvert::LogCategory::Category_Count)
    {
        double progress_sub = (double)update.cur_file / update.file_count;
        double progress = (update.cur_stage + progress_sub) / update.stage_count;
        ui->progress_big->setValue((int)(progress * 100));
        ui->progress_sub->setValue((int)(progress_sub * 100));

        ui->progress_big->setFormat(update.stage_name + " (%p%)");
        ui->progress_sub->setFormat(update.file_name);

        return;
    }

    // update file table
    int cat = (int)update.log_category;
    auto*& entry = m_log_entries[cat];
    if(!entry)
    {
        entry = new QTextEdit();
        ui->logs->addTab(entry, "");
        entry->setReadOnly(true);

        int pos = ui->logs->count() - 1;

        m_log_entry_counts[cat] = 0;
        m_cat_to_position[cat] = pos;
        m_position_to_cat[pos] = cat;

        QColor color;
        switch(XTConvert::log_level[cat])
        {
        case XTConvert::LogLevel::Info:
            color = QColor(48, 48, 192);
            break;
        case XTConvert::LogLevel::Notice:
            color = QColor(32, 128, 32);
            break;
        case XTConvert::LogLevel::Warning:
        default:
            color = QColor(192, 96, 0);
            break;
        case XTConvert::LogLevel::Error:
            color = QColor(255, 32, 32);
            break;
        }

        ui->logs->tabBar()->setTabTextColor(pos, color);
    }

    entry->append(update.file_name);
    m_log_entry_counts[cat]++;

    QString tab_label = QString(XTConvert::log_category[cat]) + " (" + QString::number(m_log_entry_counts[cat]) + ")";

    ui->logs->setTabText(m_cat_to_position[cat], tab_label);

    while(m_cat_to_position[cat] > 0)
    {
        int last_cat = m_position_to_cat[m_cat_to_position[cat] - 1];

        if(XTConvert::log_level[last_cat] > XTConvert::log_level[cat])
            break;
        else if(XTConvert::log_level[last_cat] == XTConvert::log_level[cat] && m_log_entry_counts[last_cat] >= m_log_entry_counts[cat])
            break;

        // need to update positions correctly, low_cat will be in low_pos, and high_cat will be in high_pos
        int high_pos = m_cat_to_position[cat] - 1;
        int low_pos = m_cat_to_position[cat];
        ui->logs->tabBar()->moveTab(low_pos, high_pos);

        m_position_to_cat[low_pos] = last_cat;
        m_position_to_cat[high_pos] = cat;

        m_cat_to_position[last_cat]++;
        m_cat_to_position[cat]--;
    }
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

    QString filter = "Supported files (*.wld *.wldx *.xte gameinfo.ini *.xta *.zip *.rar *.7z)";

    QString file = QFileDialog::getOpenFileName(this, tr("Select content to convert and package"),
                   m_recent_content_path,
                   filter, &selected_filter, c_fileDialogOptions);

    QString filelower = file.toLower();

    if(filelower.endsWith("gameinfo.ini") || filelower.endsWith(".xta"))
    {
        m_target_asset_pack = true;
        m_target_archive = false;
    }
    else if(filelower.endsWith(".zip") || filelower.endsWith(".rar") || filelower.endsWith(".7z"))
    {
        m_target_archive = true;
    }
    else if(filelower.endsWith(".wld") || filelower.endsWith(".wldx") || filelower.endsWith(".xte"))
    {
        m_target_archive = false;
        m_target_asset_pack = false;
    }
    else
        return;

    QFileInfo fi(file);
    m_recent_content_path = fi.absolutePath();

    ui->content_path->setText(file);
    ui->output_path->setText("");

    ui->progress_big->setValue(0);
    ui->progress_big->setFormat("");

    updateControls();
}

void XTConvertUI::on_browse_output_clicked()
{
    const QString needs_ext_filter_gp(tr("TheXTech game assets pack (%1)", "XTConvert target name"));
    const QString needs_ext_filter_ep(tr("TheXTech episode (%1)", "XTConvert target name"));

    QString needs_ext;

    if(m_target_platform == XTConvert::TargetPlatform::TPL)
        needs_ext = ".wii";
    else if(m_target_platform == XTConvert::TargetPlatform::T3X)
        needs_ext = ".3ds";
    else if(m_target_platform == XTConvert::TargetPlatform::DSG)
        needs_ext = ".dsi";

    if(m_target_legacy)
    {
        if(m_target_platform == XTConvert::TargetPlatform::T3X)
            needs_ext += ".romfs";
        else
            needs_ext += ".7z";
    }
    else
    {
        if(m_target_asset_pack)
            needs_ext += ".xta";
        else
            needs_ext += ".xte";
    }

    QString filter = m_target_asset_pack ?
                        needs_ext_filter_gp.arg(QStringLiteral("*") + needs_ext) :
                        needs_ext_filter_ep.arg(QStringLiteral("*") + needs_ext);

    if(m_recent_output_path.isEmpty())
        m_recent_output_path = m_recent_content_path;

    QFileDialog dialog(this,
                       tr("Save a package file"),
                       m_recent_output_path,
                       filter);

    dialog.setDefaultSuffix(needs_ext);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setOptions(c_fileDialogOptions);

    if(!dialog.exec())
        return;

    QString out = dialog.selectedFiles().front();

    if(out.isEmpty())
        return;

    if(!out.endsWith(needs_ext))
        out.append(needs_ext); // Ensure the extension is valid

    QFileInfo fi(out);
    m_recent_output_path = fi.absolutePath();

    ui->output_path->setText(out);

    ui->progress_big->setValue(0);
    ui->progress_big->setFormat("");

    updateControls();
}

void XTConvertUI::on_content_type_currentIndexChanged(int index)
{
    if(index == 1)
        m_target_asset_pack = true;
    else
        m_target_asset_pack = false;

    if(!m_target_legacy)
        ui->output_path->setText("");

    ui->progress_big->setValue(0);
    ui->progress_big->setFormat("");

    updateControls();
}

void XTConvertUI::on_target_platform_currentIndexChanged(int index)
{
    switch(index)
    {
    case 0:
        m_target_platform = XTConvert::TargetPlatform::Desktop;
        break;
    case 1:
        m_target_platform = XTConvert::TargetPlatform::T3X;
        break;
    case 2:
        m_target_platform = XTConvert::TargetPlatform::TPL;
        break;
    case 3:
        m_target_platform = XTConvert::TargetPlatform::DSG;
        break;
    }

    ui->output_path->setText("");

    ui->progress_big->setValue(0);
    ui->progress_big->setFormat("");

    updateControls();
}

void XTConvertUI::on_target_version_currentIndexChanged(int index)
{
    if(index == 1)
        m_target_legacy = false;
    else
        m_target_legacy = true;

    ui->output_path->setText("");

    ui->progress_big->setValue(0);
    ui->progress_big->setFormat("");

    updateControls();
}
