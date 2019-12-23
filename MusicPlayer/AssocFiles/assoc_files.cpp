#include "assoc_files.h"
#include "ui_assoc_files.h"
#include <QListWidgetItem>
#include <QList>
#include <QPair>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QSettings>
#include <QProcess>
#include <QMessageBox>
#include <QtDebug>

AssocFiles::AssocFiles(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AssocFiles)
{
    ui->setupUi(this);
    typedef QPair<QString, QString> tentry;
    QList<tentry> formats;
    formats << tentry("wav",  "WAV - Uncompressed PCM Audio");
    formats << tentry("voc",  "VOC - Creative Labs Audio File");
    formats << tentry("mp3",  "MP3 - MPEG-1/2/2.5 Layer 3, Lossy data compressed audio");
    formats << tentry("ogg",  "OGG - OGG Vorbis, Lossy data compressed audio");
    formats << tentry("flac", "FLAC - Free Lossless Audio Codec, Loss-less compressed");
    formats << tentry("mid",  "mid - Music Instrument Digital Interface, commands list");
    formats << tentry("midi", "midi - Music Instrument Digital Interface, commands list");
    formats << tentry("kar",  "kar - Music Instrument Digital Interface, commands list");
    formats << tentry("rmi",  "rmi - Music Instrument Digital Interface, commands list");
    formats << tentry("xmi",  "XMI - AIL eXtended MIDI file");
    formats << tentry("mus",  "MUS - DMX MIDI Music file");
    formats << tentry("cmf",  "CMF - Creative Music File (mixed MIDI and OPL2 synth)");

    formats << tentry("ay",   "AY - ZX Spectrum/Amstrad CPC");
    formats << tentry("gbs",  "GBS - Nintendo Game Boy");
    formats << tentry("gym",  "GYM - Sega Genesis/Mega Drive");
    formats << tentry("hes",  "HES - NEC TurboGrafx-16/PC Engine");
    formats << tentry("kss",  "KSS - MSX Home Computer/other Z80 systems (doesn't support FM sound)");
    formats << tentry("nsf",  "NSF - Nintendo NES/Famicom (with VRC 6, Namco 106, and FME-7 sound)");
    formats << tentry("nsfe", "NSFE - Nintendo NES/Famicom (with VRC 6, Namco 106, and FME-7 sound)");
    formats << tentry("sap",  "SAP - Atari systems using POKEY sound chip");
    formats << tentry("spc",  "SPC - Super Nintendo/Super Famicom");
    formats << tentry("vgm",  "VGM - Sega Master System/Mark III, Sega Genesis/Mega Drive,BBC Micro");
    formats << tentry("vgz",  "VGZ - Sega Master System/Mark III, Sega Genesis/Mega Drive,BBC Micro (GZ Compressed)");

    formats << tentry("669",  "669 - Composer 669, Unis 669");
    formats << tentry("amf",  "AMF - ASYLUM Music Format V1.0/DSMI Advanced Module Format");
    formats << tentry("apun", "APUN - APlayer");
    formats << tentry("dsm",  "DSM - DSIK internal format");
    formats << tentry("far",  "FAR - Farandole Composer");
    formats << tentry("gdm",  "GDM - General DigiMusic");
    formats << tentry("it",   "IT - Impulse Tracker");
    formats << tentry("mptm", "MPTM - Open ModPlug Tracker");
    formats << tentry("imf",  "IMF - Imago Orpheus");
    formats << tentry("mod",  "MOD - 15 and 31 instruments");
    formats << tentry("med",  "MED - OctaMED");
    formats << tentry("mtm",  "MTM - MultiTracker Module editor");
    formats << tentry("okt",  "OKT - Amiga Oktalyzer");
    formats << tentry("s3m",  "S3M - Scream Tracker 3");
    formats << tentry("stm",  "STM - Scream Tracker");
    formats << tentry("stx",  "STX - Scream Tracker Music Interface Kit");
    formats << tentry("ult",  "ULT - UltraTracker");
    formats << tentry("uni",  "UNI - MikMod");
    formats << tentry("xm",   "XM - FastTracker 2");

    for(int i=0; i<formats.size(); i++)
    {
        tentry& e = formats[i];
        QListWidgetItem item(e.second, ui->typeList);
        item.setData(Qt::UserRole, e.first);
        item.setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable|Qt::ItemIsUserCheckable);
        items.append(item);
        ui->typeList->addItem(&items.last());
    }
    ui->typeList->sortItems(Qt::AscendingOrder);
    on_reset_clicked();
}

