/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2025 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This software is licensed under a dual license system (MIT or GPL version 3 or later).
 * This means you are free to choose with which of both licenses (MIT or GPL version 3 or later)
 * you want to use this software.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can see text of MIT license in the LICENSE.mit file you can see in Engine folder,
 * or see https://mit-license.org/.
 *
 * You can see text of GPLv3 license in the LICENSE.gpl3 file you can see in Engine folder,
 * or see <http://www.gnu.org/licenses/>.
 */

#include <cmath>
#include "coverter_dialogue.h"
#include "audio_format.h"

#include "./ui_coverter_dialogue.h"
#include <QtConcurrentRun>
#include <QtDebug>

CoverterDialogue::CoverterDialogue(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CoverterDialogue)
{
    ui->setupUi(this);

    QObject::connect(this, &CoverterDialogue::setProgressSize,
                     ui->cvtProgress, &QProgressBar::setMaximum, Qt::BlockingQueuedConnection);

    QObject::connect(this, &CoverterDialogue::updateProgress,
                     ui->cvtProgress, &QProgressBar::setValue, Qt::QueuedConnection);

    QObject::connect(this, &CoverterDialogue::workStarted,
                     this, &CoverterDialogue::workStartedRun, Qt::BlockingQueuedConnection);

    QObject::connect(this, &CoverterDialogue::workFinished,
                     this, &CoverterDialogue::workFinishedRun, Qt::BlockingQueuedConnection);

    QObject::connect(this, &CoverterDialogue::sendError,
                     this, &CoverterDialogue::onError, Qt::QueuedConnection);


    QObject::connect(ui->fileIn, &QLineEdit::editingFinished, this, [this]()->void
    {
        m_setup.beginGroup("files");
        m_setup.setValue("file-in", ui->fileIn->text());
        m_setup.endGroup();
        m_setup.sync();
    });

    QObject::connect(ui->fileOut, &QLineEdit::editingFinished, this, [this]()->void
    {
        m_setup.beginGroup("files");
        m_setup.setValue("file-out", ui->fileOut->text());
        m_setup.endGroup();
        m_setup.sync();
    });

    QObject::connect(ui->dstFormat, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
    this, [this](int idx)->void
    {
        m_setup.beginGroup("files");
        m_setup.setValue("dst-format", idx);
        m_setup.endGroup();
        m_setup.sync();

        QString outFile = ui->fileOut->text();
        int dot = outFile.lastIndexOf('.');
        if(dot >= 0)
        {
            switch(idx)
            {
            case 0:
                outFile.replace(dot + 1, outFile.size() - (dot - 1), "ogg");
                ui->fileOut->setText(outFile);
                break;
            case 1:
                outFile.replace(dot + 1, outFile.size() - (dot - 1), "qoa");
                ui->fileOut->setText(outFile);
                break;
            case 2:
                outFile.replace(dot + 1, outFile.size() - (dot - 1), "xqoa");
                ui->fileOut->setText(outFile);
                break;
            case 3:
                outFile.replace(dot + 1, outFile.size() - (dot - 1), "opus");
                ui->fileOut->setText(outFile);
                break;
            case 4:
                outFile.replace(dot + 1, outFile.size() - (dot - 1), "mp3");
                ui->fileOut->setText(outFile);
                break;
            case 5:
                outFile.replace(dot + 1, outFile.size() - (dot - 1), "wav");
                ui->fileOut->setText(outFile);
                break;
            case 6:
                outFile.replace(dot + 1, outFile.size() - (dot - 1), "wav");
                ui->fileOut->setText(outFile);
                break;
            }
        }
    });

    ui->dstFormat->blockSignals(true);
    ui->dstFormat->addItem("OGG Vorbis", (int)FORMAT_OGG_VORBIS);
    ui->dstFormat->addItem("QOA (Plain QOA)", (int)FORMAT_QOA);
    ui->dstFormat->addItem("XQOA (Extended QOA)", (int)FORMAT_XQOA);
    ui->dstFormat->addItem("OPUS", (int)FORMAT_OPUS);
    ui->dstFormat->addItem("MP3", (int)FORMAT_MP3);
    ui->dstFormat->addItem("WAV", (int)FORMAT_WAV);
    ui->dstFormat->addItem("WAV float64", (int)FORMAT_WAV_F64);
    ui->dstFormat->setCurrentIndex(0);
    ui->dstFormat->blockSignals(false);

    m_setup.beginGroup("files");
    ui->fileIn->blockSignals(true);
    ui->fileIn->setText(m_setup.value("file-in").toString());
    ui->fileIn->blockSignals(false);
    ui->fileOut->blockSignals(true);
    ui->fileOut->setText(m_setup.value("file-out").toString());
    ui->fileOut->blockSignals(false);
    ui->dstFormat->blockSignals(true);
    ui->dstFormat->setCurrentIndex(m_setup.value("dst-format").toInt());
    ui->dstFormat->blockSignals(false);
    m_setup.endGroup();
}

