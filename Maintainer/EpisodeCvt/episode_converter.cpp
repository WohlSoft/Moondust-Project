#include "episode_converter.h"
#include "ui_episode_converter.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDateTime>
#include <QCloseEvent>
#include <QtConcurrent>
#include <QtConcurrentRun>
#include <common_features/app_path.h>

#include <QDebug>

EpisodeConverterWorker::EpisodeConverterWorker(QObject *parent) :
    QObject(parent),
    m_doBackup(true),
    m_targetFormat(0),
    m_targetFormatVer(64),
    m_jobRunning(false),
    m_currentValue(0),
    m_isFine(false)
{}

EpisodeConverterWorker::~EpisodeConverterWorker()
{}

void EpisodeConverterWorker::setBackup(bool enabled)
{
    m_doBackup = enabled;
}

bool EpisodeConverterWorker::initJob(QString path, bool recursive, int targetFormat, int fmtVer)
{
    m_filesToConvert.clear();
    m_errorString.clear();
    m_episode.setPath(path);
    m_targetFormat = targetFormat;
    m_targetFormatVer = fmtVer;
    if(!m_episode.setCurrent(path))
    {
        m_errorString = tr("Can't resolve path \"%1\"").arg(path);
        return false;
    }

    m_episodeBox.openEpisode(path, recursive);

    if(m_episodeBox.totalElements()==0)
    {
        m_errorString = tr("No files to convert");
        return false;
    }

    emit totalElements(m_episodeBox.totalElements());

    return true;
}

