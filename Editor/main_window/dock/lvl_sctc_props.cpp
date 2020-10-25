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

#include <audio/music_player.h>
#include <editing/_dialogs/musicfilelist.h>
#include <editing/_scenes/level/lvl_history_manager.h>
#include <common_features/json_settings_widget.h>
#include <main_window/dock/lvl_events_box.h>

#include <ui_mainwindow.h>
#include <mainwindow.h>

#include <QDesktopServices>
#include <QTextStream>
#include <QMutexLocker>
#include <QListView>

#include "custom_music_setup.h"
#include "lvl_sctc_props.h"
#include "ui_lvl_sctc_props.h"

LvlSectionProps::LvlSectionProps(QWidget *parent) :
    QDockWidget(parent),
    MWDock_Base(parent),
    ui(new Ui::LvlSectionProps)
{
    setVisible(false);
    setAttribute(Qt::WA_ShowWithoutActivating);
    ui->setupUi(this);

    switchResizeMode(false);

    m_externalLock = false;

    QRect mwg = mw()->geometry();
    int GOffset = 10;
    mw()->addDockWidget(Qt::RightDockWidgetArea, this);
    connect(mw(), SIGNAL(languageSwitched()), this, SLOT(re_translate()));
    setFloating(true);
    setGeometry(
        mwg.right() - width() - GOffset,
        mwg.y() + 250,
        width(),
        height()
    );

    m_extraSettingsSpacer.reset(new QSpacerItem(100, 999999, QSizePolicy::Minimum, QSizePolicy::Expanding));
    
    connect(mw()->ui->ResizingToolbar, SIGNAL(visibilityChanged(bool)),
            this, SLOT(switchResizeMode(bool)));
    connect(this, SIGNAL(visibilityChanged(bool)), mw()->ui->actionSection_Settings, SLOT(setChecked(bool)));
    connect(mw(), SIGNAL(setSMBX64Strict(bool)),
            this, SLOT(setSMBX64Strict(bool)));

    //Connect to menuitems
    connect(ui->LVLPropsWrapHorizontal, SIGNAL(toggled(bool)), mw()->ui->actionWrapHorizontal, SLOT(setChecked(bool)));
    connect(ui->LVLPropsWrapVertical, SIGNAL(toggled(bool)), mw()->ui->actionWrapVertically, SLOT(setChecked(bool)));
    connect(ui->LVLPropsOffScr, SIGNAL(toggled(bool)), mw()->ui->actionLevOffScr, SLOT(setChecked(bool)));
    connect(ui->LVLPropsNoTBack, SIGNAL(toggled(bool)), mw()->ui->actionLevNoBack, SLOT(setChecked(bool)));
    connect(ui->LVLPropsUnderWater, SIGNAL(toggled(bool)), mw()->ui->actionLevUnderW, SLOT(setChecked(bool)));

    connect(mw()->ui->actionWrapHorizontal, SIGNAL(triggered(bool)), this, SLOT(on_LVLPropsWrapHorizontal_clicked(bool)));
    connect(mw()->ui->actionWrapHorizontal, SIGNAL(triggered(bool)), ui->LVLPropsWrapHorizontal, SLOT(setChecked(bool)));
    connect(mw()->ui->actionWrapVertically, SIGNAL(triggered(bool)), this, SLOT(on_LVLPropsWrapVertical_clicked(bool)));
    connect(mw()->ui->actionWrapVertically, SIGNAL(triggered(bool)), ui->LVLPropsWrapVertical, SLOT(setChecked(bool)));
    connect(mw()->ui->actionLevOffScr, SIGNAL(triggered(bool)), this, SLOT(on_LVLPropsOffScr_clicked(bool)));
    connect(mw()->ui->actionLevOffScr, SIGNAL(triggered(bool)), ui->LVLPropsOffScr, SLOT(setChecked(bool)));
    connect(mw()->ui->actionLevNoBack, SIGNAL(triggered(bool)), this, SLOT(on_LVLPropsNoTBack_clicked(bool)));
    connect(mw()->ui->actionLevNoBack, SIGNAL(triggered(bool)), ui->LVLPropsNoTBack, SLOT(setChecked(bool)));
    connect(mw()->ui->actionLevUnderW, SIGNAL(triggered(bool)), this, SLOT(on_LVLPropsUnderWater_clicked(bool)));
    connect(mw()->ui->actionLevUnderW, SIGNAL(triggered(bool)), ui->LVLPropsUnderWater, SLOT(setChecked(bool)));

    connect(mw()->ui->actionSection_Settings, SIGNAL(triggered(bool)), this, SLOT(setVisible(bool)));

    QObject::connect(ui->imageSelector, &ImageSelector::currentItemChanged, this, &LvlSectionProps::backgroundImageChanged);

    ui->musicSetup->setVisible(false); // Hide music setup button by default

    m_lastVisibilityState = isVisible();
    mw()->docks_level.addState(this, &m_lastVisibilityState);
}


