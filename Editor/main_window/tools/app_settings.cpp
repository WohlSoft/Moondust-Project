/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2015 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <math.h>
#include <QProcessEnvironment>
#include <QByteArray>
#ifdef Q_OS_WIN
#include <QtWin>
#include <QSysInfo>
#endif

#include <common_features/app_path.h>
#include <common_features/installer.h>
#include <common_features/logger_sets.h>
#include <common_features/logger.h>
#include <common_features/themes.h>
#include <PGE_File_Formats/file_formats.h>
#include <main_window/global_settings.h>

#include "app_settings.h"
#include <ui_app_settings.h>

g_AppSettings::g_AppSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AppSettings)
{
    ui->setupUi(this);

    #ifdef Q_OS_MAC
    this->setWindowIcon(QIcon(":/cat_builder.icns"));
    #endif
    #ifdef Q_OS_WIN
    this->setWindowIcon(QIcon(":/cat_builder.ico"));

    if(QSysInfo::WindowsVersion>=QSysInfo::WV_VISTA)
    {
        if(QtWin::isCompositionEnabled())
        {
            this->setAttribute(Qt::WA_TranslucentBackground, true);
            QtWin::extendFrameIntoClientArea(this, -1,-1,-1,-1);
            QtWin::enableBlurBehindWindow(this);
            ui->gridLayout->setMargin(0);
        }
        else
        {
            QtWin::resetExtendedFrame(this);
            setAttribute(Qt::WA_TranslucentBackground, false);
        }
    }
    #endif

    QStringList themes=Themes::availableThemes();

    ui->Theme->clear();

    while(!themes.isEmpty())
    {
        QStringList theme = themes.first().split('|');
        themes.pop_front();
        QString data = theme.size()>=1 ? theme[0] : "[untitled theme]";
        QString title = theme.size()>=2 ? theme[1] : "";
        ui->Theme->addItem(title, data);
    }
}

g_AppSettings::~g_AppSettings()
{
    delete ui;
}

void g_AppSettings::applySettings()
{
    ui->autoPlayMusic->setChecked(GlobalSettings::autoPlayMusic);
    ui->Animations->setChecked(GlobalSettings::LvlOpts.animationEnabled);
    ui->itemsLisit->setValue(GlobalSettings::animatorItemsLimit);

    ui->Collisions->setChecked(GlobalSettings::LvlOpts.collisionsEnabled);

    ui->Editor_Mid_AllowDupe->setChecked(GlobalSettings::MidMouse_allowDuplicate);
    ui->Editor_Mid_AllowPlacing->setChecked(GlobalSettings::MidMouse_allowSwitchToPlace);
    ui->Editor_Mid_AllowDrag->setChecked(GlobalSettings::MidMouse_allowSwitchToDrag);

    ui->Editor_placing_dontShowPropsBox->setChecked(GlobalSettings::Placing_dontShowPropertiesBox);

    ui->historyLimit->setValue(GlobalSettings::historyLimit);

    if(LogWriter::enabled)
        switch(LogWriter::logLevel)
        {
        case QtDebugMsg:
            ui->logLevel->setCurrentIndex(4); break;
        case QtWarningMsg:
            ui->logLevel->setCurrentIndex(3); break;
        case QtCriticalMsg:
            ui->logLevel->setCurrentIndex(2); break;
        case QtFatalMsg:
            ui->logLevel->setCurrentIndex(1); break;
        default:
            break;
        }
    else
        ui->logLevel->setCurrentIndex(0);

    ui->logFileName->setText(LogWriter::DebugLogFile);

    if(GlobalSettings::MainWindowView==QMdiArea::SubWindowView)
        ui->MView_SubWindows->setChecked(true);
    if(GlobalSettings::MainWindowView==QMdiArea::TabbedView)
        ui->MView_Tabs->setChecked(true);

    if(GlobalSettings::LVLToolboxPos == QTabWidget::West){
        ui->LVLToolboxVertical->setChecked(true);
    }else{
        ui->LVLToolboxHorizontal->setChecked(true);
    }

    if(GlobalSettings::WLDToolboxPos == QTabWidget::North){
        ui->WLDToolboxHorizontal->setChecked(true);
    }else{
        ui->WLDToolboxVertical->setChecked(true);
    }

    if(GlobalSettings::TSTToolboxPos == QTabWidget::North)
    {
        ui->TSTToolboxHorizontal->setChecked(true);
    }else{
        ui->TSTToolboxVertical->setChecked(true);
    }

    if(!GlobalSettings::currentTheme.isEmpty())
        for(int i=0; i< ui->Theme->count(); i++)
        {
            if(ui->Theme->itemData(i).toString()==GlobalSettings::currentTheme)
            {
                ui->Theme->setCurrentIndex(i);
                break;
            }
        }

    /************************Item Defaults***************************/
    switch(GlobalSettings::LvlItemDefaults.npc_direction)
    {
        case 1: ui->defaults_npc_direction_right->setChecked(true);break;
        case 0: ui->defaults_npc_direction_random->setChecked(true);break;
        case -1: default: ui->defaults_npc_direction_left->setChecked(true);break;
    }

    switch(GlobalSettings::LvlItemDefaults.npc_generator_type)
    {
        case 1:ui->defaults_npc_generator_type->setCurrentIndex(0);
        case 2:ui->defaults_npc_generator_type->setCurrentIndex(1);
    }

    ui->defaults_npc_generator_delay->setValue(((float)GlobalSettings::LvlItemDefaults.npc_generator_delay)/10.0);
    ui->defaults_warps_type->setCurrentIndex(GlobalSettings::LvlItemDefaults.warp_type);

    ui->defaults_eventtab_layviz->setChecked(GlobalSettings::LvlItemDefaults.classicevents_tabs_layviz);
    ui->defaults_eventtab_laymov->setChecked(GlobalSettings::LvlItemDefaults.classicevents_tabs_laymov);
    ui->defaults_eventtab_autoscroll->setChecked(GlobalSettings::LvlItemDefaults.classicevents_tabs_autoscroll);
    ui->defaults_eventtab_secset->setChecked(GlobalSettings::LvlItemDefaults.classicevents_tabs_secset);
    ui->defaults_eventtab_common->setChecked(GlobalSettings::LvlItemDefaults.classicevents_tabs_common);
    ui->defaults_eventtab_holdbuttons->setChecked(GlobalSettings::LvlItemDefaults.classicevents_tabs_buttons);
    ui->defaults_eventtab_trigger->setChecked(GlobalSettings::LvlItemDefaults.classicevents_tabs_trigger);
}

