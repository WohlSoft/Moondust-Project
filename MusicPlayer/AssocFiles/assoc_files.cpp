#ifndef MUSPLAY_USE_WINAPI
#include "assoc_files.h"
#include "ui_assoc_files.h"
#include <QListWidgetItem>
#include <QList>
#include <QPair>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QSettings>
#include <QMessageBox>

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

    defaultFormats["voc"] = true;
    defaultFormats["ay"]  = true;
    defaultFormats["bgs"] = true;
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

#ifndef __APPLE__
static bool xCopyFile(const QString &src, const QString &target)
{
    QFile tmp;
    tmp.setFileName(target);
    if(tmp.exists())
        tmp.remove();
    return QFile::copy(src, target);
}

static bool xIconSize(QList<QListWidgetItem> &items, int iconSize)
{
    bool success = true;
    if(success) success = xCopyFile(QString(":/cat_musplay/cat_musplay_%1x%1.png").arg(iconSize),
                                    QString("%1/.local/share/icons/PgeMusplay-%2.png").arg(QDir::home().absolutePath(), iconSize));
    if(success) success = xCopyFile(QString(":/file_musplay/file_musplay_%1x%1.png").arg(iconSize),
                                    QString("%1/.local/share/icons/pge_musplay_file-%2.png").arg(QDir::home().absolutePath(), iconSize));
    for(int i = 0; i < items.size(); i++)
    {
        if(items[i].checkState() == Qt::Checked)
        {
            auto &item = items[i];
            if(success) success = system( QString("xdg-icon-resource install --context mimetypes --size %3 %2/.local/share/icons/pge_musplay_file-%3.png x-application-music-%1")
                                          .arg(item.data(Qt::UserRole).toString()).arg(QDir::home().absolutePath()).arg(iconSize)
                                          .toLocal8Bit().constData()
                                          ) == 0;
        }
    }
    if(success) success = system( QString("xdg-icon-resource install --context apps --novendor --size %2 %1/.local/share/icons/PgeMusplay-%2.png PgeMusplay")
                                  .arg(QDir::home().absolutePath())
                                  .arg(iconSize)
                                  .toLocal8Bit().constData()) == 0;
    return success;
}
#endif

void AssocFiles::on_AssocFiles_accepted()
{
    bool success = true;
#ifdef _WIN32

    QSettings registry_hkcu("HKEY_CURRENT_USER", QSettings::NativeFormat);

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
#elif not defined __APPLE__
    if(success) success = QDir().mkpath(QDir::home().absolutePath() + "/.local/share/mime/packages");
    if(success) success = QDir().mkpath(QDir::home().absolutePath() + "/.local/share/applications");
    if(success) success = QDir().mkpath(QDir::home().absolutePath() + "/.local/share/icons");

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

    QFile outMime(QDir::home().absolutePath() + "/.local/share/mime/packages/pge-musplay-mimeinfo.xml");
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
        QFile saveAs(QDir::home().absolutePath() + "/.local/share/applications/pge_musplay.desktop");

        if(success) success = saveAs.open(QFile::WriteOnly | QFile::Text);
        if(success) QTextStream(&saveAs) << shortcut_text.arg(QApplication::applicationDirPath() + "/", minesForDesktopFile);
    }

    for(int i = 0; i < items.size(); i++)
    {
        if(items[i].checkState() == Qt::Checked)
        {
            auto &item = items[i];
            if(success) success = system(QString("xdg-mime default pge_musplay.desktop application/x-pge-music-%1").arg(item.data(Qt::UserRole).toString()).toLocal8Bit()) == 0;
        }
    }

    if(success) success = system(QString("update-desktop-database " + QDir::home().absolutePath() + "/.local/share/applications").toLocal8Bit().constData()) == 0;
    if(success) success = system(QString("update-mime-database " + QDir::home().absolutePath() + "/.local/share/mime").toLocal8Bit().constData()) == 0;

    if(!success)
        QMessageBox::warning(this, tr("Error"), tr("File association error has occured."));
    else
        QMessageBox::information(this, tr("Success"), tr("All files has been associated!"));
#else
    Q_UNUSED(success);
#endif
}

#endif
