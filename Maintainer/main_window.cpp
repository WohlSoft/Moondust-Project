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

#include "main_window.h"
#include "ui_main_window.h"

#include <Music/audiocvt_sox_gui.h>
#include <Graphics/gifs2png.h>
#include <Graphics/png2gifs.h>
#include <Graphics/lazyfix.h>
#include <Graphics/palette_filter.h>
#include <EpisodeCvt/episode_converter.h>
#include <EpisodeCvt/case_fixer.h>
#ifdef PGE_ENABLE_XTCONVERT
#   include <XTConvert/xt_convert.h>
#endif
#include <pge_app_path.h>
#include <QMessageBox>

void MaintainerMain::showWIP()
{
    QMessageBox::information(this,
                             tr("Under construction"),
                             tr("This feature is under construction, "
                                "it doesn't work properly yet. It may damage some of your stuff you'll try to process. "
                                "Therefore please, make a full backup of episode that you would to process by this thing."),
                             QMessageBox::Ok);
}

MaintainerMain::MaintainerMain(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MaintainerMain)
{
    ui->setupUi(this);

    QObject::connect(&m_translator, &Translator::languageSwitched,
                     this, &MaintainerMain::languageSwitched);
    ui->language->setMenu(&m_langMenu);
    m_translator.setSettings(&m_langMenu,
                             "maintainer",
                             AppPathManager::languagesDir(),
                             AppPathManager::settingsFile());
    m_translator.initTranslator();

#ifndef PGE_ENABLE_XTCONVERT
    ui->xt_convert->hide();
#endif
}

MaintainerMain::~MaintainerMain()
{
    delete ui;
}

void MaintainerMain::languageSwitched()
{
    ui->retranslateUi(this);
}

void MaintainerMain::on_quitOut_clicked()
{
    this->close();
}

const Qt::WindowFlags g_dialogFlags =
        Qt::Window |
        Qt::WindowSystemMenuHint |
        Qt::WindowTitleHint |
        Qt::WindowCloseButtonHint |
        Qt::WindowMinimizeButtonHint;

void MaintainerMain::on_music_converter_batch_clicked()
{
    AudioCvt_Sox_gui mus(nullptr);
    mus.setWindowFlags(g_dialogFlags);
    mus.setWindowModality(Qt::NonModal);
    this->hide();
    mus.exec();
    this->show();
}

void MaintainerMain::on_episode_converter_clicked()
{
    EpisodeConverter eps(nullptr);
    eps.setWindowFlags(g_dialogFlags);
    eps.setWindowModality(Qt::NonModal);
    this->hide();
    eps.exec();
    this->show();
}

void MaintainerMain::on_gifs2png_converter_clicked()
{
    GIFs2PNG g2p(nullptr);
    g2p.setWindowFlags(g_dialogFlags);
    g2p.setWindowModality(Qt::NonModal);
    this->hide();
    g2p.exec();
    this->show();
}

void MaintainerMain::on_png2gifs_converter_clicked()
{
    PNG2GIFs p2g(nullptr);
    p2g.setWindowFlags(g_dialogFlags);
    p2g.setWindowModality(Qt::NonModal);
    this->hide();
    p2g.exec();
    this->show();
}

void MaintainerMain::on_lazyfix_tool_clicked()
{
    LazyFixTool lzf(nullptr);
    lzf.setWindowFlags(g_dialogFlags);
    lzf.setWindowModality(Qt::NonModal);
    this->hide();
    lzf.exec();
    this->show();
}

void MaintainerMain::on_palette_filter_clicked()
{
    PaletteFilter pft(nullptr);
    pft.setWindowFlags(g_dialogFlags);
    pft.setWindowModality(Qt::NonModal);
    this->hide();
    pft.exec();
    this->show();
}

void MaintainerMain::on_pathcase_fixer_clicked()
{
    showWIP();
    CaseFixer casefixer(nullptr);
    casefixer.setWindowFlags(g_dialogFlags);
    casefixer.setWindowModality(Qt::NonModal);
    this->hide();
    casefixer.exec();
    this->show();
}

#ifdef PGE_ENABLE_XTCONVERT
void MaintainerMain::on_xt_convert_clicked()
{
    XTConvertUI xt_convert(nullptr);
    xt_convert.setWindowFlags(g_dialogFlags);
    xt_convert.setWindowModality(Qt::NonModal);
    this->hide();
    xt_convert.exec();
    this->show();
}
#endif