bool EpisodeConverterWorker::runJob()
{
    m_jobRunning = true;
    m_isFine = false;

    QString BackupDirectory = QString("pge_maintainer_backup-%1-%2-%3_%4-%5-%6")
            .arg(QDate().currentDate().year())
            .arg(QDate().currentDate().month())
            .arg(QDate().currentDate().day())
            .arg(QTime().currentTime().hour())
            .arg(QTime().currentTime().minute())
            .arg(QTime().currentTime().second());
    m_currentValue = 0;

    try
    {
        for(int i=0; i<m_episodeBox.d.size(); i++)
        {
            EpisodeBox_level& lvl = m_episodeBox.d[i];
            qDebug() << "open level" << lvl.fPath;
            QString relDirPath = m_episode.relativeFilePath( lvl.fPath );
            int idx = relDirPath.lastIndexOf('/');
            if(idx >= 0)
                relDirPath.remove(idx, relDirPath.size()-idx);
            else
                relDirPath.clear();

            if(relDirPath.startsWith("pge_maintainer_backup-", Qt::CaseInsensitive))
                continue;

            qDebug() << "Will be processed";

            if(m_doBackup)
            {
                qDebug() << "Make backup";
                QFileInfo oldFile(lvl.fPath);
                m_episode.mkpath(BackupDirectory + "/" + relDirPath);
                QFile::copy(lvl.fPath, BackupDirectory+"/" + relDirPath + "/" + oldFile.fileName() );
            }
            QString oldPath = lvl.fPath;

            switch(m_targetFormat)
            {
            case 0://PGE-X
                qDebug() << "Make LVLX";
                lvl.ftype = EpisodeBox_level::F_LVLX;
                if(lvl.fPath.endsWith(".lvl", Qt::CaseInsensitive))
                    lvl.fPath.append("x");
                else if(!lvl.fPath.endsWith(".lvlx", Qt::CaseInsensitive))
                    lvl.fPath.append(".lvlx");
                if(!FileFormats::SaveLevelFile(lvl.d, lvl.fPath, FileFormats::LVL_PGEX))
                    throw(FileFormats::errorString);
                break;
            case 1://SMBX 1...64
                qDebug() << "Make LVL SMBX" << lvl.ftypeVer;
                lvl.ftype = EpisodeBox_level::F_LVL;
                lvl.ftypeVer = m_targetFormatVer;
                if(lvl.fPath.endsWith(".lvlx", Qt::CaseInsensitive))
                    lvl.fPath.remove(lvl.fPath.size()-1, 1);
                else if(!lvl.fPath.endsWith(".lvl", Qt::CaseInsensitive))
                    lvl.fPath.append(".lvl");
                if(!FileFormats::SaveLevelFile(lvl.d, lvl.fPath, FileFormats::LVL_SMBX64, m_targetFormatVer))
                    throw(FileFormats::errorString);
                break;
            case 2://SMBX-38A
                qDebug() << "Make LVL SMBX-38a";
                lvl.ftype = EpisodeBox_level::F_LVL38A;
                if(lvl.fPath.endsWith(".lvlx", Qt::CaseInsensitive))
                    lvl.fPath.remove(lvl.fPath.size()-1, 1);
                else if(!lvl.fPath.endsWith(".lvl", Qt::CaseInsensitive))
                    lvl.fPath.append(".lvl");
                if(!FileFormats::SaveLevelFile(lvl.d, lvl.fPath, FileFormats::LVL_SMBX38A))
                    throw(FileFormats::errorString);
                break;
            }

            if(oldPath != lvl.fPath)
            {
                qDebug() << "Rename all levels";
                m_episodeBox.renameLevel(oldPath, lvl.fPath);
                qDebug() << "Remove old file";
                QFile::remove(oldPath);
            }

            qDebug() << "do next";
            m_currentValue++;
        }

        for(int i=0; i<m_episodeBox.dw.size(); i++)
        {
            EpisodeBox_world& wld = m_episodeBox.dw[i];
            qDebug() << "Open world map" << wld.fPath;
            QString relDirPath = m_episode.relativeFilePath( wld.fPath );
            int idx = relDirPath.lastIndexOf('/');
            if(idx >= 0)
                relDirPath.remove(idx, relDirPath.size()-idx);
            else
                relDirPath.clear();

            if(relDirPath.startsWith("pge_maintainer_backup-", Qt::CaseInsensitive))
                continue;

            qDebug() << "Will be processed";

            if(m_doBackup)
            {
                qDebug() << "Make backup";
                QFileInfo oldFile(wld.fPath);
                m_episode.mkpath(BackupDirectory + "/" + relDirPath);
                QFile::copy(wld.fPath, BackupDirectory+"/" + relDirPath + "/" + oldFile.fileName() );
            }
            QString oldPath = wld.fPath;

            switch(m_targetFormat)
            {
            case 0://PGE-X
                qDebug() << "Make WLDX";
                wld.ftype = EpisodeBox_world::F_WLDX;
                if(wld.fPath.endsWith(".wld", Qt::CaseInsensitive))
                    wld.fPath.append("x");
                else if(!wld.fPath.endsWith(".wldx", Qt::CaseInsensitive))
                    wld.fPath.append(".wldx");
                if(!FileFormats::SaveWorldFile(wld.d, wld.fPath, FileFormats::WLD_PGEX))
                    throw(FileFormats::errorString);
                break;
            case 1://SMBX 1...64
                qDebug() << "Make WLD SMBX "<<wld.ftypeVer;
                wld.ftype = EpisodeBox_world::F_WLD;
                wld.ftypeVer = m_targetFormatVer;
                if(wld.fPath.endsWith(".wldx", Qt::CaseInsensitive))
                    wld.fPath.remove(wld.fPath.size()-1, 1);
                else if(!wld.fPath.endsWith(".wld", Qt::CaseInsensitive))
                    wld.fPath.append(".wld");
                if(!FileFormats::SaveWorldFile(wld.d, wld.fPath, FileFormats::WLD_SMBX64, m_targetFormatVer))
                    throw(FileFormats::errorString);
                break;
            case 2://SMBX-38A
                qDebug() << "Make WLD SMBX-38a";
                wld.ftype = EpisodeBox_world::F_WLD38A;
                if(wld.fPath.endsWith(".wldx", Qt::CaseInsensitive))
                    wld.fPath.remove(wld.fPath.size()-1, 1);
                else if(!wld.fPath.endsWith(".wld", Qt::CaseInsensitive))
                    wld.fPath.append(".wld");
                if(!FileFormats::SaveWorldFile(wld.d, wld.fPath, FileFormats::WLD_SMBX38A))
                    throw(FileFormats::errorString);
                break;
            }

            if(oldPath != wld.fPath)
            {
                qDebug() << "Delete old file";
                //m_episodeBox.renameLevel(oldPath, wld.fPath);
                QFile::remove(oldPath);
            }

            qDebug() << "Process next";
            m_currentValue++;
        }

        m_isFine=true;
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

QString EpisodeConverterWorker::errorString()
{
    return m_errorString;
}






EpisodeConverter::EpisodeConverter(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EpisodeConverter)
{
    ui->setupUi(this);
    ui->progressBar->reset();

    connect(&m_progressWatcher, &QTimer::timeout,
                   this, &EpisodeConverter::refreshProgressBar,
                   Qt::QueuedConnection);

    connect(&m_worker, &EpisodeConverterWorker::totalElements,
                   ui->progressBar, &QProgressBar::setMaximum,
                   Qt::QueuedConnection);

    connect(&m_worker, &EpisodeConverterWorker::workFinished,
            this, &EpisodeConverter::workFinished, Qt::QueuedConnection);

    connect(this, &EpisodeConverter::setLocked, ui->DoMadJob, &QPushButton::setDisabled);
    connect(this, &EpisodeConverter::setLocked, ui->lookForSubDirs, &QCheckBox::setDisabled);
    connect(this, &EpisodeConverter::setLocked, ui->makeBacup, &QCheckBox::setDisabled);
    connect(this, &EpisodeConverter::setLocked, ui->closeBox, &QPushButton::setDisabled);
    connect(this, &EpisodeConverter::setLocked, ui->episodePath, &QLineEdit::setDisabled);
    connect(this, &EpisodeConverter::setLocked, ui->browse, &QPushButton::setDisabled);
    connect(this, &EpisodeConverter::setLocked, ui->targetFormat, &QComboBox::setDisabled);
    connect(this, &EpisodeConverter::setLocked, ui->targetFormatVer, &QSpinBox::setDisabled);
}

EpisodeConverter::~EpisodeConverter()
{
    delete ui;
}

void EpisodeConverter::closeEvent(QCloseEvent *cl)
{
    if(m_process.isRunning())
        cl->ignore();
    else
        cl->accept();
}

void EpisodeConverter::on_closeBox_clicked()
{
    this->close();
}

void EpisodeConverter::on_DoMadJob_clicked()
{
    if(ui->episodePath->text().isEmpty() || !QDir(ui->episodePath->text()).exists())
    {
        QMessageBox::warning(this,
                             tr("Episode path error"),
                             tr("Episode path wasn't declared. Please choice target episode path first."));
        return;
    }

    m_worker.setBackup(ui->makeBacup->isChecked());
    emit setLocked(true);

    m_process = QtConcurrent::run<bool>(&m_worker,
                                        &EpisodeConverterWorker::initJob,
                                        ui->episodePath->text(),
                                        ui->lookForSubDirs->isChecked(),
                                        ui->targetFormat->currentIndex(),
                                        ui->targetFormatVer->value());

    while(m_process.isRunning())
    {
        qApp->processEvents(); QThread::msleep(10);
    }

    if(!m_process.result())
    {
        QMessageBox::warning(this,
                             tr("Worker error"),
                             tr("Can't initialize job because %1").arg(m_worker.errorString()));
        return;
    }

    m_process = QtConcurrent::run<bool>(&m_worker, &EpisodeConverterWorker::runJob);
    m_progressWatcher.start(100);
}

void EpisodeConverter::on_browse_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open target episode path"),
                                               (ui->episodePath->text().isEmpty() ? ApplicationPath : ui->episodePath->text())
                                                    );
    if(dir.isEmpty())
        return;

    ui->episodePath->setText(dir);
}

void EpisodeConverter::workFinished(bool isFine)
{
    m_process.waitForFinished();
    m_progressWatcher.stop();
    ui->progressBar->reset();
    emit setLocked(false);

    if(isFine)
        QMessageBox::information(this, tr("Work finished"), tr("Episode has been converted!"));
    else
        QMessageBox::warning(this,
                             tr("Error occouped while work process"),
                             tr("Episode conversion aborted with error: %1")
                             .arg(m_worker.errorString()));
}

void EpisodeConverter::refreshProgressBar()
{
    ui->progressBar->setValue(m_worker.m_currentValue);
}