LvlSectionProps::~LvlSectionProps()
{
    if(m_extraSettings.get())
        ui->extraSettings->layout()->removeWidget(m_extraSettings.get()->getWidget());
    ui->extraSettings->layout()->removeItem(m_extraSettingsSpacer.get());
    m_extraSettings.reset();
    m_extraSettingsSpacer.reset();
    delete ui;
}

QPushButton *LvlSectionProps::getEditBackground2IniButton()
{
    return ui->editBackground2Ini;
}

void LvlSectionProps::setSMBX64Strict(bool en)
{
    DataConfig &c = mw()->configs;

    bool wrap_vertical_enabled = !en && (c.editor.supported_features.level_section_vertical_wrap == EditorSetup::FeaturesSupport::F_ENABLED);
    bool wrap_vertical_hidden = (c.editor.supported_features.level_section_vertical_wrap == EditorSetup::FeaturesSupport::F_HIDDEN);

    ui->LVLPropsWrapVertical->setEnabled(wrap_vertical_enabled);
    ui->LVLPropsWrapVertical->setHidden(wrap_vertical_hidden);
    mw()->ui->actionWrapVertically->setEnabled(wrap_vertical_enabled);
    mw()->ui->actionWrapVertically->setVisible(!wrap_vertical_hidden);

    ui->extraSettings->setEnabled(!en);
}

void LvlSectionProps::re_translate()
{
    ui->retranslateUi(this);
    initDefaults();
}

void LvlSectionProps::focusInEvent(QFocusEvent *ev)
{
    QDockWidget::focusInEvent(ev);
    //ev->accept();
    //qApp->setActiveWindow(mw());
}

void LvlSectionProps::updateExtraSettingsWidget()
{
    LevelEdit *edit = nullptr;
    QString defaultDir = mw()->configs.config_dir;

    if((mw()->activeChildWindow() == MainWindow::WND_Level) && (edit = mw()->activeLvlEditWin()))
    {
        QMutexLocker mlock(&m_mutex); Q_UNUSED(mlock)
        bool spacerNeeded = false;
        
        CustomDirManager uLVL(edit->LvlData.meta.path, edit->LvlData.meta.filename);
        uLVL.setDefaultDir(defaultDir);

        QString esLayoutFile = uLVL.getCustomFile("lvl_section.json");
        if(esLayoutFile.isEmpty())
            return;

        auto &section = edit->LvlData.sections[edit->LvlData.CurSection];

        QFile layoutFile(esLayoutFile);
        if(!layoutFile.open(QIODevice::ReadOnly))
            return;

        ui->extraSettings->setToolTip("");
        ui->extraSettings->setMinimumHeight(0);
        ui->extraSettings->setStyleSheet("");
        if(m_extraSettings.get())
            ui->extraSettings->layout()->removeWidget(m_extraSettings.get()->getWidget());
        ui->extraSettings->layout()->removeItem(m_extraSettingsSpacer.get());
        m_extraSettings.reset();

        QByteArray rawLayout = layoutFile.readAll();
        m_extraSettings.reset(new JsonSettingsWidget(ui->extraSettings));
        if(m_extraSettings.get())
        {
            m_extraSettings->setSearchDirectories(edit->LvlData.meta.path, edit->LvlData.meta.filename);
            m_extraSettings->setConfigPack(&mw()->configs);
            if(!m_extraSettings->loadLayout(section.custom_params.toUtf8(), rawLayout))
            {
                LogWarning(m_extraSettings->errorString());
                ui->extraSettings->setToolTip(tr("Error in the file %1:\n%2")
                                              .arg(esLayoutFile)
                                              .arg(m_extraSettings->errorString()));
                ui->extraSettings->setMinimumHeight(12);
                ui->extraSettings->setStyleSheet("*{background-color: #FF0000;}");
            }
            auto *widget = m_extraSettings->getWidget();
            if(widget)
            {
                widget->layout()->setContentsMargins(0, 0, 0, 0);
                ui->extraSettings->layout()->addWidget(widget);
                JsonSettingsWidget::connect(m_extraSettings.get(),
                                            &JsonSettingsWidget::settingsChanged,
                                            this,
                                            &LvlSectionProps::onExtraSettingsChanged);
                spacerNeeded = spacerNeeded || m_extraSettings->spacerNeeded();
            }
        }
        layoutFile.close();
        
        ui->extraSettings->setMinimumHeight(spacerNeeded ? 0 : 150);
        if(spacerNeeded)
            ui->extraSettings->layout()->addItem(m_extraSettingsSpacer.get());
    }
}

