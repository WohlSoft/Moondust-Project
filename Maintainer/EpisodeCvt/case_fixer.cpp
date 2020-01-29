#include "case_fixer.h"
#include "ui_case_fixer.h"
#include <QtConcurrent>
#include <QDirIterator>
#include <QMessageBox>
#include <QEventLoop>
#include <QCryptographicHash>


// Returns empty QByteArray() on failure.
static QByteArray fileChecksum(const QString &fileName, QCryptographicHash::Algorithm hashAlgorithm)
{
    QFile f(fileName);
    if(f.open(QFile::ReadOnly))
    {
        QCryptographicHash hash(hashAlgorithm);
        if (hash.addData(&f))
        {
            return hash.result();
        }
    }
    return QByteArray();
}

static bool renameFiles(const QString &oldFile, const QString &newFile)
{
    if(!QFile::rename(oldFile, newFile))
    {
        auto hashOld = fileChecksum(oldFile, QCryptographicHash::Sha256);
        auto hashNew = fileChecksum(newFile, QCryptographicHash::Sha256);
        if(hashOld == hashNew) // If two files of different case are matching, remove old
        {
            QFile::remove(oldFile);
        }
        else // Else, keep "new" file as "_old" file
        {
            QFileInfo o(newFile);
            QString newOldFile = newFile;
            {
                int cutTo = (newOldFile.size() - 1) - o.suffix().size();
                newOldFile.remove(cutTo, o.suffix().size());
                newOldFile.append("_old." + o.suffix());
            }
            QFile::rename(newFile, newOldFile);
            QFile::rename(oldFile, newFile);
        }
    }
    return true;
}

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
        m_errorString = tr("No files to scan");
        return false;
    }

    return true;
}

bool CaseFixerWorker::runJob()
{
    m_jobRunning = true;
    m_isFine = false;

    try
    {
        emit totalElements(0);

        // Find all files in the episode
        QDirIterator it(m_episode.path(), QDir::Files, QDirIterator::Subdirectories);
        QDir d(m_episode.path());
        while (it.hasNext())
        {
            it.next();
            QFileInfo q = it.fileInfo();
            m_filesToConvert.append(q.absoluteFilePath());
            qDebug() << d.relativeFilePath(q.absoluteFilePath());
        }
        emit totalElements(m_filesToConvert.size());
        m_currentValue = 0;

        // For each found file, check is it referred anywhere, if yes, do anything
        for(const QString &p : m_filesToConvert)
        {
            qDebug() << "Trying to process " << p << "...";
            switch(m_mode)
            {
            case MODE_TOLOWER:
            {
                QString f = m_episodeBox.findFileAliasCaseInsensitive(p);
                if(f.isEmpty())
                    break; // Do nothing if no any usage alias found
                QFileInfo pi(p);
                QString fl = pi.absoluteDir().absolutePath() + QStringLiteral("/") + pi.fileName().toLower();
                qDebug() << "Rename file: " << p << " => " << fl;
                renameFiles(p, fl);
                qDebug() << "Rename setup: " << p << " => " << fl;
                m_episodeBox.renameFile(p, fl);
                break;
            }
            default:
            case MODE_MATCH_FS:
            {
                QString f = m_episodeBox.findFileAliasCaseInsensitive(p);
                if(f.isEmpty())
                    break; // Do nothing if no any usage alias found
                QString fAbs = d.absoluteFilePath(f);
                qDebug() << "Rename setup: " << fAbs << " => " << p;
                m_episodeBox.renameFile(fAbs, p);
                break;
            }
            case MODE_MATCH_SETUP:
            {
                QString f = m_episodeBox.findFileAliasCaseInsensitive(p);
                if(f.isEmpty())
                    break; // Do nothing if no any usage alias found
                QString fAbs = d.absoluteFilePath(f);
                qDebug() << "Rename file: " << p << " => " << fAbs;
                renameFiles(p, d.absoluteFilePath(f));
                break;
            }
            }

            m_currentValue++;
        }
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

void CaseFixer::on_start_clicked()
{
    if(ui->episodeToFix->text().isEmpty() || !QDir(ui->episodeToFix->text()).exists())
    {
        QMessageBox::warning(this,
                             tr("Episode path error"),
                             tr("Episode path wasn't declared. Please choice target episode path first."));
        return;
    }
    emit setLocked(true);

    int mode = 0;

    if(ui->modeMatchFields->isChecked())
        mode = CaseFixerWorker::MODE_MATCH_SETUP;
    else if(ui->modeMatchFS->isChecked())
        mode = CaseFixerWorker::MODE_MATCH_FS;
    else if(ui->modeAllLower->isChecked())
        mode = CaseFixerWorker::MODE_TOLOWER;

    m_process = QtConcurrent::run<bool>(&m_worker,
                                        &CaseFixerWorker::initJob,
                                        ui->configPackPath->text(),
                                        ui->episodeToFix->text(),
                                        true, mode);

    while(m_process.isRunning())
    {
        qApp->processEvents();
        QThread::msleep(10);
    }

    if(!m_process.result())
    {
        QMessageBox::warning(this,
                             tr("Worker error"),
                             tr("Can't initialize job because %1").arg(m_worker.errorString()));
        return;
    }

    m_process = QtConcurrent::run<bool>(&m_worker, &CaseFixerWorker::runJob);
    m_progressWatcher.start(100);
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