CoverterDialogue::~CoverterDialogue()
{
    m_runner.waitForFinished();
    delete ui;
}

void CoverterDialogue::onError(const QString &errString)
{
    ui->error->setText(errString);
}

void CoverterDialogue::on_runCvt_clicked()
{
    ui->error->clear();
    m_cvt.setCutAtLoopEnd(ui->dstCutAtLoopEnd->isChecked());

    QString inPath = ui->fileIn->text();
    QString inPathArgs;
    int argsBegin = inPath.indexOf('|');

    if(argsBegin >= 0)
    {
        inPathArgs = inPath.mid(argsBegin + 1);
        inPath.remove(argsBegin, inPath.size() - argsBegin);
    }

    if(!m_cvt.openInFile(inPath.toStdString(), inPathArgs.toStdString()))
    {
        auto err = QString("Failed to open input file %1: %2").arg(inPath, QString::fromStdString(m_cvt.getLastError()));
        qWarning() << err;
        emit sendError(err);
        m_cvt.close();
        return;
    }

    m_dstSpec = m_cvt.getInSpec();

    m_dstSpec.vbr = ui->quality->isChecked();
    m_dstSpec.bitrate = 128000;
    m_dstSpec.quality = 5;
    m_dstSpec.profile = -1;
    int oldRate = m_dstSpec.m_sample_rate;

    if(ui->channels->isChecked())
        m_dstSpec.m_channels = ui->dstChannels->value();

    if(ui->quality->isChecked())
        m_dstSpec.quality = ui->dstQuality->value();

    if(ui->bitrate->isChecked())
        m_dstSpec.bitrate = ui->dstBitRate->value() * 1000;

    if(ui->rate->isChecked())
        m_dstSpec.m_sample_rate = ui->dstRate->value();

    if(!m_cvt.openOutFile(ui->fileOut->text().toStdString(), ui->dstFormat->currentData().toInt(), m_dstSpec))
    {
        auto err = QString("Failed to open output file %1: %2").arg(ui->fileOut->text(), QString::fromStdString(m_cvt.getLastError()));
        qWarning() << err;
        emit sendError(err);
        m_cvt.close();
        return;
    }

    m_dstSpec = m_cvt.getOutSpec();

    if(m_dstSpec.m_sample_rate != oldRate &&
       m_dstSpec.m_loop_start != 0 && m_dstSpec.m_loop_end != 0 &&
       m_dstSpec.m_loop_start < m_dstSpec.m_loop_end &&
       m_dstSpec.m_loop_end <= m_dstSpec.m_total_length) // If sample rate is different and valid loop points are set
    {
        // Perform a dry run to figure the valid loop points data
        m_phase = PHASE_LENGHT_MEASURE;
    }
    else // Otherwise, just write down as-is
    {
        m_phase = PHASE_CONVERSION;
    }

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    m_runner = QtConcurrent::run(this, &CoverterDialogue::runner);
#else
    m_runner = QtConcurrent::run(&CoverterDialogue::runner, this);
#endif
}

