#include "audiocvt_sox_gui.h"
#include "ui_audiocvt_sox_gui.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <common_features/app_path.h>
#include <common_features/util.h>
#include <audio/sdl_music_player.h>
#include <audio/music_player.h>
#include <dev_console/devconsole.h>
#include <common_features/mainwinconnect.h>

#include <QtDebug>

AudioCvt_Sox_gui::AudioCvt_Sox_gui(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AudioCvt_Sox_gui)
{
    ui->setupUi(this);

    ui->start->setText(tr("Start"));
#ifdef Q_OS_WIN
    ui->sox_bin_path->setText(ApplicationPath+"/tools/sox/sox.exe");
#else
    ui->sox_bin_path->setText(ApplicationPath+"/usr/bin/sox");
#endif

    connect(&converter, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(nexStep(int,QProcess::ExitStatus)));
    connect(&converter, SIGNAL(readyReadStandardOutput()),this, SLOT(consoleMessage()) );
    connect(&converter, SIGNAL(readyReadStandardError()),this, SLOT(consoleMessageErr()) );

    isLevel = (MainWinConnect::pMainWin->activeChildWindow()==1);

    if(isLevel)
    {
        LevelEdit *edit = MainWinConnect::pMainWin->activeLvlEditWin();
        if(edit)
        {
            curSectionMusic = edit->LvlData.path + "/" + edit->LvlData.sections[edit->LvlData.CurSection].music_file;
            curSectionMusic.replace("\\", "/");
            for(int s=0;s<edit->LvlData.sections.size(); s++)
            {
                QString file = edit->LvlData.path + "/" + edit->LvlData.sections[s].music_file;
                file.replace("\\", "/");
                if(QFileInfo(file).exists() && QFileInfo(file).isFile())
                    curLevelMusic << file;
            }
        }
    }
    else
    {
        ui->cur_level->setEnabled(false);
        ui->cur_section->setEnabled(false);
        ui->cur_custom->setChecked(true);
    }

    inWork=false;
    madeJob=0;
}

AudioCvt_Sox_gui::~AudioCvt_Sox_gui()
{
    delete ui;
}

void AudioCvt_Sox_gui::setEnableControls(bool en)
{
    ui->sox_bin_path->setEnabled(en);
    ui->browse->setEnabled(en);
    ui->cur_section->setEnabled(en && isLevel);
    ui->cur_level->setEnabled(en && isLevel);
    ui->cur_custom->setEnabled(en);
    ui->musics_list->setEnabled(en && ui->cur_custom->isChecked());
    ui->add->setEnabled(en && ui->cur_custom->isChecked());
    ui->remove->setEnabled(en && ui->cur_custom->isChecked());
    ui->box_resample->setEnabled(en);
    ui->convertoTo->setEnabled(en);
    ui->backup->setEnabled(en);
    ui->progress->setEnabled(!en);
    ui->convertoTo->setEnabled(en);
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

    if(ui->cur_custom->isChecked())
    {
        for(int i=0; i<ui->musics_list->count(); i++)
            filesToConvert.enqueue(ui->musics_list->item(i)->text());
    }
    else
    if(ui->cur_section->isChecked())
    {
        if(QFileInfo(curSectionMusic).exists() && QFileInfo(curSectionMusic).isFile())
            filesToConvert.enqueue(curSectionMusic);
    }
    else
    if(ui->cur_level->isChecked())
    {
        for(int i=0; i<curLevelMusic.size(); i++)
            filesToConvert.enqueue(curLevelMusic[i]);
    }

    if(filesToConvert.isEmpty())
    {
        QMessageBox::warning(this, tr("Nothing to do"), tr("No files to convert"));
        return;
    }

    ui->start->setText(tr("Stop"));
    setEnableControls(false);

    ui->progress->setMaximum(filesToConvert.size());
    ui->progress->setValue(0);
    lastOutput.clear();
    inWork=true;
    madeJob=0;
    PGE_MusPlayer::MUS_freeStream();
    nexStep(0, QProcess::NormalExit);
}

void AudioCvt_Sox_gui::stop()
{
    ui->start->setText(tr("Start"));
    ui->progress->setValue(0);
    ui->progress->setMaximum(10);
    setEnableControls(true);
    if(converter.state()==QProcess::Running)
        converter.terminate();
    current_musFileNew.clear();
    current_musFileOld.clear();
    filesToConvert.clear();
    inWork=false;
    MainWinConnect::pMainWin->setMusic();
}

void AudioCvt_Sox_gui::nexStep(int retStatus, QProcess::ExitStatus exitStatus)
{
    if(exitStatus==QProcess::CrashExit)
    {
        QMessageBox::warning(this, tr("SoX error"), tr("SoX was crashed"));
        stop();
        return;
    }

    if(retStatus!=0)
    {
        QMessageBox::warning(this, tr("SoX error"), tr("SoX returned non-zero code: %1\n%2").arg(retStatus)
                             .arg(lastOutput));
        stop();
        return;
    }
    ui->progress->setValue(ui->progress->maximum()-filesToConvert.size());
    if((!current_musFileOld.isEmpty()) && (QFile(current_musFileOld).exists()))
    {
        if(!ui->backup->isChecked())
        {
            QFile(current_musFileOld).remove();
        }
    }

    QStringList args;
    bool hasWork=false;
    bool renameToBak=false;

retry_queue:
    if(filesToConvert.isEmpty())
    {
        QMessageBox::information(this, tr("All works completed"), tr("All files successfully converted!\n%1")
                                 .arg(lastOutput));

        util::memclear(ui->musics_list);

        stop();
        return;
    }

    args.clear();
    hasWork=false;
    renameToBak=false;

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
        QFile(current_musFileNew).rename(current_musFileOld);

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

}

void AudioCvt_Sox_gui::consoleMessage()
{
    DevConsole::show();
    QByteArray strdata = converter.readAllStandardOutput();
    lastOutput.append(strdata);
    lastOutput.append("\n");
    DevConsole::log(strdata, "SoX");
}

void AudioCvt_Sox_gui::consoleMessageErr()
{
    DevConsole::show();
    QByteArray strdata = converter.readAllStandardError();
    lastOutput.append(strdata);
    lastOutput.append("\n");
    DevConsole::log(strdata, "SoX");
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
        stop();
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
