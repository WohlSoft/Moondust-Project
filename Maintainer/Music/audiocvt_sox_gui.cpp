#include "audiocvt_sox_gui.h"
#include "ui_audiocvt_sox_gui.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <common_features/app_path.h>
#include <PGE_File_Formats/file_formats.h>
#include <QDirIterator>

#include <QtDebug>


void memclear(QListWidget *wid)
{
    QList<QListWidgetItem*> items = wid->findItems(QString("*"), Qt::MatchWrap | Qt::MatchWildcard);
    while(!items.isEmpty())
    {
        QListWidgetItem *tmp = items.first();
        items.pop_front();
        delete tmp;
    }
}


AudioCvt_Sox_gui::AudioCvt_Sox_gui(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AudioCvt_Sox_gui)
{
    ui->setupUi(this);

    ui->start->setText(tr("Start"));
#ifdef Q_OS_WIN
    ui->sox_bin_path->setText(ApplicationPath+"/tools/sox/sox.exe");
#else
    ui->sox_bin_path->setText("/usr/bin/sox");
#endif

    connect(&converter, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(nextStep(int,QProcess::ExitStatus)));
    connect(&converter, SIGNAL(readyReadStandardOutput()),this, SLOT(consoleMessage()) );
    connect(&converter, SIGNAL(readyReadStandardError()),this, SLOT(consoleMessageErr()) );

    inWork=false;
    isBackUp = false;
    madeJob=0;
}

AudioCvt_Sox_gui::~AudioCvt_Sox_gui()
{
    delete ui;
}

bool AudioCvt_Sox_gui::formatSupports(QString file)
{
    bool valid=false;
    if(file.endsWith(".mp3", Qt::CaseInsensitive)) valid=true;
    if(file.endsWith(".ogg", Qt::CaseInsensitive)) valid=true;
    if(file.endsWith(".wav", Qt::CaseInsensitive)) valid=true;
    if(file.endsWith(".flac", Qt::CaseInsensitive)) valid=true;
    return valid;
}

void AudioCvt_Sox_gui::resetStat()
{
    stat_converted_files=0;
    stat_overwritten_levels=0;
    stat_overwritten_worlds=0;
    stat_failed_levels=0;
}

void AudioCvt_Sox_gui::setEnableControls(bool en)
{
    ui->sox_bin_path->setEnabled(en);
    ui->browse->setEnabled(en);
    ui->doconv_level->setEnabled(en);
    ui->doconv_episode->setEnabled(en);
    ui->doconv_custom->setEnabled(en);
    ui->cvt_lvlfile->setEnabled(en && ui->doconv_level->isChecked());
    ui->cvt_lvlfile_browse->setEnabled(en && ui->doconv_level->isChecked());
    ui->cvt_episode->setEnabled(en && ui->doconv_episode->isChecked());
    ui->cvt_episode_browse->setEnabled(en && ui->doconv_episode->isChecked());
    ui->musics_list->setEnabled(en && ui->doconv_custom->isChecked());
    ui->add->setEnabled(en && ui->doconv_custom->isChecked());
    ui->remove->setEnabled(en && ui->doconv_custom->isChecked());
    ui->box_resample->setEnabled(en);
    ui->convertoTo->setEnabled(en);
    ui->backup->setEnabled(en);
    ui->progress->setEnabled(!en);
    ui->convertoTo->setEnabled(en);
    ui->closeME->setEnabled(en);
}