void LvlSectionProps::onExtraSettingsChanged()
{
    if(mw()->activeChildWindow() == MainWindow::WND_Level)
    {
        LevelEdit *edit = mw()->activeLvlEditWin();
        if(!edit)
            return;
        auto &section = edit->LvlData.sections[edit->LvlData.CurSection];
        QString custom_params = m_extraSettings->saveSettings();
        QList<QVariant> xtraSetupData;
        xtraSetupData.push_back(section.custom_params);
        xtraSetupData.push_back(custom_params);
        edit->scene->m_history->addChangeSectionSettings(edit->LvlData.CurSection,
                HistorySettings::SETTING_SEC_XTRA,
                QVariant(xtraSetupData));
        section.custom_params = custom_params;
        edit->LvlData.meta.modified = true;
    }
}

// Level Section tool box show/hide
void MainWindow::on_actionSection_Settings_triggered(bool checked)
{
    if(checked) dock_LvlSectionProps->raise();
}


// ////////////////Set LevelSection data//////////////////////////////////
void LvlSectionProps::initDefaults()
{
    m_externalLock = true;
    mw()->dock_LvlEvents->setEventToolsLocked(true);

    auto *eventsSctMus = mw()->dock_LvlEvents->cbox_sct_mus();
    auto *eventsSctBg = mw()->dock_LvlEvents->cbox_sct_bg();

    eventsSctBg->setMinimumHeight(80);

    LogDebug(QString("Set level Section Data"));
    ui->LVLPropsMusicNumber->clear();
    ui->imageSelector->clear();
    ui->imageSelector->setMinimumHeight(80);


    eventsSctMus->clear(); //Music list in events
    eventsSctBg->clear();  //Background list in events

    QPixmap empty(100, 70);
    empty.fill(QColor(Qt::black));

    ui->imageSelector->addItem(empty, tr("[No image]"), 0);

    eventsSctBg->addItem(empty, tr("[No image]"), 0);
    eventsSctBg->setItem(0);
    ui->LVLPropsMusicNumber->addItem(tr("[Silence]"), QVariant::fromValue<unsigned long>(0));
    eventsSctMus->addItem(tr("[Silence]"), QVariant::fromValue<unsigned long>(0));

    PGE_DataArray<obj_BG > &main_bg = mw()->configs.main_bg;
    for(int i = 1; i < main_bg.size(); i++)
    {
        const obj_BG &bgD = main_bg[i];
        QPixmap bgThumb(100, 70);
        bgThumb.fill(QColor(Qt::white));
        QPainter xx(&bgThumb);
        bool isCustom = false;
        QString bgTitle = bgD.setup.name;
        QPixmap tmp = bgD.image.scaledToHeight(70);

        if(mw()->activeChildWindow() == 1)
        {
            LevelEdit *edit = mw()->activeLvlEditWin();
            if(edit->scene->m_localConfigBackgrounds.contains(bgD.setup.id))
            {
                obj_BG &bgX = edit->scene->m_localConfigBackgrounds[bgD.setup.id];
                if(!bgX.image.isNull()) tmp = bgX.image.scaledToHeight(70);
                bgTitle = bgX.setup.name;
                isCustom = true;
            }
        }
        if(!tmp.isNull())
        {
            int d = 0;
            for(int i = 0; i < 100; i += tmp.width())
            {
                xx.drawPixmap(i, 0, tmp.width(), tmp.height(), tmp);
                d += tmp.width();
            }
            if(d < 100) xx.drawPixmap(d, 0, tmp.width() - (100 - d), tmp.height(), tmp);
        }
        xx.end();

        ui->imageSelector->addItem(bgThumb,
                                   QString("%2%3")
                                   .arg(isCustom ? "* " : "") //Show a star over customized background
                                   .arg(bgTitle), //Background title
                                   bgD.setup.id);

        eventsSctBg->addItem(bgThumb, bgTitle, bgD.setup.id);
    }

    PGE_DataArray<obj_music > &main_music_lvl = mw()->configs.main_music_lvl;
    for(int i = 1; i < main_music_lvl.size(); i++)
    {
        const obj_music &mus = main_music_lvl[i];
        ui->LVLPropsMusicNumber->addItem(mus.name, QString::number(mus.id));
        eventsSctMus->addItem(mus.name, QString::number(mus.id));
    }

    mw()->dock_LvlEvents->setEventToolsLocked(false);
    m_externalLock = false;

    //Set current data
    refreshFileData();
}


