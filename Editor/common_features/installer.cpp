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

#include <QString>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QSettings>
#include <QApplication>

#include <PGE_File_Formats/file_formats.h>

#include "app_path.h"
#include "installer.h"

Installer::Installer(QObject *parent):
    QObject(parent)
{}

Installer::~Installer()
{}

void Installer::moveFromAppToUser()
{
    if(AppPathManager::userAppDir()==ApplicationPath) return;

    QDir app(ApplicationPath);
    QStringList find;
    find << "pge_editor.ini" << "*counters.ini";
    QStringList files = app.entryList(find);

    foreach(QString f, files)
    {
        QFile::copy(ApplicationPath+"/"+f, AppPathManager::userAppDir()+"/"+f);
    }
    QSettings setup(AppPathManager::settingsFile(), QSettings::IniFormat);
    setup.beginGroup("logging");
    setup.remove("log-path");
    setup.endGroup();

    foreach(QString f, files)
    {
        QFile(ApplicationPath+"/"+f).remove();
    }
}

bool Installer::associateFiles()
{
    bool success = true;

    #ifdef _WIN32
        //QSettings registry_hkcr("HKEY_CLASSES_ROOT", QSettings::NativeFormat);
        QSettings registry_hkcu("HKEY_CURRENT_USER", QSettings::NativeFormat);

        // add template
        //WriteToLog(QtDebugMsg, registry_hkcu.value("Software/Microsoft/Windows/CurrentVersion/Explorer/Shell Folders/Templates").toString().replace("\\","/")+QString("/sample.lvl"));
        //WriteToLog(QtDebugMsg, registry_hkcu.value("Software/Microsoft/Windows/CurrentVersion/Explorer/Shell Folders/Templates").toString().replace("\\","/")+QString("/sample.wld"));
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
    #elif defined Q_OS_ANDROID
    //Is not supported yet :P
    success=false;
    #else

        // Here need correctly associate too
        if (success) success = QDir().mkpath(QDir::home().absolutePath() + "/.local/share/mime/packages");
        if (success) success = QDir().mkpath(QDir::home().absolutePath() + "/.local/share/applications");
        if (success) success = QDir().mkpath(QDir::home().absolutePath() + "/.local/share/icons");

        QFile tmp; Q_UNUSED(tmp);
        #define XcopyFile(src, target) tmp.setFileName(target); if(tmp.exists()) tmp.remove();\
                                       QFile::copy(src, target);

        XcopyFile(":/_files/_files/pge-project-mimeinfo.xml", QDir::home().absolutePath()+"/.local/share/mime/packages/pge-project-mimeinfo.xml");

        #define IconSize(Size) \
        XcopyFile(":/_files/_files/file_lvl/file_lvl_" Size ".png", QDir::home().absolutePath()+"/.local/share/icons/smbx64-level-" Size ".png");\
        XcopyFile(":/_files/_files/file_lvlx/file_lvlx_" Size ".png", QDir::home().absolutePath()+"/.local/share/icons/pgex-level-" Size ".png");\
        XcopyFile(":/_files/_files/file_wld/file_wld_" Size ".png", QDir::home().absolutePath()+"/.local/share/icons/smbx64-world-" Size ".png");\
        XcopyFile(":/_files/_files/file_wldx/file_wldx_" Size ".png", QDir::home().absolutePath()+"/.local/share/icons/pgex-world-" Size ".png");\
        XcopyFile(":/images/cat_builder/cat_builder_" Size ".png", QDir::home().absolutePath()+"/.local/share/icons/PgeEditor-" Size ".png");\
        if (success) success = system( QString("xdg-icon-resource install --context mimetypes --size " Size " "+QDir::home().absolutePath()+"/.local/share/icons/smbx64-level-" Size ".png x-application-smbx64-level").toLocal8Bit().constData())==0;\
        if (success) success = system( QString("xdg-icon-resource install --context mimetypes --size " Size " "+QDir::home().absolutePath()+"/.local/share/icons/smbx64-world-" Size ".png x-application-smbx64-world").toLocal8Bit().constData())==0;\
        if (success) success = system( QString("xdg-icon-resource install --context mimetypes --size " Size " "+QDir::home().absolutePath()+"/.local/share/icons/pgex-level-" Size ".png x-application-pgex-level").toLocal8Bit().constData())==0;\
        if (success) success = system( QString("xdg-icon-resource install --context mimetypes --size " Size " "+QDir::home().absolutePath()+"/.local/share/icons/pgex-world-" Size ".png x-application-pgex-world").toLocal8Bit().constData())==0;\
        if (success) success = system( QString("xdg-icon-resource install --context apps --novendor --size " Size " "+QDir::home().absolutePath()+"/.local/share/icons/PgeEditor-" Size ".png PgeEditor").toLocal8Bit().constData())==0;

        IconSize("16");
        IconSize("32");
        IconSize("48");
        IconSize("256");

        QFile shortcut(":/_files/_files/pge_editor.desktop");
        if (success) success = shortcut.open(QFile::ReadOnly|QFile::Text);
        if (success)
        {
            QTextStream shtct(&shortcut);
            QString shortcut_text=shtct.readAll();
            QFile saveAs(QDir::home().absolutePath()+"/.local/share/applications/pge_editor.desktop");

            if (success) success = saveAs.open(QFile::WriteOnly|QFile::Text);
            if (success) QTextStream(&saveAs)<<shortcut_text.arg(ApplicationPath_x);
        }

        if (success) success = system("xdg-mime default pge_editor.desktop application/x-smbx64-level")==0;
        if (success) success = system("xdg-mime default pge_editor.desktop application/x-smbx64-world")==0;
        if (success) success = system("xdg-mime default pge_editor.desktop application/x-pgex-level")==0;
        if (success) success = system("xdg-mime default pge_editor.desktop application/x-pgex-world")==0;
        if (success) success = system( QString("update-desktop-database "+QDir::home().absolutePath()+"/.local/share/applications").toLocal8Bit().constData())==0;
        if (success) success = system( QString("update-mime-database "+QDir::home().absolutePath()+"/.local/share/mime").toLocal8Bit().constData())==0;

    #endif
    return success;
}