void AudioCvt_Sox_gui::start()
{
    if(ui->sox_bin_path->text().isEmpty())
    {
        QMessageBox::warning(this, tr("SoX error"), tr("SoX binary path is not defined.\nPlease set SoX path first"));
        return;
    }

    if(!QFile(ui->sox_bin_path->text()).exists())
    {
        QMessageBox::warning(this, tr("SoX error"), tr("SoX binary path is wrong.\nPlease set SoX path first"));
        return;
    }

    resetStat();

    if(ui->doconv_custom->isChecked())
    {
        job=CVT_CUSTOM;
        for(int i=0; i<ui->musics_list->count(); i++)
            filesToConvert.enqueue(ui->musics_list->item(i)->text());
    }
    else
    if(ui->doconv_episode->isChecked())
    {
        job=CVT_EPS;
        if(QDir(ui->cvt_episode->text()).exists())
        {
            episodeBox.openEpisode(ui->cvt_episode->text());
            QDir musicDir(ui->cvt_episode->text());
            QStringList filters;
            filters //MPEG 1 Layer III (LibMAD)
                    << "*.mp3"
                    //OGG Vorbis and FLAC (LibOGG, LibVorbis, LibFLAC)
                    << "*.ogg" << "*.flac";
                    //Uncompressed audio data
                    //<< "*.wav";
            musicDir.setSorting(QDir::Name);
            musicDir.setNameFilters(filters);
            QDirIterator dirsList(ui->cvt_episode->text(), filters,
                                  QDir::Files|QDir::NoSymLinks|QDir::NoDotAndDotDot,
                                  QDirIterator::Subdirectories);
            while(dirsList.hasNext())
            {
                dirsList.next();
                filesToConvert.enqueue(episodeBox.epPath+"/"+musicDir.relativeFilePath(dirsList.filePath()));
            }
        }
    }
    else
    if(ui->doconv_level->isChecked())
    {
        job=CVT_LVL;
        episodeBoxLvl.open(ui->cvt_lvlfile->text());
        for(int i=0; i<episodeBoxLvl.d.sections.size(); i++)
        {
            if(!episodeBoxLvl.d.sections[i].music_file.isEmpty())
                filesToConvert.enqueue(episodeBoxLvl.d.path+"/"+episodeBoxLvl.d.sections[i].music_file);
        }
    }

    if(filesToConvert.isEmpty())
    {
        QMessageBox::warning(this, tr("Nothing to do"), tr("No files to convert"));
        return;
    }

    ui->start->setText(tr("Stop"));
    setEnableControls(false);

    ui->progress->setMaximum(filesToConvert.size());
    ui->progress->setValue(-1);
    lastOutput.clear();
    inWork=true;
    madeJob=0;
    nextStep(0, QProcess::NormalExit);
}

void AudioCvt_Sox_gui::stop(bool do_abort)
{
    ui->start->setText(tr("Start"));
    ui->progress->setValue(0);
    ui->progress->setMaximum(10);
    setEnableControls(true);
    current_musFileNew.clear();
    current_musFileOld.clear();
    filesToConvert.clear();
    inWork=false;

    //Restore backUP
    if(do_abort)
    {
        if(converter.state()==QProcess::Running)
            converter.terminate();

        if(isBackUp)
        {
            if(QFile(current_musFileOld).exists())
            {
                if(QFile(current_musFileNew).exists())
                    QFile(current_musFileNew).remove();
                QFile(current_musFileOld).rename(current_musFileNew);
            }
        }
        else
        {
            if(QFile(current_musFileNew).exists())
                QFile(current_musFileNew).remove();
        }

        QMessageBox::warning(this, tr("SoX error"), tr("Operation cancaled"));
    }
}