void CoverterDialogue::workStartedRun()
{
    ui->setup->setDisabled(true);
    ui->cvtProgress->setEnabled(true);
}

void CoverterDialogue::workFinishedRun()
{
    ui->setup->setDisabled(false);
    ui->cvtProgress->setEnabled(false);
    ui->cvtProgress->setValue(0);
    qDebug() << "Stats: samples read" << m_cvt.getSamplesReadStat()
             << "write" << m_cvt.getSamplesWrittenStat();
    m_cvt.close();
}

void CoverterDialogue::runner()
{
    bool hasMeasure = (m_phase == PHASE_LENGHT_MEASURE);
    emit workStarted();
    uint32_t prev_progress = 0;

    emit setProgressSize(hasMeasure ? m_cvt.numChunks() * 2 : m_cvt.numChunks());
    emit updateProgress(m_cvt.curChunk());
    prev_progress = m_cvt.curChunk();

    while(!m_cvt.done())
    {
        if(hasMeasure)
        {
            switch(m_phase)
            {
            case PHASE_LENGHT_MEASURE:
                if(!m_cvt.runChunk(true))
                {
                    auto err = QString("Conversion failed (Phase Measure): %1").arg(QString::fromStdString(m_cvt.getLastError()));
                    qWarning() << err;
                    emit sendError(err);
                    emit workFinished();
                    return;
                }

                if(prev_progress != m_cvt.curChunk())
                {
                    // qDebug() << "Progress" << m_cvt.curChunk() << " of " << m_cvt.numChunks() * 2;
                    emit updateProgress(m_cvt.curChunk());
                    prev_progress = m_cvt.curChunk();
                }

                if(m_cvt.done())
                {
                    int64_t totalRead = m_cvt.getSamplesReadStat();
                    int64_t totalWrite = m_cvt.getSamplesWrittenStat();

                    double rateFactor = totalWrite / (double)totalRead;
                    m_dstSpec.m_total_length = totalWrite;
                    m_dstSpec.m_loop_start = std::floor(m_dstSpec.m_loop_start * rateFactor);
                    m_dstSpec.m_loop_end = std::floor(m_dstSpec.m_loop_end * rateFactor);
                    if(m_dstSpec.m_loop_end > m_dstSpec.m_total_length)
                        m_dstSpec.m_loop_end = m_dstSpec.m_total_length;
                    m_dstSpec.m_loop_len = m_dstSpec.m_loop_end - m_dstSpec.m_loop_start;

                    m_cvt.rewindRead();

                    if(!m_cvt.openOutFile(ui->fileOut->text().toStdString(), ui->dstFormat->currentData().toInt(), m_dstSpec))
                    {
                        auto err = QString("Failed to open output file %1: %2").arg(ui->fileOut->text(), QString::fromStdString(m_cvt.getLastError()));
                        qWarning() << err;
                        emit sendError(err);
                        emit workFinished();
                        return;
                    }

                    m_phase = PHASE_CONVERSION;
                }

                break;
            case PHASE_CONVERSION:
                if(!m_cvt.runChunk(false))
                {
                    auto err = QString("Conversion failed (Phase Conversion): %1").arg(QString::fromStdString(m_cvt.getLastError()));
                    qWarning() << err;
                    emit sendError(err);
                    emit workFinished();
                    return;
                }

                if(prev_progress != m_cvt.curChunk())
                {
                    // qDebug() << "Progress" << m_cvt.numChunks() + m_cvt.curChunk() << " of " << m_cvt.numChunks() * 2;
                    emit updateProgress(m_cvt.numChunks() + m_cvt.curChunk());
                    prev_progress = m_cvt.curChunk();
                }

                break;
            }
        }
        else
        {
            if(!m_cvt.runChunk())
            {
                auto err = QString("Conversion failed: %1").arg(QString::fromStdString(m_cvt.getLastError()));
                qWarning() << err;
                emit sendError(err);
                emit workFinished();
                return;
            }

            emit updateProgress(m_cvt.curChunk());
        }
    }

    emit sendError("Success!");
    emit workFinished();
}
