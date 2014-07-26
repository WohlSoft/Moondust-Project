/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "appsettings.h"
#include "./ui_appsettings.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <math.h>

#include "../common_features/logger_sets.h"

AppSettings::AppSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AppSettings)
{
    ui->setupUi(this);
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

    if(ui->MView_SubWindows->isChecked())
        MainWindowView = QMdiArea::SubWindowView;
    if(ui->MView_Tabs->isChecked())
        MainWindowView = QMdiArea::TabbedView;

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

    bool success = true;

    #ifdef __WIN32__
        QSettings registry_hkcr("HKEY_CLASSES_ROOT", QSettings::NativeFormat);

        // file extension(s)
        registry_hkcr.setValue(".lvl/Default", "SMBX64.Level");
        registry_hkcr.setValue(".wld/Default", "SMBX64.World");
        //registry_hkcr.setValue(".lvlx/Default", "PGWWohlstand.Level");
        //registry_hkcr.setValue(".wldx/Default", "PGWWohlstand.World");

        registry_hkcr.setValue("SMBX64.Level/Default", tr("SMBX Level file", "File Types"));
        registry_hkcr.setValue("SMBX64.Level/DefaultIcon/Default", "\"" + QApplication::applicationFilePath().replace("/", "\\") + "\",3");
        registry_hkcr.setValue("SMBX64.Level/Shell/Open/Command/Default", "\"" + QApplication::applicationFilePath().replace("/", "\\") + "\" \"%1\"");

        registry_hkcr.setValue("SMBX64.World/Default", tr("SMBX World file", "File Types"));
        registry_hkcr.setValue("SMBX64.World/DefaultIcon/Default", "\"" + QApplication::applicationFilePath().replace("/", "\\") + "\",4");
        registry_hkcr.setValue("SMBX64.World/Shell/Open/Command/Default", "\"" + QApplication::applicationFilePath().replace("/", "\\") + "\" \"%1\"");

        // User variable(s)
        QSettings registry_hkcu("HKEY_CURRENT_USER", QSettings::NativeFormat);
        registry_hkcu.setValue("Environment/QT_PLUGIN_PATH", "\"" + QApplication::applicationDirPath().replace("/", "\\") + "\"");

    #elif defined __APPLE__
        // only useful when other apps have taken precedence over our file extensions and you want to reset it
    //Need write correct strings for allow associations for Mac OS:
    /*
        system("defaults write com.apple.LaunchServices LSHandlers -array-add '<dict><key>LSHandlerContentTag</key><string>lvl</string><key>LSHandlerContentTagClass</key><string>public.filename-extension</string><key>LSHandlerRoleAll</key><string>org.pge_editor.desktop</string></dict>'");
        system("defaults write com.apple.LaunchServices LSHandlers -array-add '<dict><key>LSHandlerContentTag</key><string>wld</string><key>LSHandlerContentTagClass</key><string>public.filename-extension</string><key>LSHandlerRoleAll</key><string>org.pge_editor.desktop</string></dict>'");
        system("/System/Library/Frameworks/CoreServices.framework/Versions/A/Frameworks/LaunchServices.framework/Versions/A/Support/lsregister -kill -domain local -domain system -domain user");
     */
    success=false; // remove this when associator was created
    #else

        // Here need correctly associate too
      /*
        // this is a little silly due to all the system commands below anyway - just use mkdir -p ?  Does have the advantage of the alert I guess
        if (success) success = checkForDir(QDir::home().absolutePath() + "/.local");
        if (success) success = checkForDir(QDir::home().absolutePath() + "/.local/share");
        if (success) success = checkForDir(QDir::home().absolutePath() + "/.local/share/mime");
        if (success) success = checkForDir(QDir::home().absolutePath() + "/.local/share/mime/packages");
        if (success) success = checkForDir(QDir::home().absolutePath() + "/.local");
        if (success) success = checkForDir(QDir::home().absolutePath() + "/.local/share");
        if (success) success = checkForDir(QDir::home().absolutePath() + "/.local/share/applications");
        if (success) success = system(("cp "+datadir->absolutePath()+"/misc/hedgewars-mimeinfo.xml "+QDir::home().absolutePath()+"/.local/share/mime/packages").toLocal8Bit().constData())==0;
        if (success) success = system(("cp "+datadir->absolutePath()+"/misc/hedgewars.desktop "+QDir::home().absolutePath()+"/.local/share/applications").toLocal8Bit().constData())==0;
        if (success) success = system(("cp "+datadir->absolutePath()+"/misc/hwengine.desktop "+QDir::home().absolutePath()+"/.local/share/applications").toLocal8Bit().constData())==0;
        if (success) success = system(("update-mime-database "+QDir::home().absolutePath()+"/.local/share/mime").toLocal8Bit().constData())==0;
        if (success) success = system("xdg-mime default pge_editor.desktop application/x-smbx64-level")==0;
        if (success) success = system("xdg-mime default pge_editor.desktop application/x-smbx64-world")==0;
        // hack to add user's settings to hwengine. might be better at this point to read in the file, append it, and write it out to its new home.  This assumes no spaces in the data dir path
        if (success) success = system(("sed -i 's|^\\(Exec=.*\\) \\(%f\\)|\\1 \\2 |' "+QDir::home().absolutePath()+"/.local/share/applications/pge_editor.desktop").toLocal8Bit().constData())==0;
      */
    success=false; // remove this when associator was created
    #endif
        if (success)
            QMessageBox::information(this, tr("Success"), tr("All file associations have been set"), QMessageBox::Ok);
        else
            QMessageBox::warning(this, tr("Error"), QMessageBox::tr("File association failed."), QMessageBox::Ok);

}