void AssocFiles::on_reset_clicked()
{
    QHash<QString, bool> defaultFormats;

    defaultFormats["kar"] = true;

    defaultFormats["xmi"] = true;
    defaultFormats["mus"] = true;
    defaultFormats["cmf"] = true;

    defaultFormats["voc"] = true;
    defaultFormats["ay"]  = true;
    defaultFormats["gbs"] = true;
    defaultFormats["gym"] = true;
    defaultFormats["hes"] = true;
    defaultFormats["kss"] = true;
    defaultFormats["nsf"] = true;
    defaultFormats["nsfe"]= true;
    defaultFormats["sap"] = true;
    defaultFormats["spc"] = true;
    defaultFormats["vgm"] = true;
    defaultFormats["vgz"] = true;

    defaultFormats["669"] = true;
    defaultFormats["amf"] = true;
    defaultFormats["apun"]= true;
    defaultFormats["dsm"] = true;
    defaultFormats["far"] = true;
    defaultFormats["gdm"] = true;
    defaultFormats["it"]  = true;
    defaultFormats["mptm"]= true;
    defaultFormats["imf"] = true;
    defaultFormats["mod"] = true;
    defaultFormats["med"] = true;
    defaultFormats["mtm"] = true;
    defaultFormats["okt"] = true;
    defaultFormats["s3m"] = true;
    defaultFormats["stm"] = true;
    defaultFormats["stx"] = true;
    defaultFormats["ult"] = true;
    defaultFormats["uni"] = true;
    defaultFormats["xm"]  = true;

    for(int i=0; i<items.size(); i++)
    {
        items[i].setCheckState(
                    defaultFormats.contains(items[i].data(Qt::UserRole).toString()) ?
                        Qt::Checked : Qt::Unchecked );

    }
}

void AssocFiles::on_selectAll_clicked()
{
    for(int i=0; i<items.size(); i++)
    {
        items[i].setCheckState( Qt::Checked );
    }
}

void AssocFiles::on_clear_clicked()
{
    for(int i=0; i<items.size(); i++)
    {
        items[i].setCheckState( Qt::Unchecked );
    }
}