void AudioCvt_Sox_gui::nextStep(int retStatus, QProcess::ExitStatus exitStatus)
{
    if(!inWork) return;

    if(exitStatus==QProcess::CrashExit)
    {
        QMessageBox::warning(this, tr("SoX error"), tr("SoX was crashed"));
        stop(true);
        return;
    }

    if(retStatus!=0)
    {
        QMessageBox::warning(this, tr("SoX error"), tr("SoX returned non-zero code: %1\n%2").arg(retStatus)
                             .arg(lastOutput));
        stop(true);
        return;
    }
    ui->progress->setValue(ui->progress->maximum()-filesToConvert.size());
    if((!current_musFileOld.isEmpty()) && (QFile(current_musFileOld).exists()))
    {
        if(!ui->backup->isChecked())
        {
            QFile(current_musFileOld).remove();
        }
        if(job==CVT_EPS)
            episodeBox.renameMusic(current_musFileOld, current_musFileNew, &stat_overwritten_levels, &stat_overwritten_worlds);
        else if (job==CVT_LVL)
            episodeBoxLvl.renameMusic(current_musFileOld, current_musFileNew);
        /*
        if(ledit)
        {
            for(int x=0;x<ledit->LvlData.sections.size();x++)
            {
                QString mFile = ledit->LvlData.sections[x].music_file;
                if(mFile.isEmpty()) continue;
                mFile.replace("\\", "/");
                if(current_musFileOld.endsWith(mFile, Qt::CaseInsensitive))
                {
                    QString newMusFile=current_musFileNew;
                    newMusFile.remove(ledit->LvlData.path + "/");
                    ledit->LvlData.sections[x].music_file = newMusFile;
                    ledit->LvlData.modified=true;
                }
            }
        }*/
    }

    QStringList args;
    bool hasWork=false;
    bool renameToBak=false;

retry_queue:
    if(filesToConvert.isEmpty())
    {
        QMessageBox::information(this, tr("All works completed"), tr("All files successfully converted!\n"
                                "Converted files: %1\n"
                                "Overwritten levels: %2\n"
                                "Overwritten worldss: %3\n"
                                                                     "%4")
                                 .arg(stat_converted_files)
                                 .arg(stat_overwritten_levels)
                                 .arg(stat_overwritten_worlds)
                                 .arg(lastOutput));

        memclear(ui->musics_list);

        stop();
        return;
    }

    args.clear();
    hasWork=false;
    renameToBak=false;
    isBackUp = false;

    converter.setProgram(ui->sox_bin_path->text());

    current_musFileNew = filesToConvert.dequeue();
    current_musFileOld.clear();

    if(ui->convertoTo->currentWidget()==ui->dont_change)
    {
        QFileInfo original(current_musFileNew);
        current_musFileOld = original.canonicalPath() + "/" + original.baseName() + "-backup";
        if (!original.completeSuffix().isEmpty())
            current_musFileOld += "." + original.completeSuffix();
        renameToBak=true;
    }
    else
    {
        QFileInfo original(current_musFileNew);
        current_musFileOld = original.canonicalPath() + "/" + original.baseName() + "-backup";
        if (!original.completeSuffix().isEmpty())
            current_musFileOld += "." + original.completeSuffix();
        renameToBak=true;

        if(ui->convertoTo->currentWidget()==ui->to_mp3)
        {
            if(!current_musFileNew.endsWith(".mp3", Qt::CaseInsensitive))
            {
                hasWork=true;
                current_musFileOld = current_musFileNew;
                current_musFileNew.replace(QRegExp("([^.]+)$"), "mp3");
                renameToBak=false;
            }
        }
        else
        if(ui->convertoTo->currentWidget()==ui->to_ogg)
        {
            if(!current_musFileNew.endsWith(".ogg", Qt::CaseInsensitive))
            {
                hasWork=true;
                current_musFileOld = current_musFileNew;
                current_musFileNew.replace(QRegExp("([^.]+)$"), "ogg");
                renameToBak=false;
            }
        }
        else
        if(ui->convertoTo->currentWidget()==ui->to_flac)
        {
            if(!current_musFileNew.endsWith(".flac", Qt::CaseInsensitive))
            {
                hasWork=true;
                current_musFileOld = current_musFileNew;
                current_musFileNew.replace(QRegExp("([^.]+)$"), "flac");
                renameToBak=false;
            }
        }
        else
        if(ui->convertoTo->currentWidget()==ui->to_wav)
        {
            if(!current_musFileNew.endsWith(".wav", Qt::CaseInsensitive))
            {
                hasWork=true;
                current_musFileOld = current_musFileNew;
                current_musFileNew.replace(QRegExp("([^.]+)$"), "wav");
                renameToBak=false;
            }
        }
    }

    args << current_musFileOld;
    if(ui->box_resample->isChecked())
    {
        hasWork=true;
        args << "-r";
        args << ui->rate->text();
    }

    if(ui->convertoTo->currentWidget()==ui->to_mp3)
    {
        hasWork=true;
        if(ui->mp3_set_birtate->isChecked())
        {
            args << "-C";
            args << ui->mp3_bitrate->currentText();
        }
    }

    if(ui->convertoTo->currentWidget()==ui->to_ogg)
    {
        hasWork=true;
        if(ui->ogg_set_quality->isChecked())
        {
            args << "-C";
            args << QString::number(ui->ogg_quality->value());
        }
    }

    if(renameToBak)
    {
        QFile(current_musFileNew).rename(current_musFileOld);
        isBackUp=true;
    }

    args << current_musFileNew;
    converter.setArguments(args);

    if(!hasWork)
    {
        if(!filesToConvert.isEmpty())
            goto retry_queue;

        if(madeJob==0)
            QMessageBox::warning(this, tr("Nothing to do"), tr("Tasks are not defined. Nothing to do."));
        stop();
        return;
    }

    qDebug() << ui->sox_bin_path->text() << args;
    madeJob++;
    converter.start();
    stat_converted_files++;
}