void LvlSectionProps::refreshFileData()
{
    m_externalLock = true;
    //Set current data
    if(mw()->activeChildWindow() == MainWindow::WND_Level)
    {
        LevelEdit *edit = mw()->activeLvlEditWin();
        if(!edit)
        {
            m_externalLock = false;
            return;
        }

        ui->LVLProp_CurSect->setText(QString::number(edit->LvlData.CurSection));

        ui->imageSelector->setItem(edit->LvlData.sections[edit->LvlData.CurSection].background);

        ui->LVLPropsMusicNumber->setCurrentIndex(0);
        for(int i = 0; i < ui->LVLPropsMusicNumber->count(); i++)
        {
            if((unsigned long)ui->LVLPropsMusicNumber->itemData(i).toInt() ==
               edit->LvlData.sections[edit->LvlData.CurSection].music_id)
            {
                ui->LVLPropsMusicNumber->setCurrentIndex(i);
                break;
            }
        }

        ui->LVLPropsWrapHorizontal->setChecked(edit->LvlData.sections[edit->LvlData.CurSection].wrap_h);
        ui->LVLPropsWrapVertical->setChecked(edit->LvlData.sections[edit->LvlData.CurSection].wrap_v);
        ui->LVLPropsOffScr->setChecked(edit->LvlData.sections[edit->LvlData.CurSection].OffScreenEn);
        ui->LVLPropsNoTBack->setChecked(edit->LvlData.sections[edit->LvlData.CurSection].lock_left_scroll);
        ui->LVLPropsUnderWater->setChecked(edit->LvlData.sections[edit->LvlData.CurSection].underwater);

        QString musFile = edit->LvlData.sections[edit->LvlData.CurSection].music_file;
        ui->LVLPropsMusicCustom->setText(musFile);
        ui->LVLPropsMusicCustomEn->setChecked((edit->LvlData.sections[edit->LvlData.CurSection].music_id == mw()->configs.music_custom_id));
        ui->musicSetup->setVisible(CustomMusicSetup::settingsNeeded(musFile));

        updateExtraSettingsWidget();

        loadMusic();
    }
    m_externalLock = false;
}





// Level Section Settings
void LvlSectionProps::on_LVLPropsWrapHorizontal_clicked(bool checked)
{
    if(mw()->activeChildWindow() == MainWindow::WND_Level)
    {
        LevelEdit *edit = mw()->activeLvlEditWin();
        if(!edit) return;
        edit->scene->m_history->addChangeSectionSettings(edit->LvlData.CurSection, HistorySettings::SETTING_SECWRAPH, QVariant(checked));
        edit->LvlData.sections[edit->LvlData.CurSection].wrap_h = checked;
        edit->LvlData.meta.modified = true;
    }
}

void LvlSectionProps::on_LVLPropsWrapVertical_clicked(bool checked)
{
    if(mw()->activeChildWindow() == MainWindow::WND_Level)
    {
        LevelEdit *edit = mw()->activeLvlEditWin();
        if(!edit) return;
        edit->scene->m_history->addChangeSectionSettings(edit->LvlData.CurSection, HistorySettings::SETTING_SECWRAPV, QVariant(checked));
        edit->LvlData.sections[edit->LvlData.CurSection].wrap_v = checked;
        edit->LvlData.meta.modified = true;
    }
}

