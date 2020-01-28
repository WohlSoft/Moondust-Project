#include "case_fixer.h"
#include "ui_case_fixer.h"
#include <QtConcurrent>
#include <QMessageBox>



CaseFixerWorker::CaseFixerWorker(QObject *parent):
    QObject(parent),
    m_mode(MODE_TOLOWER),
    m_jobRunning(false),
    m_currentValue(0),
    m_isFine(false)
{

}

CaseFixerWorker::~CaseFixerWorker()
{

}

bool CaseFixerWorker::initJob(QString configPack, QString episodePath, bool recursive, int targetMode)
{
    m_filesToConvert.clear();
    m_errorString.clear();
    m_episode.setPath(episodePath);
    m_mode = targetMode;
    if(!m_episode.setCurrent(episodePath))
    {
        m_errorString = tr("Can't resolve path \"%1\"").arg(episodePath);
        return false;
    }

    m_episodeBox.openEpisode(episodePath, recursive);

    if(m_episodeBox.totalElements()==0)
    {
        m_errorString = tr("No files to convert");
        return false;
    }

    emit totalElements(m_episodeBox.totalElements());

    return true;
}

bool CaseFixerWorker::runJob()
{
    m_jobRunning = true;
    m_isFine = false;

    try
    {
        // TODO: Put code HERE
    }
    catch(QString err)
    {
        m_errorString = "Error ocouped while conversion process: " + err;
        emit workFinished(false);
        m_isFine = false;
    }

    m_jobRunning = false;
    qDebug() << "Work done, exiting";
    emit workFinished(true);
    return m_isFine;
}

QString CaseFixerWorker::errorString()
{
    return m_errorString;
}




CaseFixer::CaseFixer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CaseFixer)
{
    ui->setupUi(this);

    ui->progress->reset();

    connect(&m_progressWatcher, &QTimer::timeout,
                   this, &CaseFixer::refreshProgressBar,
                   Qt::QueuedConnection);

    connect(&m_worker, &CaseFixerWorker::totalElements,
                   ui->progress, &QProgressBar::setMaximum,
                   Qt::QueuedConnection);

    connect(&m_worker, &CaseFixerWorker::workFinished,
            this, &CaseFixer::workFinished, Qt::QueuedConnection);
}

CaseFixer::~CaseFixer()
{
    delete ui;
}

void CaseFixer::on_episodeBrowse_clicked()
{

}

void CaseFixer::on_configPackBrowse_clicked()
{

}

void CaseFixer::workFinished(bool isFine)
{
    m_process.waitForFinished();
    m_progressWatcher.stop();
    ui->progress->reset();
    emit setLocked(false);

    if(isFine)
        QMessageBox::information(this, tr("Work finished"), tr("Case sensitive fixing work has been converted!"));
    else
        QMessageBox::warning(this,
                             tr("Error occouped while work process"),
                             tr("Case fixer was aborted with an error: %1")
                             .arg(m_worker.errorString()));
}

void CaseFixer::refreshProgressBar()
{
    ui->progress->setValue(m_worker.m_currentValue);
}