void g_AppSettings::on_setLogFile_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Set log file"),
        ui->logFileName->text(), tr("Text files (*.txt *.log)"));
    if (fileName.isEmpty())
        return;
    ui->logFileName->setText(fileName);
}

void g_AppSettings::on_buttonBox_accepted()
{
    GlobalSettings::autoPlayMusic = ui->autoPlayMusic->isChecked();
    GlobalSettings::animatorItemsLimit = ui->itemsLisit->value();
    GlobalSettings::LvlOpts.animationEnabled = ui->Animations->isChecked();
    GlobalSettings::LvlOpts.collisionsEnabled = ui->Collisions->isChecked();

    GlobalSettings::MidMouse_allowDuplicate = ui->Editor_Mid_AllowDupe->isChecked();
    GlobalSettings::MidMouse_allowSwitchToPlace = ui->Editor_Mid_AllowPlacing->isChecked();
    GlobalSettings::MidMouse_allowSwitchToDrag  = ui->Editor_Mid_AllowDrag->isChecked();

    GlobalSettings::Placing_dontShowPropertiesBox = ui->Editor_placing_dontShowPropsBox->isChecked();
    GlobalSettings::historyLimit = ui->historyLimit->value();

    if(ui->MView_SubWindows->isChecked())
        GlobalSettings::MainWindowView = QMdiArea::SubWindowView;
    if(ui->MView_Tabs->isChecked())
        GlobalSettings::MainWindowView = QMdiArea::TabbedView;

    if(ui->LVLToolboxVertical->isChecked())
        GlobalSettings::LVLToolboxPos = QTabWidget::West;
    else
        GlobalSettings::LVLToolboxPos = QTabWidget::North;

    if(ui->WLDToolboxHorizontal->isChecked())
        GlobalSettings::WLDToolboxPos = QTabWidget::North;
    else
        GlobalSettings::WLDToolboxPos = QTabWidget::West;

    if(ui->TSTToolboxHorizontal->isChecked())
        GlobalSettings::TSTToolboxPos = QTabWidget::North;
    else
        GlobalSettings::TSTToolboxPos = QTabWidget::West;

    Themes::loadTheme(ui->Theme->currentData().toString());
    GlobalSettings::currentTheme = ui->Theme->currentData().toString();

    bool logEnabled=true;
    switch(ui->logLevel->currentIndex())
    {
    case 4:
        LogWriter::logLevel = QtDebugMsg; break;
    case 3:
        LogWriter::logLevel = QtWarningMsg; break;
    case 2:
        LogWriter::logLevel = QtCriticalMsg; break;
    case 1:
        LogWriter::logLevel = QtFatalMsg; break;
    case 0:
    default:
        logEnabled=false;
        LogWriter::logLevel = QtFatalMsg; break;
    }

    LogWriter::DebugLogFile = ui->logFileName->text();
    LogWriter::enabled = logEnabled;


    /************************Item Defaults***************************/
    if(ui->defaults_npc_direction_right->isChecked())
        GlobalSettings::LvlItemDefaults.npc_direction=1;
    else
    if(ui->defaults_npc_direction_random->isChecked())
        GlobalSettings::LvlItemDefaults.npc_direction=0;
    else
        GlobalSettings::LvlItemDefaults.npc_direction=-1;

    GlobalSettings::LvlItemDefaults.npc_generator_type = ui->defaults_npc_generator_type->currentIndex()+1;
    GlobalSettings::LvlItemDefaults.npc_generator_delay = round(ui->defaults_npc_generator_delay->value()*10.0);
    GlobalSettings::LvlItemDefaults.warp_type = ui->defaults_warps_type->currentIndex();

    GlobalSettings::LvlItemDefaults.classicevents_tabs_layviz=ui->defaults_eventtab_layviz->isChecked();
    GlobalSettings::LvlItemDefaults.classicevents_tabs_laymov=ui->defaults_eventtab_laymov->isChecked();
    GlobalSettings::LvlItemDefaults.classicevents_tabs_autoscroll=ui->defaults_eventtab_autoscroll->isChecked();
    GlobalSettings::LvlItemDefaults.classicevents_tabs_secset=ui->defaults_eventtab_secset->isChecked();
    GlobalSettings::LvlItemDefaults.classicevents_tabs_common=ui->defaults_eventtab_common->isChecked();
    GlobalSettings::LvlItemDefaults.classicevents_tabs_buttons=ui->defaults_eventtab_holdbuttons->isChecked();
    GlobalSettings::LvlItemDefaults.classicevents_tabs_trigger=ui->defaults_eventtab_trigger->isChecked();

    accept();
}

void g_AppSettings::on_AssociateFiles_clicked()
{
    bool success = Installer::associateFiles();

    if (success)
        QMessageBox::information(this, tr("Success"), tr("All file associations have been set"), QMessageBox::Ok);
    else
        QMessageBox::warning(this, tr("Error"), QMessageBox::tr("File association failed."), QMessageBox::Ok);
}
