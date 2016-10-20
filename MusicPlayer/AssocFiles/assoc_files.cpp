#ifndef MUSPLAY_USE_WINAPI
#include "assoc_files.h"
#include "ui_assoc_files.h"
#include <QListWidgetItem>
#include <QList>
#include <QPair>
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

void AssocFiles::on_AssocFiles_accepted()
{
#ifdef _WIN32
    //QSettings registry_hkcr("HKEY_CLASSES_ROOT", QSettings::NativeFormat);
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
#endif
}

#endif
