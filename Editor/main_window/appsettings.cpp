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
#include <QProcessEnvironment>
#include <QByteArray>

#include "../common_features/app_path.h"

#include "../common_features/logger_sets.h"
#include "../file_formats/file_formats.h"
#include "../common_features/logger.h"

#include "../common_features/themes.h"

AppSettings::AppSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AppSettings)
{
    ui->setupUi(this);
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

    for(int i=0; i< ui->Theme->count(); i++)
    {
        if(ui->Theme->itemData(i).toString()==Themes::currentTheme())
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

    Themes::loadTheme(ui->Theme->currentData().toString());

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

    #ifdef _WIN32
        //QSettings registry_hkcr("HKEY_CLASSES_ROOT", QSettings::NativeFormat);
        QSettings registry_hkcu("HKEY_CURRENT_USER", QSettings::NativeFormat);

        // add template
        WriteToLog(QtDebugMsg, registry_hkcu.value("Software/Microsoft/Windows/CurrentVersion/Explorer/Shell Folders/Templates").toString().replace("\\","/")+QString("/sample.lvl"));
        WriteToLog(QtDebugMsg, registry_hkcu.value("Software/Microsoft/Windows/CurrentVersion/Explorer/Shell Folders/Templates").toString().replace("\\","/")+QString("/sample.wld"));
        //registry_hkcr
        //QFile l(QProcessEnvironment::systemEnvironment().value("windir","C:\\Windows").replace("\\","/")+QString("/ShellNew/sample.lvl"));
        //QFile w(QProcessEnvironment::systemEnvironment().value("windir","C:\\Windows").replace("\\","/")+QString("/ShellNew/sample.wld"));
        QFile l(registry_hkcu.value("Software/Microsoft/Windows/CurrentVersion/Explorer/Shell Folders/Templates").toString().replace("\\","/")+QString("/sample.lvl"));
        QFile w(registry_hkcu.value("Software/Microsoft/Windows/CurrentVersion/Explorer/Shell Folders/Templates").toString().replace("\\","/")+QString("/sample.wld"));

        success = l.open(QIODevice::WriteOnly);
        if(success){
            l.write(QByteArray(FileFormats::WriteSMBX64LvlFile(FileFormats::dummyLvlDataArray()).toStdString().c_str()));
            l.close();
        }
        success = w.open(QIODevice::WriteOnly);
        if(success){
            w.write(QByteArray(FileFormats::WriteSMBX64WldFile(FileFormats::dummyWldDataArray()).toStdString().c_str()));
            w.close();
        }


        // file extension(s)
        registry_hkcu.setValue("Software/Classes/.lvlx/Default", "PGEWohlstand.Level"); //Reserved
        registry_hkcu.setValue("Software/Classes/.wldx/Default", "PGEWohlstand.World"); //Reserved
        registry_hkcu.setValue("Software/Classes/.lvl/Default", "SMBX64.Level");
        registry_hkcu.setValue("Software/Classes/.wld/Default", "SMBX64.World");

        registry_hkcu.setValue("Software/Classes/.lvl/ShellNew/FileName", "sample.lvl");
        registry_hkcu.setValue("Software/Classes/.wld/ShellNew/FileName", "sample.wld");
        //registry_hkcr.setValue(".lvlx/Default", "PGWWohlstand.Level");
        //registry_hkcr.setValue(".wldx/Default", "PGWWohlstand.World");


        registry_hkcu.setValue("Software/Classes/PGEWohlstand.Level/Default", tr("PGE Level file", "File Types"));
        registry_hkcu.setValue("Software/Classes/PGEWohlstand.Level/DefaultIcon/Default", "\"" + QApplication::applicationFilePath().replace("/", "\\") + "\",1");
        registry_hkcu.setValue("Software/Classes/PGEWohlstand.Level/Shell/Open/Command/Default", "\"" + QApplication::applicationFilePath().replace("/", "\\") + "\" \"%1\"");

        registry_hkcu.setValue("Software/Classes/PGEWohlstand.World/Default", tr("PGE World Map", "File Types"));
        registry_hkcu.setValue("Software/Classes/PGEWohlstand.World/DefaultIcon/Default", "\"" + QApplication::applicationFilePath().replace("/", "\\") + "\",2");
        registry_hkcu.setValue("Software/Classes/PGEWohlstand.World/Shell/Open/Command/Default", "\"" + QApplication::applicationFilePath().replace("/", "\\") + "\" \"%1\"");

        registry_hkcu.setValue("Software/Classes/SMBX64.Level/Default", tr("SMBX Level file", "File Types"));
        registry_hkcu.setValue("Software/Classes/SMBX64.Level/DefaultIcon/Default", "\"" + QApplication::applicationFilePath().replace("/", "\\") + "\",3");
        registry_hkcu.setValue("Software/Classes/SMBX64.Level/Shell/Open/Command/Default", "\"" + QApplication::applicationFilePath().replace("/", "\\") + "\" \"%1\"");

        registry_hkcu.setValue("Software/Classes/SMBX64.World/Default", tr("SMBX World Map", "File Types"));
        registry_hkcu.setValue("Software/Classes/SMBX64.World/DefaultIcon/Default", "\"" + QApplication::applicationFilePath().replace("/", "\\") + "\",4");
        registry_hkcu.setValue("Software/Classes/SMBX64.World/Shell/Open/Command/Default", "\"" + QApplication::applicationFilePath().replace("/", "\\") + "\" \"%1\"");

        // User variable(s)
        registry_hkcu.setValue("Environment/QT_PLUGIN_PATH", "\"" + QString(ApplicationPath).replace("/", "\\") + "\"");

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
