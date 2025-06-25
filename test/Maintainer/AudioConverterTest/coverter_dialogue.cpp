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
                     ui->cvtProgress, &QProgressBar::setMaximum, Qt::QueuedConnection);

    QObject::connect(this, &CoverterDialogue::updateProgress,
                     ui->cvtProgress, &QProgressBar::setValue, Qt::QueuedConnection);

    QObject::connect(this, &CoverterDialogue::workStarted,
                     this, &CoverterDialogue::workStartedRun, Qt::QueuedConnection);

    QObject::connect(this, &CoverterDialogue::workFinished,
                     this, &CoverterDialogue::workFinishedRun, Qt::QueuedConnection);
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

    auto spec = m_cvt.getInSpec();

    spec.vbr = true;
    spec.bitrate = 128000;
    spec.quality = 5;
    spec.profile = -1;
    int oldRate = spec.m_sample_rate;

    if(ui->channels->isChecked())
        spec.m_channels = ui->dstChannels->value();

    if(ui->quality->isChecked())
        spec.quality = ui->dstQuality->value();

    if(ui->rate->isChecked())
        spec.m_sample_rate = ui->dstRate->value();

    double rateFactor = spec.m_sample_rate / (double)oldRate;

    qDebug() << "Input length" << spec.m_total_length;
    const int64_t wantedoutframes = ((int64_t)spec.m_total_length * spec.m_sample_rate / oldRate);

    // FIXME: De-facto length is slightly shorter than the expected length after performing the resampling
    // Sounds like to properly adjust loop points it's need to perform dynamic measure of the possible duration
    // and then use it to compute the proper factor. However, that will take a twice job of decoding and resampling.
    // Alternative solution: write them down at end of the file rather than at begin.
    spec.m_total_length = floor(spec.m_total_length * rateFactor);
    spec.m_loop_start = floor(spec.m_loop_start * rateFactor);
    spec.m_loop_end = floor(spec.m_loop_end * rateFactor);
    if(spec.m_loop_end > spec.m_total_length)
        spec.m_loop_end = spec.m_total_length;
    spec.m_loop_len = spec.m_loop_end - spec.m_loop_start;
    qDebug() << "Promised target length:" << spec.m_total_length << wantedoutframes;

    if(!m_cvt.openOutFile(ui->fileOut->text().toStdString(), FORMAT_OGG_VORBIS, spec))
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
    emit workStarted();
    emit setProgressSize(m_cvt.numChunks());

    while(!m_cvt.done())
    {
        m_cvt.runChunk();
        emit updateProgress(m_cvt.curChunk());
    }

    emit workFinished();
}