void AudioCvt_Sox_gui::consoleMessage()
{
    QByteArray strdata = converter.readAllStandardOutput();
    lastOutput.append(strdata);
    lastOutput.append("\n");
    //DevConsole::log(strdata, "SoX");
}

void AudioCvt_Sox_gui::consoleMessageErr()
{
    QByteArray strdata = converter.readAllStandardError();
    lastOutput.append(strdata);
    lastOutput.append("\n");
    //DevConsole::log(strdata, "SoX");
}

void AudioCvt_Sox_gui::on_browse_clicked()
{
    QString filter;
#ifdef Q_OS_WIN
    filter="SoX binary (sox.exe)";
#else
    filter="SoX binary (sox)";
#endif

    QString dir = QFileDialog::getOpenFileName(this, tr("Open SoX binary path"),
                                                 (ui->sox_bin_path->text().isEmpty() ? ApplicationPath : ui->sox_bin_path->text()),
                                               filter);
    if(dir.isEmpty()) return;

    ui->sox_bin_path->setText(dir);
}

void AudioCvt_Sox_gui::on_add_clicked()
{
    QString filter;
    filter="Audio files (*.wav *.ogg *.flac *.mp3)";
    QStringList files = QFileDialog::getOpenFileNames(this, tr("Add file to convert"),
                                                 (ui->sox_bin_path->text().isEmpty() ? ApplicationPath : ui->sox_bin_path->text()),
                                               filter);
    if(files.isEmpty()) return;

    ui->musics_list->addItems(files);
}

void AudioCvt_Sox_gui::on_remove_clicked()
{
    if(ui->musics_list->selectedItems().isEmpty()) return;
    delete ui->musics_list->selectedItems().first();
}

void AudioCvt_Sox_gui::on_start_clicked()
{
    if(inWork)
        stop(true);
    else
        start();
}

void AudioCvt_Sox_gui::closeEvent(QCloseEvent *e)
{
    if(inWork)
        e->ignore();
    else
        e->accept();
}

void AudioCvt_Sox_gui::on_closeME_clicked()
{
    this->close();
}

void AudioCvt_Sox_gui::on_cvt_lvlfile_browse_clicked()
{
    QString filter;
    filter="SMBX/PGE Level and world files with music (*.lvl *.lvlx *.wldx)";
    QString file = QFileDialog::getOpenFileName(this, tr("Selecting a level file to convert custom music"),
                                                 (ui->cvt_lvlfile->text().isEmpty() ? ApplicationPath : ui->cvt_lvlfile->text()),
                                               filter);
    if(file.isEmpty()) return;

    ui->cvt_lvlfile->setText(file);
}

void AudioCvt_Sox_gui::on_cvt_episode_browse_clicked()
{
    QString filter;
    filter="SoX binary (sox)";

    QString dir = QFileDialog::getExistingDirectory(this, tr("Open episode path to convert entire music set"),
                                                 (ui->cvt_episode->text().isEmpty() ? ApplicationPath : ui->cvt_episode->text()));
    if(dir.isEmpty()) return;

    ui->cvt_episode->setText(dir);
}