void LvlSectionProps::on_LVLPropsOffScr_clicked(bool checked)
{
    if(mw()->activeChildWindow() == MainWindow::WND_Level)
    {
        LevelEdit *edit = mw()->activeLvlEditWin();
        if(!edit) return;
        edit->scene->m_history->addChangeSectionSettings(edit->LvlData.CurSection, HistorySettings::SETTING_SECOFFSCREENEXIT, QVariant(checked));
        edit->LvlData.sections[edit->LvlData.CurSection].OffScreenEn = checked;
        edit->LvlData.meta.modified = true;
    }
}

void LvlSectionProps::on_LVLPropsNoTBack_clicked(bool checked)
{
    if(mw()->activeChildWindow() == MainWindow::WND_Level)
    {
        LevelEdit *edit = mw()->activeLvlEditWin();
        if(!edit) return;
        edit->scene->m_history->addChangeSectionSettings(edit->LvlData.CurSection, HistorySettings::SETTING_SECNOBACK, QVariant(checked));
        edit->LvlData.sections[edit->LvlData.CurSection].lock_left_scroll = checked;
        edit->LvlData.meta.modified = true;
    }
}

void LvlSectionProps::on_LVLPropsUnderWater_clicked(bool checked)
{
    if(mw()->activeChildWindow() == MainWindow::WND_Level)
    {
        LevelEdit *edit = mw()->activeLvlEditWin();
        if(!edit) return;
        edit->scene->m_history->addChangeSectionSettings(edit->LvlData.CurSection, HistorySettings::SETTING_SECUNDERWATER, QVariant(checked));
        edit->LvlData.sections[edit->LvlData.CurSection].underwater = checked;
        edit->LvlData.meta.modified = true;
    }
}



///////////////////////////////////////Resize section/////////////////////////////////////
void LvlSectionProps::on_ResizeSection_clicked()
{
    if(mw()->activeChildWindow() == MainWindow::WND_Level)
    {
        LevelEdit *edit = mw()->activeLvlEditWin();
        if(!edit) return;
        qApp->setActiveWindow(mw());
        edit->setFocus();
        if(edit->scene->m_resizeBox == nullptr)
            edit->scene->setSectionResizer(true);
    }
}

void LvlSectionProps::on_applyResize_clicked()
{
    if(mw()->activeChildWindow() == MainWindow::WND_Level)
    {
        LevelEdit *edit = mw()->activeLvlEditWin();
        if(!edit) return;
        mw()->on_actionResizeApply_triggered();
    }
}

void LvlSectionProps::on_cancelResize_clicked()
{
    if(mw()->activeChildWindow() == MainWindow::WND_Level)
    {
        LevelEdit *edit = mw()->activeLvlEditWin();
        if(!edit) return;
        mw()->on_actionResizeCancel_triggered();
    }
}


// ////////////////////////////////////////////////////////////////////////////////
void LvlSectionProps::backgroundImageChanged(int index)
{
    if(m_externalLock) return;

    if(mw()->configs.main_bg.stored() == 0)
    {
        LogCritical(QString("Error! *.INI Configs for backgrounds not loaded"));
        return;
    }

    ui->imageSelector->setEnabled(false);

    LogDebug("Change BG to " + QString::number(index));
    if(mw()->activeChildWindow() == MainWindow::WND_Level)
    {
        LevelEdit *edit = mw()->activeLvlEditWin();
        if(!edit) return;

        QList<QVariant> backData;
        backData.push_back(edit->LvlData.sections[edit->LvlData.CurSection].background);
        backData.push_back(index);
        edit->scene->m_history->addChangeSectionSettings(edit->LvlData.CurSection, HistorySettings::SETTING_SECBACKGROUNDIMG, QVariant(backData));
        edit->scene->ChangeSectionBG(index);
        edit->LvlData.meta.modified = true;
    }

    ui->imageSelector->setEnabled(true);
}

