#include "audiocvt_sox_gui.h"
#include "ui_audiocvt_sox_gui.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <pge_qt_compat.h>
#include <common_features/app_path.h>
#include <common_features/util.h>
#include <audio/sdl_music_player.h>
#include <audio/music_player.h>
#include <dev_console/devconsole.h>
#include <common_features/main_window_ptr.h>

#include <QtDebug>

#include "qfile_dialogs_default_options.hpp"


AudioCvt_Sox_gui::AudioCvt_Sox_gui(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AudioCvt_Sox_gui)
{
    m_mw = qobject_cast<MainWindow*>(parent);
    Q_ASSERT(m_mw);//Parent must be a Main Window!
    ui->setupUi(this);
    ui->start->setText(tr("Start"));
#ifdef Q_OS_WIN
    ui->sox_bin_path->setText(ApplicationPath + "/tools/sox/sox.exe");
#else
    ui->sox_bin_path->setText("/usr/bin/sox");
#endif

    // Moved from UI file itself
    QObject::connect(ui->ogg_quality, &QSlider::valueChanged,
                     ui->ogg_quality_var,
                     static_cast<void(QLabel::*)(int)>(&QLabel::setNum));

    QObject::connect(&converter, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(nextStep(int, QProcess::ExitStatus)));
    QObject::connect(&converter, SIGNAL(readyReadStandardOutput()), this, SLOT(consoleMessage()));
    QObject::connect(&converter, SIGNAL(readyReadStandardError()), this, SLOT(consoleMessageErr()));
    isLevel = (m_mw && m_mw->activeChildWindow() == MainWindow::WND_Level);
    ledit = NULL;

    if(m_mw && isLevel)
    {
        ledit = m_mw->activeLvlEditWin();

        if(ledit)
        {
            curSectionMusic = ledit->LvlData.meta.path + "/" + ledit->LvlData.sections[ledit->LvlData.CurSection].music_file;
            curSectionMusic.replace("\\", "/");

            if(!formatSupports(curSectionMusic))
                curSectionMusic.clear();

            for(int s = 0; s < ledit->LvlData.sections.size(); s++)
            {
                QString file = ledit->LvlData.meta.path + "/" + ledit->LvlData.sections[s].music_file;
                file.replace("\\", "/");

                if(curLevelMusic.contains(file))//Don't add duplicate
                    continue;

                if(!formatSupports(file)) //Don't add unsupported formnats
                    continue;

                if(QFileInfo(file).exists() && QFileInfo(file).isFile())//Don't add non-existing files
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

    inWork = false;
    isBackUp = false;
    madeJob = 0;
}

AudioCvt_Sox_gui::~AudioCvt_Sox_gui()
{
    delete ui;
}

bool AudioCvt_Sox_gui::formatSupports(QString file)
{
    bool valid = false;

    if(file.endsWith(".mp3", Qt::CaseInsensitive))
        valid = true;
    else
    if(file.endsWith(".ogg", Qt::CaseInsensitive))
        valid = true;
    else
    if(file.endsWith(".wav", Qt::CaseInsensitive))
        valid = true;
    else
    if(file.endsWith(".flac", Qt::CaseInsensitive))
        valid = true;

    return valid;
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
        QMessageBox::warning(this,
                             tr("SoX error"),
                             tr("SoX executable path is not defined.\nPlease set SoX path first"));
        return;
    }

    if(!QFile(ui->sox_bin_path->text()).exists())
    {
        QMessageBox::warning(this,
                             tr("SoX error"),
                             tr("SoX executable path is invalid.\nPlease set SoX path first"));
        return;
    }

    if(ui->cur_custom->isChecked())
    {
        for(int i = 0; i < ui->musics_list->count(); i++)
            filesToConvert.enqueue(ui->musics_list->item(i)->text());
    }
    else if(ui->cur_section->isChecked())
    {
        if(QFileInfo(curSectionMusic).exists() && QFileInfo(curSectionMusic).isFile())
            filesToConvert.enqueue(curSectionMusic);
    }
    else if(ui->cur_level->isChecked())
    {
        for(int i = 0; i < curLevelMusic.size(); i++)
            filesToConvert.enqueue(curLevelMusic[i]);
    }

    if(filesToConvert.isEmpty())
    {
        QMessageBox::warning(this,
                             tr("Nothing to do."),
                             tr("No files to convert"));
        return;
    }

    ui->start->setText(tr("Stop"));
    setEnableControls(false);
    ui->progress->setMaximum(filesToConvert.size());
    ui->progress->setValue(0);
    lastOutput.clear();
    inWork = true;
    madeJob = 0;
    PGE_MusPlayer::closeFile();
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
    inWork = false;

    //Restore backUP
    if(do_abort)
    {
        if(converter.state() == QProcess::Running)
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

        PGE_MusPlayer::openFile(LvlMusPlay::currentMusicPath);
        PGE_MusPlayer::changeVolume(m_mw ? m_mw->musicVolume() : MIX_MAX_VOLUME);
        PGE_MusPlayer::play();
        QMessageBox::warning(this,
                             tr("SoX error"),
                             tr("Operation canceled"));
    }
}

void AudioCvt_Sox_gui::nextStep(int retStatus, QProcess::ExitStatus exitStatus)
{
    if(!inWork)
        return;

    if(exitStatus == QProcess::CrashExit)
    {
        QMessageBox::warning(this,
                             tr("SoX error"),
                             tr("Sorry, SoX has crashed"));
        stop(true);
        return;
    }

    if(retStatus != 0)
    {
        QString warns = lastOutput;
        if(warns.size() > 200)
        {
            warns.resize(200);
            warns.append("...");
        }
        QMessageBox::warning(this,
                             tr("SoX error"),
                             tr("SoX returned a non-zero exit code: %1\n%2")
                             .arg(retStatus)
                             .arg(warns));
        stop(true);
        return;
    }

    ui->progress->setValue(ui->progress->maximum() - filesToConvert.size());

    if((!current_musFileOld.isEmpty()) && (QFile(current_musFileOld).exists()))
    {
        if(!ui->backup->isChecked())
            QFile(current_musFileOld).remove();

        if(ledit)
        {
            for(int x = 0; x < ledit->LvlData.sections.size(); x++)
            {
                QString mFile = ledit->LvlData.sections[x].music_file;

                if(mFile.isEmpty()) continue;

                mFile.replace("\\", "/");

                if(current_musFileOld.endsWith(mFile, Qt::CaseInsensitive))
                {
                    QString newMusFile = current_musFileNew;
                    newMusFile.remove(ledit->LvlData.meta.path + "/");
                    ledit->LvlData.sections[x].music_file = newMusFile;
                    ledit->LvlData.meta.modified = true;
                }
            }
        }
    }

    QStringList args;
    bool hasWork = false;
    bool renameToBak = false;
retry_queue:

    if(filesToConvert.isEmpty())
    {
        if(ledit)
        {
            if(curSectionMusic == LvlMusPlay::currentMusicPath)
                LvlMusPlay::currentMusicPath = ledit->LvlData.meta.path + "/" + ledit->LvlData.sections[ledit->LvlData.CurSection].music_file;
        }

        PGE_MusPlayer::openFile(LvlMusPlay::currentMusicPath);
        PGE_MusPlayer::changeVolume(m_mw ? m_mw->musicVolume() : MIX_MAX_VOLUME);
        PGE_MusPlayer::play();
        QString warns = lastOutput;
        if(warns.size() > 200)
        {
            warns.resize(200);
            warns.append("...");
        }

        QMessageBox::information(this,
                                 tr("Operation complete"),
                                 tr("All files successfully converted!\n%1")
                                 .arg(warns));
        util::memclear(ui->musics_list);
        stop();
        return;
    }

    args.clear();
    hasWork = false;
    renameToBak = false;
    isBackUp = false;
    converter.setProgram(ui->sox_bin_path->text());
    current_musFileNew = filesToConvert.dequeue();
    current_musFileOld.clear();

    if(ui->convertoTo->currentWidget() == ui->dont_change)
    {
        QFileInfo original(current_musFileNew);
        current_musFileOld = original.canonicalPath() + "/" + util::getBaseFilename(original.fileName()) + "-backup";

        if(!original.completeSuffix().isEmpty())
            current_musFileOld += "." + original.completeSuffix();

        renameToBak = true;
    }
    else
    {
        QFileInfo original(current_musFileNew);
        current_musFileOld = original.canonicalPath() + "/" + util::getBaseFilename(original.baseName()) + "-backup";

        if(!original.completeSuffix().isEmpty())
            current_musFileOld += "." + original.completeSuffix();

        renameToBak = true;

        if(ui->convertoTo->currentWidget() == ui->to_mp3)
        {
            if(!current_musFileNew.endsWith(".mp3", Qt::CaseInsensitive))
            {
                hasWork = true;
                current_musFileOld = current_musFileNew;
                current_musFileNew.replace(Q_QRegExp("([^.]+)$"), "mp3");
                renameToBak = false;
            }
        }
        else if(ui->convertoTo->currentWidget() == ui->to_ogg)
        {
            if(!current_musFileNew.endsWith(".ogg", Qt::CaseInsensitive))
            {
                hasWork = true;
                current_musFileOld = current_musFileNew;
                current_musFileNew.replace(Q_QRegExp("([^.]+)$"), "ogg");
                renameToBak = false;
            }
        }
        else if(ui->convertoTo->currentWidget() == ui->to_flac)
        {
            if(!current_musFileNew.endsWith(".flac", Qt::CaseInsensitive))
            {
                hasWork = true;
                current_musFileOld = current_musFileNew;
                current_musFileNew.replace(Q_QRegExp("([^.]+)$"), "flac");
                renameToBak = false;
            }
        }
        else if(ui->convertoTo->currentWidget() == ui->to_wav)
        {
            if(!current_musFileNew.endsWith(".wav", Qt::CaseInsensitive))
            {
                hasWork = true;
                current_musFileOld = current_musFileNew;
                current_musFileNew.replace(Q_QRegExp("([^.]+)$"), "wav");
                renameToBak = false;
            }
        }
    }

    args << current_musFileOld;

    if(ui->box_resample->isChecked())
    {
        hasWork = true;
        args << "-r";
        args << ui->rate->text();
    }

    if(ui->convertoTo->currentWidget() == ui->to_mp3)
    {
        hasWork = true;

        if(ui->mp3_set_birtate->isChecked())
        {
            args << "-C";
            args << ui->mp3_bitrate->currentText();
        }
    }

    if(ui->convertoTo->currentWidget() == ui->to_ogg)
    {
        hasWork = true;

        if(ui->ogg_set_quality->isChecked())
        {
            args << "-C";
            args << QString::number(ui->ogg_quality->value());
        }
    }

    if(renameToBak)
    {
        QFile(current_musFileNew).rename(current_musFileOld);
        isBackUp = true;
    }

    args << current_musFileNew;
    converter.setArguments(args);

    if(!hasWork)
    {
        if(!filesToConvert.isEmpty())
            goto retry_queue;

        if(madeJob == 0)
            QMessageBox::warning(this,
                                 tr("Nothing to do."),
                                 tr("No tasks defined. Nothing to do."));

        stop();
        return;
    }

    qDebug() << ui->sox_bin_path->text() << args;
    madeJob++;
    converter.start();
}

void AudioCvt_Sox_gui::consoleMessage()
{
    QByteArray strdata = converter.readAllStandardOutput();
    lastOutput.append(strdata);
    lastOutput.append("\n");
    DevConsole::log(strdata, "SoX");
}

void AudioCvt_Sox_gui::consoleMessageErr()
{
    QByteArray strdata = converter.readAllStandardError();
    QString &out = strdata.contains("WARN") ? lastWarnOutput : lastOutput;
    out.append(strdata);
    out.append("\n");
    DevConsole::log(strdata, "SoX");
}

void AudioCvt_Sox_gui::on_browse_clicked()
{
    QString filter;
#ifdef Q_OS_WIN
    filter = "SoX binary (sox.exe)";
#else
    filter = "SoX binary (sox)";
#endif
    QString dir = QFileDialog::getOpenFileName(this, tr("Open SoX executable path"),
                  (ui->sox_bin_path->text().isEmpty() ? ApplicationPath : ui->sox_bin_path->text()),
                  filter, nullptr, c_fileDialogOptions);

    if(dir.isEmpty()) return;

    ui->sox_bin_path->setText(dir);
}

void AudioCvt_Sox_gui::on_add_clicked()
{
    QString filter;
    filter = "Audio files (*.wav *.ogg *.flac *.mp3)";
    QStringList files = QFileDialog::getOpenFileNames(this, tr("Select file to convert"),
                        (ui->sox_bin_path->text().isEmpty() ? ApplicationPath : ui->sox_bin_path->text()),
                        filter, nullptr, c_fileDialogOptions);

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
