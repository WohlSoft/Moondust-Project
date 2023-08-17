/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2023 Vitaly Novichkov <admin@wohlnet.ru>
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
#include <QtDebug>
#include <QProcess>
#include <QtConcurrent>
#include <QProgressDialog>
#include <QFutureWatcher>

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
    if(AppPathManager::userAppDir() == ApplicationPath) return;

    QDir app(ApplicationPath);
    QStringList find;
    find << "pge_editor.ini" << "*counters.ini";
    QStringList files = app.entryList(find);

    for(QString &f : files)
        QFile::copy(ApplicationPath + "/" + f, AppPathManager::userAppDir() + "/" + f);
    QSettings setup(AppPathManager::settingsFile(), QSettings::IniFormat);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    setup.setIniCodec("UTF-8");
#endif

    setup.beginGroup("logging");
    setup.remove("log-path");
    setup.endGroup();

    for(QString &f : files)
        QFile(ApplicationPath + "/" + f).remove();
}

#if defined(__linux__)
static bool xCopyFile(const QString &src, const QString &target)
{
    QFile tmp;
    tmp.setFileName(target);
    if(tmp.exists())
        tmp.remove();
    bool ret = QFile::copy(src, target);
    if(!ret)
        qWarning() << "Failed to copy file" << src << "into" << target;
    else
        qDebug() << "File " << src << "was successfully copiled into" << target;
    return ret;
}

static void xRemoveFile(const QString &target)
{
    if(QFile::exists(target))
    {
        if(QFile::remove(target))
            qDebug() << "Deleted legacy file " << target;
        else
            qWarning() << "Failed to delete file" << target;
    }
}

static bool xRunCommand(QString command, const QStringList &args)
{
    QProcess xdg;
    xdg.setProgram(command);
    xdg.setArguments(args);

    xdg.start();
    xdg.waitForFinished();

    QString printed = xdg.readAll();

    if(!printed.isEmpty())
        qDebug() << printed;

    bool ret = (xdg.exitCode() == 0);
    if(!ret)
        qWarning() << "Command" << command << "with arguments" << args << "finished with failure";
    else
        qDebug() << "Command" << command << "with arguments" << args << "successfully finished";

    return ret;
}

static bool xInstallIconResource(QString context, int iconSize, QString iconName, QString mimeName)
{
    QStringList args;
    args << "install";
    args << "--context";
    args << context;
    if(context == "apps")
        args << "--novendor";
    args << "--size";
    args << QString::number(iconSize);
    args << QDir::home().absolutePath() + "/.local/share/icons/" + iconName.arg(iconSize);
    args << mimeName;

    return xRunCommand("xdg-icon-resource", args);
}

static bool xUnInstallIconResource(QString context, int iconSize, QString iconName, QString mimeName)
{
    QStringList args;
    args << "uninstall";
    args << "--context";
    args << context;
    args << "--size";
    args << QString::number(iconSize);
    args << QDir::home().absolutePath() + "/.local/share/icons/" + iconName.arg(iconSize);
    args << mimeName;

    return xRunCommand("xdg-icon-resource", args);
}

static bool xIconSize(int iconSize)
{
    bool success = true;
    QString home = QDir::home().absolutePath();

    // Clean-up legacy resources
    xRemoveFile(QString("%1/.local/share/icons/PgeEditor-%2.png").arg(home).arg(iconSize));
    xUnInstallIconResource("apps", iconSize, "PgeEditor-%1.png", "PgeEditor");

    // Install new resources
    if(success) success = xCopyFile(QString(":/files/lvl/lvl-%1.png")
                                            .arg(iconSize),
                                            QString("%1/.local/share/icons/smbx64-level-%2.png")
                                            .arg(home).arg(iconSize));

    if(success) success = xCopyFile(QString(":/files/lvlx/lvlx-%1.png")
                                            .arg(iconSize),
                                            QString("%1/.local/share/icons/pgex-level-%2.png")
                                            .arg(home).arg(iconSize));

    if(success) success = xCopyFile(QString(":/files/wld/wld-%1.png")
                                            .arg(iconSize),
                                            QString("%1/.local/share/icons/smbx64-world-%2.png")
                                            .arg(home).arg(iconSize));

    if(success) success = xCopyFile(QString(":/files/wldx/wldx-%1.png")
                                            .arg(iconSize),
                                            QString("%1/.local/share/icons/pgex-world-%2.png")
                                            .arg(home).arg(iconSize));

    if(success) success = xCopyFile(QString(":/appicon/%1.png")
                                            .arg(iconSize),
                                            QString("%1/.local/share/icons/MoondustEditor-%2.png")
                                            .arg(home).arg(iconSize));

    if(success) success = xInstallIconResource("mimetypes", iconSize, "smbx64-level-%1.png", "x-application-smbx64-level");
    if(success) success = xInstallIconResource("mimetypes", iconSize, "smbx64-world-%1.png", "x-application-smbx64-world");
    if(success) success = xInstallIconResource("mimetypes", iconSize, "pgex-level-%1.png", "x-application-pgex-level");
    if(success) success = xInstallIconResource("mimetypes", iconSize, "pgex-world-%1.png", "x-application-pgex-world");
    if(success) success = xInstallIconResource("apps", iconSize, "MoondustEditor-%1.png", "MoondustEditor");

    return success;
}
#endif