void LvlSectionProps::on_editBackground2Ini_clicked()
{
    if(mw()->activeChildWindow() != MainWindow::WND_Level)
        return;

    LevelEdit *edit = mw()->activeLvlEditWin();
    if(!edit)
        return;

    if(edit->isUntitled())
    {
        QMessageBox::information(this, tr("Please, save file"), tr("Please, save file first, if you want to manage custom background config files."), QMessageBox::Ok);
        return;
    }

    int backgroundId = ui->imageSelector->currentItem();
    if(backgroundId <= 0)
    {
        QMessageBox::information(this,
                                 tr("Choose a background first"),
                                 tr("Please, choose the background image first."),
                                 QMessageBox::Ok);
        return;
    }

    QString fileName = QString("background2-%3.ini").arg(backgroundId);
    QString dirPath  = QString("%1/%2")
                       .arg(edit->LvlData.meta.path)
                       .arg(edit->LvlData.meta.filename);
    QString fullPath = QString("%1/%2")
                       .arg(dirPath)
                       .arg(fileName);

    if(!QFile::exists(fullPath))
    {
        QDir dir;
        dir.mkpath(dirPath);
        QFile f(fullPath);
        f.open(QIODevice::WriteOnly | QIODevice::Truncate);

        PGE_DataArray<obj_BG > &main_bg = mw()->configs.main_bg;
        if(backgroundId < main_bg.size())
        {
            const obj_BG &bg = main_bg[backgroundId];
            QTextStream o(&f);
            o << "[background2]\r\n"

              << "; " << tr("Name that will appear in the editor",
                            "A comment in the template of Background2 INI file.") << "\r\n"
              << QString("name = \"%1\"\r\n").arg(bg.setup.name)

              << "; " << tr("Backdrop fill color",
                            "A comment in the template of Background2 INI file.") << "\r\n"
              << QString("fill-color = black\r\n")

              << "\r\n\r\n"
              << "; " << tr("Add layers here, for example:",
                            "A comment in the template of Background2 INI file.") << "\r\n\r\n"
              << "; [Layer1]\r\n"
              << "; " << "depth = INFINITE" << "\r\n"
              << "; " << "img = \"background.png\"" << "\r\n"
              << "; " << "repeatX = true" << "\r\n";
        }
    }

    QDesktopServices::openUrl(QUrl::fromLocalFile(fullPath));
}

void LvlSectionProps::switchResizeMode(bool mode)
{
    ui->applyResize->setVisible(mode);
    ui->cancelResize->setVisible(mode);
    ui->ResizeSection->setVisible(!mode);
}

void LvlSectionProps::loadMusic()
{
    if(mw()->activeChildWindow() == MainWindow::WND_Level)
    {
        LevelEdit *edit = mw()->activeLvlEditWin();
        if(!edit)
            return;

        LvlMusPlay::setMusic(mw(), LvlMusPlay::LevelMusic,
                             edit->LvlData.sections[edit->LvlData.CurSection].music_id,
                             edit->LvlData.sections[edit->LvlData.CurSection].music_file);
    }
    mw()->setMusic();
}


void LvlSectionProps::on_LVLPropsMusicNumber_currentIndexChanged(int index)
{
    if(m_externalLock) return;

    unsigned int test = index;
    ui->LVLPropsMusicCustomEn->setChecked(test == mw()->configs.music_custom_id);

    if(mw()->activeChildWindow() == MainWindow::WND_Level)
    {
        LevelEdit *edit = mw()->activeLvlEditWin();
        if(!edit) return;

        QList<QVariant> musicData;
        musicData.push_back(edit->LvlData.sections[edit->LvlData.CurSection].music_id);
        musicData.push_back(ui->LVLPropsMusicNumber->currentIndex());
        edit->scene->m_history->addChangeSectionSettings(edit->LvlData.CurSection, HistorySettings::SETTING_SECMUSIC, QVariant(musicData));
        edit->LvlData.sections[edit->LvlData.CurSection].music_id = ui->LVLPropsMusicNumber->currentIndex();
        if(ui->LVLPropsMusicNumber->hasFocus()) edit->LvlData.meta.modified = true;
    }
    loadMusic();
}

