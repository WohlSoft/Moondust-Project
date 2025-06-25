#include "coverter_dialogue.h"
#include "./ui_coverter_dialogue.h"
#include <QtConcurrent/QtConcurrent>
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

    m_setup.beginGroup("files");
    ui->fileIn->blockSignals(true);
    ui->fileIn->setText(m_setup.value("file-in").toString());
    ui->fileIn->blockSignals(false);
    ui->fileOut->blockSignals(true);
    ui->fileOut->setText(m_setup.value("file-out").toString());
    ui->fileOut->blockSignals(false);
    m_setup.endGroup();
}

CoverterDialogue::~CoverterDialogue()
{
    m_runner.waitForFinished();
    delete ui;
}

void CoverterDialogue::on_runCvt_clicked()
{
    if(!m_cvt.openInFile(ui->fileIn->text().toStdString()))
    {
        qWarning() << "Failed to open input file" << ui->fileIn->text();
        return;
    }

    m_dstSpec = m_cvt.getInSpec();

    m_dstSpec.vbr = true;
    m_dstSpec.bitrate = 128000;
    m_dstSpec.quality = 5;
    m_dstSpec.profile = -1;
    int oldRate = m_dstSpec.m_sample_rate;

    if(ui->channels->isChecked())
        m_dstSpec.m_channels = ui->dstChannels->value();

    if(ui->quality->isChecked())
        m_dstSpec.quality = ui->dstQuality->value();

    if(ui->rate->isChecked())
        m_dstSpec.m_sample_rate = ui->dstRate->value();

    if(m_dstSpec.m_sample_rate != oldRate &&
       m_dstSpec.m_loop_start != 0 && m_dstSpec.m_loop_end != 0 &&
       m_dstSpec.m_loop_start < m_dstSpec.m_loop_end) // If sample rate is different and valid loop points are set
    {
        // Perform a dry run to figure the valid loop points data
        m_phase = PHASE_LENGHT_MEASURE;
    }
    else // Otherwise, just write down as-is
    {
        m_phase = PHASE_CONVERSION;

        if(!m_cvt.openOutFile(ui->fileOut->text().toStdString(), FORMAT_OGG_VORBIS, m_dstSpec))
        {
            qWarning() << "Failed to open output file" << ui->fileOut->text();
            return;
        }
    }

    if(!m_cvt.openOutFile(ui->fileOut->text().toStdString(), FORMAT_OGG_VORBIS, m_dstSpec))
    {
        qWarning() << "Failed to open output file" << ui->fileOut->text();
        return;
    }

    m_runner = QtConcurrent::run<void>(this, &CoverterDialogue::runner);
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
                m_cvt.runChunk(true);

                if(prev_progress != m_cvt.curChunk())
                {
                    qDebug() << "Progress" << m_cvt.curChunk() << " of " << m_cvt.numChunks() * 2;
                    emit updateProgress(m_cvt.curChunk());
                    prev_progress = m_cvt.curChunk();
                }

                if(m_cvt.done())
                {
                    int64_t totalRead = m_cvt.getSamplesReadStat();
                    int64_t totalWrite = m_cvt.getSamplesWrittenStat();

                    double rateFactor = totalWrite / (double)totalRead;
                    m_dstSpec.m_total_length = totalWrite;
                    m_dstSpec.m_loop_start = floor(m_dstSpec.m_loop_start * rateFactor);
                    m_dstSpec.m_loop_end = floor(m_dstSpec.m_loop_end * rateFactor);
                    if(m_dstSpec.m_loop_end > m_dstSpec.m_total_length)
                        m_dstSpec.m_loop_end = m_dstSpec.m_total_length;
                    m_dstSpec.m_loop_len = m_dstSpec.m_loop_end - m_dstSpec.m_loop_start;

                    m_cvt.rewindRead();

                    if(!m_cvt.openOutFile(ui->fileOut->text().toStdString(), FORMAT_OGG_VORBIS, m_dstSpec))
                    {
                        qWarning() << "Failed to open output file" << ui->fileOut->text();
                        emit workFinished();
                        return;
                    }

                    m_phase = PHASE_CONVERSION;
                }

                break;
            case PHASE_CONVERSION:
                m_cvt.runChunk(false);

                if(prev_progress != m_cvt.curChunk())
                {
                    qDebug() << "Progress" << m_cvt.numChunks() + m_cvt.curChunk() << " of " << m_cvt.numChunks() * 2;
                    emit updateProgress(m_cvt.numChunks() + m_cvt.curChunk());
                    prev_progress = m_cvt.curChunk();
                }

                break;
            }
        }
        else
        {
            m_cvt.runChunk();
            emit updateProgress(m_cvt.curChunk());
        }
    }

    emit workFinished();
}