AssocFiles::~AssocFiles()
{
    delete ui;
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

static bool xIconSize(QList<QListWidgetItem> &items, int iconSize)
{
    bool success = true;
    const QString home = QDir::home().absolutePath();
    if(success) success = xCopyFile(QString(":/cat_musplay/cat_musplay_%1x%1.png")
                                    .arg(iconSize),
                                    QString("%1/.local/share/icons/PgeMusplay-%2.png")
                                    .arg(home).arg(iconSize));
    if(success) success = xCopyFile(QString(":/file_musplay/file_musplay_%1x%1.png")
                                    .arg(iconSize),
                                    QString("%1/.local/share/icons/pge_musplay_file-%2.png")
                                    .arg(home).arg(iconSize));
    for(int i = 0; i < items.size(); i++)
    {
        if(items[i].checkState() == Qt::Checked)
        {
            auto &item = items[i];
            if(success) success = xInstallIconResource("mimetypes",
                                                       iconSize,
                                                       "pge_musplay_file-%1.png",
                                                       QString("x-application-music-%1").arg(item.data(Qt::UserRole).toString()));
        }
    }
    if(success) success = xInstallIconResource("apps", iconSize, "PgeMusplay-%1.png", "PgeMusplay");

    return success;
}
#endif

void AssocFiles::on_AssocFiles_accepted()
{
    bool success = true;

#if defined(_WIN32)
    QSettings registry_hkcu("HKEY_CURRENT_USER", QSettings::NativeFormat);

    success = registry_hkcu.isWritable();

    //Main entry
    registry_hkcu.setValue("Software/Classes/PGEWohlstand.MusicFile/Default", tr("PGE-MusPlay Music File", "File Types"));
    registry_hkcu.setValue("Software/Classes/PGEWohlstand.MusicFile/DefaultIcon/Default", "\"" + QApplication::applicationFilePath().replace("/", "\\") + "\",1");
    registry_hkcu.setValue("Software/Classes/PGEWohlstand.MusicFile/Shell/Open/Command/Default", "\"" + QApplication::applicationFilePath().replace("/", "\\") + "\" \"%1\"");

    for(int i=0; i<items.size(); i++)
    {
        if(items[i].checkState()==Qt::Checked)
        {
            registry_hkcu.setValue(QString("Software/Classes/.%1/Default").arg(items[i].data(Qt::UserRole).toString()), "PGEWohlstand.MusicFile");
        }
    }

#elif defined(__linux__)
    const QString home = QDir::home().absolutePath();

    if(success) success = QDir().mkpath(home + "/.local/share/mime/packages");
    if(success) success = QDir().mkpath(home + "/.local/share/applications");
    if(success) success = QDir().mkpath(home + "/.local/share/icons");

    QString mimeHead =
            "<?xml version=\"1.0\"?>\n"
            "<mime-info xmlns='http://www.freedesktop.org/standards/shared-mime-info'>\n";
    QString entryTemplate =
            "   <mime-type type=\"application/x-pge-music-%1\">"
            "       <icon name=\"x-application-music-%1\"/>\n"
            "       <comment>%2</comment>\n"
            "       <glob pattern=\"*.%1\"/>\n"
            "   </mime-type>\n";
    QString mimeFoot = "</mime-info>\n";
    QStringList entries;
    QString minesForDesktopFile;

    for(int i = 0; i < items.size(); i++)
    {
        if(items[i].checkState() == Qt::Checked)
        {
            auto &item = items[i];
            entries.append(entryTemplate.arg(item.data(Qt::UserRole).toString(), item.text()));
            minesForDesktopFile.append(QString("application/x-pge-music-%1;").arg(item.data(Qt::UserRole).toString()));
        }
    }

    QFile outMime(home + "/.local/share/mime/packages/pge-musplay-mimeinfo.xml");
    outMime.open(QIODevice::WriteOnly|QIODevice::Truncate|QIODevice::Text);
    QTextStream outMimeS(&outMime);
    outMimeS << mimeHead;
    for(const QString &s : entries)
        outMimeS << s;
    outMimeS << mimeFoot;
    outMimeS.flush();
    outMime.close();

    if(success) success = xIconSize(items, 16);
    if(success) success = xIconSize(items, 32);
    if(success) success = xIconSize(items, 48);
    if(success) success = xIconSize(items, 256);

    QFile shortcut(":/PGE Music Player.desktop.template");
    if(success) success = shortcut.open(QFile::ReadOnly | QFile::Text);
    if(success)
    {
        QTextStream shtct(&shortcut);
        QString shortcut_text = shtct.readAll();
        QFile saveAs(home + "/.local/share/applications/pge_musplay.desktop");

        if(success) success = saveAs.open(QFile::WriteOnly | QFile::Text);
        if(success) QTextStream(&saveAs) << shortcut_text.arg(QApplication::applicationDirPath() + "/", minesForDesktopFile);
    }

    for(int i = 0; i < items.size(); i++)
    {
        if(items[i].checkState() == Qt::Checked)
        {
            auto &item = items[i];
            if(success) success = xRunCommand("xdg-mime", {"default", "pge_musplay.desktop", QString("application/x-pge-music-%1").arg(item.data(Qt::UserRole).toString())});
        }
    }

    if(success) success = xRunCommand("update-desktop-database", {home + "/.local/share/applications"});
    if(success) success = xRunCommand("update-mime-database", {home + "/.local/share/mime"});
#else
    success = false;
#endif

    if(!success)
        QMessageBox::warning(this, tr("Error"), tr("File association error has occured."));
    else
        QMessageBox::information(this, tr("Success"), tr("All files has been associated!"));
}