void LvlSectionProps::on_LVLPropsMusicCustomEn_toggled(bool checked)
{
    if(m_externalLock) return;

    if(ui->LVLPropsMusicCustomEn->hasFocus())
    {
        if(checked)
        {
            ui->LVLPropsMusicNumber->setCurrentIndex(mw()->configs.music_custom_id);
            if(mw()->activeChildWindow() == MainWindow::WND_Level)
            {
                LevelEdit *edit = mw()->activeLvlEditWin();
                if(!edit) return;

                QList<QVariant> musicData;
                musicData.push_back(edit->LvlData.sections[edit->LvlData.CurSection].music_id);
                musicData.push_back(ui->LVLPropsMusicNumber->currentIndex());
                edit->scene->m_history->addChangeSectionSettings(edit->LvlData.CurSection, HistorySettings::SETTING_SECMUSIC, QVariant(musicData));
                edit->LvlData.sections[edit->LvlData.CurSection].music_id = ui->LVLPropsMusicNumber->currentIndex();
                edit->LvlData.meta.modified = true;
            }
        }
    }
}

void LvlSectionProps::on_LVLPropsMusicCustomBrowse_clicked()
{
    QString dirPath;
    if(mw()->activeChildWindow() != MainWindow::WND_Level)
        return;

    LevelEdit *edit = mw()->activeLvlEditWin();
    if(!edit) return;

    dirPath = edit->LvlData.meta.path;

    if(edit->isUntitled())
    {
        QMessageBox::information(this, tr("Please, save file"), tr("Please, save file first, if you want to select custom music file."), QMessageBox::Ok);
        return;
    }

    MusicFileList musicList(dirPath, ui->LVLPropsMusicCustom->text());
    if(musicList.exec() == QDialog::Accepted)
    {
        ui->LVLPropsMusicCustom->setText(musicList.currentFile());
        ui->LVLPropsMusicCustom->setModified(true);
        on_LVLPropsMusicCustom_editingFinished();
    }
}


void LvlSectionProps::on_LVLPropsMusicCustom_editingFinished()//_textChanged(const QString &arg1)
{
    if(m_externalLock) return;
    if(!ui->LVLPropsMusicCustom->isModified()) return;
    ui->LVLPropsMusicCustom->setModified(false);

    ui->musicSetup->setVisible(CustomMusicSetup::settingsNeeded(ui->LVLPropsMusicCustom->text()));

    if(mw()->activeChildWindow() == MainWindow::WND_Level)
    {
        LevelEdit *edit = mw()->activeLvlEditWin();
        if(!edit) return;
        QString arg1 = ui->LVLPropsMusicCustom->text();

        QList<QVariant> cusMusicData;
        cusMusicData.push_back(edit->LvlData.sections[edit->LvlData.CurSection].music_file);
        cusMusicData.push_back(arg1.simplified().remove('\"'));
        edit->scene->m_history->addChangeSectionSettings(edit->LvlData.CurSection, HistorySettings::SETTING_SECCUSTOMMUSIC, QVariant(cusMusicData));
        edit->LvlData.sections[edit->LvlData.CurSection].music_file = arg1.simplified().remove('\"');
    }

    loadMusic();
}

void LvlSectionProps::on_musicSetup_clicked()
{
    if(mw()->activeChildWindow() != MainWindow::WND_Level)
        return;

    LevelEdit *edit = mw()->activeLvlEditWin();
    if(!edit)
        return;

    CustomMusicSetup set(this);
    set.initLists();

    QString musicPath = ui->LVLPropsMusicCustom->text();
    set.setMusicPath(musicPath);

    QObject::connect(&set, &CustomMusicSetup::musicSetupChanged, [&musicPath](const QString &music)->void
    {
        musicPath = music;
    });

    QObject::connect(&set, &CustomMusicSetup::updateSongPlay, [this,edit, &musicPath]()->void
    {
        LvlMusPlay::setMusic(mw(), LvlMusPlay::LevelMusic,
                             edit->LvlData.sections[edit->LvlData.CurSection].music_id,
                             musicPath);
        mw()->setMusic();
    });

    QObject::connect(&set, &CustomMusicSetup::updateSongTempo, [](double tempo)->void
    {
        LvlMusPlay::setTempo(tempo);
    });

    int ret = set.exec();

    if(ret == QDialog::Accepted)
    {
        // Apply the new music state if changed
        if(ui->LVLPropsMusicCustom->text() != musicPath)
        {
            ui->LVLPropsMusicCustom->setText(musicPath);
            ui->LVLPropsMusicCustom->setModified(true);
            on_LVLPropsMusicCustom_editingFinished();
        }
    }
    else
    {
        // Restore current music state
        loadMusic();
    }
}