bool Installer::associateFiles(QWidget *parent)
{
    if(!parent)
        return associateFiles_thread();
    else
    {
        QFutureWatcher<bool> watcher;
        QProgressDialog waiter(parent);
        waiter.setWindowTitle(tr("Please wait..."));
        waiter.setLabelText(tr("Please wait..."));
        waiter.setModal(true);
        waiter.setValue(0);
        waiter.setMaximum(0);
        waiter.setCancelButton(nullptr);
        waiter.show();

        QObject::connect(&watcher, &QFutureWatcher<bool>::finished, &waiter, &QProgressDialog::cancel);

        QFuture<bool> reply = QtConcurrent::run(&Installer::associateFiles_thread);
        watcher.setFuture(reply);

        waiter.exec();
        reply.waitForFinished();
        return reply.result();
    }
}

bool Installer::associateFiles_thread()
{
    bool success = true;

#if defined(_WIN32)
    //QSettings registry_hkcr("HKEY_CLASSES_ROOT", QSettings::NativeFormat);
    QSettings registry_hkcu("HKEY_CURRENT_USER", QSettings::NativeFormat);

    // add template
    //WriteToLog(QtDebugMsg, registry_hkcu.value("Software/Microsoft/Windows/CurrentVersion/Explorer/Shell Folders/Templates").toString().replace("\\","/")+QString("/sample.lvl"));
    //WriteToLog(QtDebugMsg, registry_hkcu.value("Software/Microsoft/Windows/CurrentVersion/Explorer/Shell Folders/Templates").toString().replace("\\","/")+QString("/sample.wld"));
    //registry_hkcr
    //QFile l(QProcessEnvironment::systemEnvironment().value("windir","C:\\Windows").replace("\\","/")+QString("/ShellNew/sample.lvl"));
    //QFile w(QProcessEnvironment::systemEnvironment().value("windir","C:\\Windows").replace("\\","/")+QString("/ShellNew/sample.wld"));
    QFile l(registry_hkcu.value("Software/Microsoft/Windows/CurrentVersion/Explorer/Shell Folders/Templates").toString().replace("\\", "/") + QString("/sample.lvl"));
    QFile w(registry_hkcu.value("Software/Microsoft/Windows/CurrentVersion/Explorer/Shell Folders/Templates").toString().replace("\\", "/") + QString("/sample.wld"));
    QFile lx(registry_hkcu.value("Software/Microsoft/Windows/CurrentVersion/Explorer/Shell Folders/Templates").toString().replace("\\", "/") + QString("/sample.lvlx"));
    QFile wx(registry_hkcu.value("Software/Microsoft/Windows/CurrentVersion/Explorer/Shell Folders/Templates").toString().replace("\\", "/") + QString("/sample.wldx"));

    success = l.open(QIODevice::WriteOnly);
    QString raw;
    if(success)
    {
        LevelData indata;
        FileFormats::CreateLevelData(indata);
        FileFormats::WriteSMBX64LvlFileRaw(indata, raw, 64);
        l.write(QByteArray(raw.toStdString().c_str()));
        l.close();
    }

    success = w.open(QIODevice::WriteOnly);
    if(success)
    {
        WorldData indata;
        FileFormats::CreateWorldData(indata);
        FileFormats::WriteSMBX64WldFileRaw(indata, raw, 64);
        w.write(QByteArray(raw.toStdString().c_str()));
        w.close();
    }

    success = lx.open(QIODevice::WriteOnly);
    if(success)
    {
        LevelData indata;
        FileFormats::CreateLevelData(indata);
        FileFormats::WriteExtendedLvlFileRaw(indata, raw);
        l.write(QByteArray(raw.toStdString().c_str()));
        l.close();
    }

    success = wx.open(QIODevice::WriteOnly);
    if(success)
    {
        WorldData indata;
        FileFormats::CreateWorldData(indata);
        FileFormats::WriteExtendedWldFileRaw(indata, raw);
        w.write(QByteArray(raw.toStdString().c_str()));
        w.close();
    }


    // file extension(s)
    registry_hkcu.setValue("Software/Classes/.lvlx/Default", "PGEWohlstand.Level");
    registry_hkcu.setValue("Software/Classes/.wldx/Default", "PGEWohlstand.World");
    registry_hkcu.setValue("Software/Classes/.lvl/Default", "SMBX64.Level");
    registry_hkcu.setValue("Software/Classes/.wld/Default", "SMBX64.World");

    registry_hkcu.setValue("Software/Classes/.lvlx/ShellNew/FileName", "sample.lvlx");
    registry_hkcu.setValue("Software/Classes/.wldx/ShellNew/FileName", "sample.wldx");
    registry_hkcu.setValue("Software/Classes/.lvl/ShellNew/FileName", "sample.lvl");
    registry_hkcu.setValue("Software/Classes/.wld/ShellNew/FileName", "sample.wld");
    //registry_hkcr.setValue(".lvlx/Default", "PGWWohlstand.Level");
    //registry_hkcr.setValue(".wldx/Default", "PGWWohlstand.World");


    registry_hkcu.setValue("Software/Classes/PGEWohlstand.Level/Default", tr("PGE Level file", "File Types"));
    registry_hkcu.setValue("Software/Classes/PGEWohlstand.Level/DefaultIcon/Default", "\"" + QApplication::applicationFilePath().replace("/", "\\") + "\",1");
    registry_hkcu.setValue("Software/Classes/PGEWohlstand.Level/Shell/Open/Command/Default", "\"" + QApplication::applicationFilePath().replace("/", "\\") + "\" \"%1\"");
    registry_hkcu.setValue("Software/Classes/PGEWohlstand.Level/Shell/Play level/Command/Default", "\"" + QApplication::applicationDirPath().replace("/", "\\") + "\\pge_engine.exe\" \"%1\"");

    registry_hkcu.setValue("Software/Classes/PGEWohlstand.World/Default", tr("PGE World Map", "File Types"));
    registry_hkcu.setValue("Software/Classes/PGEWohlstand.World/DefaultIcon/Default", "\"" + QApplication::applicationFilePath().replace("/", "\\") + "\",2");
    registry_hkcu.setValue("Software/Classes/PGEWohlstand.World/Shell/Open/Command/Default", "\"" + QApplication::applicationFilePath().replace("/", "\\") + "\" \"%1\"");
    registry_hkcu.setValue("Software/Classes/PGEWohlstand.World/Shell/Play episode/Command/Default", "\"" + QApplication::applicationDirPath().replace("/", "\\") + "\\pge_engine.exe\" \"%1\"");

    registry_hkcu.setValue("Software/Classes/SMBX64.Level/Default", tr("SMBX Level file", "File Types"));
    registry_hkcu.setValue("Software/Classes/SMBX64.Level/DefaultIcon/Default", "\"" + QApplication::applicationFilePath().replace("/", "\\") + "\",3");
    registry_hkcu.setValue("Software/Classes/SMBX64.Level/Shell/Open/Command/Default", "\"" + QApplication::applicationFilePath().replace("/", "\\") + "\" \"%1\"");
    registry_hkcu.setValue("Software/Classes/SMBX64.Level/Shell/Play level/Command/Default", "\"" + QApplication::applicationDirPath().replace("/", "\\") + "\\pge_engine.exe\" \"%1\"");

    registry_hkcu.setValue("Software/Classes/SMBX64.World/Default", tr("SMBX World Map", "File Types"));
    registry_hkcu.setValue("Software/Classes/SMBX64.World/DefaultIcon/Default", "\"" + QApplication::applicationFilePath().replace("/", "\\") + "\",4");
    registry_hkcu.setValue("Software/Classes/SMBX64.World/Shell/Open/Command/Default", "\"" + QApplication::applicationFilePath().replace("/", "\\") + "\" \"%1\"");
    registry_hkcu.setValue("Software/Classes/SMBX64.World/Shell/Play episode/Command/Default", "\"" + QApplication::applicationDirPath().replace("/", "\\") + "\\pge_engine.exe\" \"%1\"");

    // User variable(s)
    registry_hkcu.setValue("Environment/QT_PLUGIN_PATH", "\"" + QString(ApplicationPath).replace("/", "\\") + "\"");

#elif defined(__APPLE__)
    // only useful when other apps have taken precedence over our file extensions and you want to reset it
    //Need write correct strings for allow associations for Mac OS:

    /*
    defaults write com.apple.LaunchServices LSHandlers -array-add '{
            LSHandlerContentType = "com.apple.property-list";
            LSHandlerRoleAll = "com.apple.dt.xcode"; }'
    */

    QString x = QString("defaults write com.apple.LaunchServices LSHandlers -array-add '"
                        "<dict>"
                        "<key>LSHandlerContentTag</key>"
                        "<string>%1</string>"
                        "<key>LSHandlerContentTagClass</key>"
                        "<string>public.filename-extension</string>"
                        "<key>LSHandlerRoleAll</key>"
                        "<string>ru.wohlsoft.pge-editor</string>"
                        "</dict>"
                        "'");

    int ret = system(x.arg("lvl").toStdString().c_str());
    ret    += system(x.arg("lvlx").toStdString().c_str());
    ret    += system(x.arg("wld").toStdString().c_str());
    ret    += system(x.arg("wldx").toStdString().c_str());

    ret    += system("/System/Library/Frameworks/CoreServices.framework/Versions/A/Frameworks/LaunchServices.framework/Versions/A/Support/lsregister -kill -domain local -domain system -domain user");

    success = (ret == 0); // remove this when associator was created
#elif defined Q_OS_ANDROID
    //Is not supported yet :P
    success = false;

#elif defined(__linux__)
    QString home = QDir::home().absolutePath();

    // Here need correctly associate too
    if(success) success = QDir().mkpath(home + "/.local/share/mime/packages");
    if(success) success = QDir().mkpath(home + "/.local/share/applications");
    if(success) success = QDir().mkpath(home + "/.local/share/icons");

    // Remove legacy file
    xRemoveFile(home + "/.local/share/mime/packages/pge-project-mimeinfo.xml");
    xRemoveFile(home + "/.local/share/applications/pge_editor.desktop");

    // Copy new file
    xCopyFile(":/files/moondust-project-mimeinfo.xml",
              home + "/.local/share/mime/packages/moondust-project-mimeinfo.xml");

    if(success) success = xIconSize(16);
    if(success) success = xIconSize(32);
    if(success) success = xIconSize(48);
    if(success) success = xIconSize(256);

    QFile shortcut(":/files/moondust_editor.desktop");
    if(success) success = shortcut.open(QFile::ReadOnly | QFile::Text);
    if(success)
    {
        QTextStream shtct(&shortcut);
        QString shortcut_text = shtct.readAll();
        QFile saveAs(home + "/.local/share/applications/moondust_editor.desktop");

        if(success) success = saveAs.open(QFile::WriteOnly | QFile::Text);
        if(success) QTextStream(&saveAs) << shortcut_text.arg(ApplicationPath_x);
    }

    if(success) success = xRunCommand("xdg-mime", {"default", "pge_editor.desktop", "application/x-smbx64-level"});
    if(success) success = xRunCommand("xdg-mime", {"default", "pge_editor.desktop", "application/x-smbx64-world"});
    if(success) success = xRunCommand("xdg-mime", {"default", "pge_editor.desktop", "application/x-pgex-level"});
    if(success) success = xRunCommand("xdg-mime", {"default", "pge_editor.desktop", "application/x-pgex-world"});
    if(success) success = xRunCommand("update-desktop-database", {home + "/.local/share/applications"});
    if(success) success = xRunCommand("update-mime-database", {home + "/.local/share/mime"});
#else
    // Unsupported operating system
    success = false;

#endif

    return success;
}
