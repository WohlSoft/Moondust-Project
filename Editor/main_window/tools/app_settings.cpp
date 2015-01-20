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
#endif

#include <common_features/app_path.h>
#include <common_features/installer.h>
#include <common_features/logger_sets.h>
#include <common_features/logger.h>
#include <common_features/themes.h>
#include <file_formats/file_formats.h>

#include "app_settings.h"
#include <ui_app_settings.h>

AppSettings::AppSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AppSettings)
{
    ui->setupUi(this);

    #ifdef Q_OS_MAC
    this->setWindowIcon(QIcon(":/cat_builder.icns"));
    #endif
    #ifdef Q_OS_WIN
    this->setWindowIcon(QIcon(":/cat_builder.ico"));

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

AppSettings::~AppSettings()
{
    delete ui;
}

void AppSettings::applySettings()
{
    //set settings into GUI fields
    ui->autoPlayMusic->setChecked(autoPlayMusic);
    ui->Animations->setChecked(Animation);
    ui->itemsLisit->setValue(AnimationItemLimit);

    ui->Collisions->setChecked(Collisions);

    loglevel = LogWriter::logLevel;
    logfile = LogWriter::DebugLogFile;
    logEnabled = LogWriter::enabled;

    ui->Editor_Mid_AllowDupe->setChecked(midmouse_allowDupe);
    ui->Editor_Mid_AllowPlacing->setChecked(midmouse_allowPlace);
    ui->Editor_Mid_AllowDrag->setChecked(midmouse_allowDragMode);

    ui->Editor_placing_dontShowPropsBox->setChecked(placing_dont_show_props_box);

    if(logEnabled)
        switch(loglevel)
        {
        case QtDebugMsg:
            ui->logLevel->setCurrentIndex(4); break;
        case QtWarningMsg:
            ui->logLevel->setCurrentIndex(3); break;
        case QtCriticalMsg:
            ui->logLevel->setCurrentIndex(2); break;
        case QtFatalMsg:
            ui->logLevel->setCurrentIndex(1); break;
        }
    else
        ui->logLevel->setCurrentIndex(0);

    ui->logFileName->setText(logfile);

    if(MainWindowView==QMdiArea::SubWindowView)
        ui->MView_SubWindows->setChecked(true);
    if(MainWindowView==QMdiArea::TabbedView)
        ui->MView_Tabs->setChecked(true);

    if(LVLToolboxPos == QTabWidget::West){
        ui->LVLToolboxVertical->setChecked(true);
    }else{
        ui->LVLToolboxHorizontal->setChecked(true);
    }

    if(WLDToolboxPos == QTabWidget::North){
        ui->WLDToolboxHorizontal->setChecked(true);
    }else{
        ui->WLDToolboxVertical->setChecked(true);
    }

    if(TSTToolboxPos == QTabWidget::North)
    {
        ui->TSTToolboxHorizontal->setChecked(true);
    }else
    {
        ui->TSTToolboxVertical->setChecked(true);
    }

    if(!selectedTheme.isEmpty())
        for(int i=0; i< ui->Theme->count(); i++)
        {
            if(ui->Theme->itemData(i).toString()==selectedTheme)
            {
                ui->Theme->setCurrentIndex(i);
                break;
            }
        }
}

void AppSettings::on_setLogFile_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Set log file"),
        logfile, tr("Text files (*.txt *.log)"));
    if (fileName.isEmpty())
        return;

    logfile = fileName;
    ui->logFileName->setText(fileName);
}

void AppSettings::on_buttonBox_accepted()
{
    autoPlayMusic = ui->autoPlayMusic->isChecked();
    Animation = ui->Animations->isChecked();
    AnimationItemLimit = ui->itemsLisit->value();

    Collisions = ui->Collisions->isChecked();

    midmouse_allowDupe = ui->Editor_Mid_AllowDupe->isChecked();
    midmouse_allowPlace = ui->Editor_Mid_AllowPlacing->isChecked();
    midmouse_allowDragMode = ui->Editor_Mid_AllowDrag->isChecked();

    placing_dont_show_props_box = ui->Editor_placing_dontShowPropsBox->isChecked();

    if(ui->MView_SubWindows->isChecked())
        MainWindowView = QMdiArea::SubWindowView;
    if(ui->MView_Tabs->isChecked())
        MainWindowView = QMdiArea::TabbedView;

    if(ui->LVLToolboxVertical->isChecked())
        LVLToolboxPos = QTabWidget::West;
    else
        LVLToolboxPos = QTabWidget::North;

    if(ui->WLDToolboxHorizontal->isChecked())
        WLDToolboxPos = QTabWidget::North;
    else
        WLDToolboxPos = QTabWidget::West;

    if(ui->TSTToolboxHorizontal->isChecked())
        TSTToolboxPos = QTabWidget::North;
    else
        TSTToolboxPos = QTabWidget::West;

    Themes::loadTheme(ui->Theme->currentData().toString());
    selectedTheme = ui->Theme->currentData().toString();

    logfile = ui->logFileName->text();

    logEnabled=true;
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

    LogWriter::DebugLogFile = logfile;
    LogWriter::enabled = logEnabled;

    accept();
}

void AppSettings::on_AssociateFiles_clicked()
{
    bool success = Installer::associateFiles();

    if (success)
        QMessageBox::information(this, tr("Success"), tr("All file associations have been set"), QMessageBox::Ok);
    else
        QMessageBox::warning(this, tr("Error"), QMessageBox::tr("File association failed."), QMessageBox::Ok);
}
