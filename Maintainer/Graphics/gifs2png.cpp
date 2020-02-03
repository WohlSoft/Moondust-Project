#include "gifs2png.h"
#include "ui_gifs2png.h"
#include <QMimeData>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMessageBox>
#include <QCloseEvent>
#include <QFileDialog>
#include <QSettings>
#include <signal.h>
#include <common_features/app_path.h>

GIFs2PNG::GIFs2PNG(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GIFs2PNG)
{
    ui->setupUi(this);
#ifdef _WIN32
    m_gifs2png.setProgram(ApplicationPath_x + "/GIFs2PNG.exe");
#else
    m_gifs2png.setProgram(ApplicationPath_x + "/GIFs2PNG");
#endif
    m_gifs2png.setProcessChannelMode(QProcess::MergedChannels);
    QObject::connect(&m_gifs2png, &QProcess::readyReadStandardError,
                     this, &GIFs2PNG::processOutput);
    QObject::connect(&m_gifs2png, &QProcess::readyReadStandardOutput,
                     this, &GIFs2PNG::processOutput);
    QObject::connect(&m_gifs2png, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                     this, &GIFs2PNG::processFinished);
    findConfigPacks();
}

GIFs2PNG::~GIFs2PNG()
{
    delete ui;
}

void GIFs2PNG::closeEvent(QCloseEvent *e)
{
    if(m_gifs2png.state() == QProcess::Running)
    {
        e->ignore();
        return;
    }
}

void GIFs2PNG::dragEnterEvent(QDragEnterEvent *e)
{
    if(ui->whatToConvert->currentWidget() != ui->filesTab)
        return; // Don't accept drop when drop area tab isn't active
    if(e->mimeData()->hasUrls())
        e->acceptProposedAction();
}

void GIFs2PNG::dropEvent(QDropEvent *e)
{
    this->raise();
    this->setFocus(Qt::ActiveWindowFocusReason);

    if(m_gifs2png.state() == QProcess::Running)
    {
        QMessageBox::information(this,
                                 tr("Process is busy"),
                                 tr("Can't convert these files while already running conversion!"));
        return;
    }

    QStringList filesToConvert;
    for(const QUrl &url : e->mimeData()->urls())
    {
        const QString &fileName = url.toLocalFile();
        if(fileName.endsWith(".gif", Qt::CaseSensitive))
            filesToConvert.push_back(fileName);
    }

    if(!filesToConvert.isEmpty())
    {
        m_gifs2png.setArguments(filesToConvert);
        ui->outputLog->clear();
        m_gifs2png.start();
        toggleBusy(true);
    }
}

void GIFs2PNG::on_doMadJob_clicked()
{
    if(m_gifs2png.state() == QProcess::Running)
    {
        m_gifs2png.terminate();
        return;
    }

    QString configPack = ui->configPackPath->text();
    QString episodePath = ui->inPath->text();

    QStringList args;

    if(!configPack.isEmpty())
    {
        QDir cpDir(configPack);
        if(!cpDir.exists())
        {
            QMessageBox::warning(this,
                                 tr("Incorrect input"),
                                 tr("Given config pack directory doesn't exist."));
            return;
        }

        if(!QFile::exists(cpDir.absolutePath() + "/main.ini"))
        {
            QMessageBox::warning(this,
                                 tr("Incorrect input"),
                                 tr("Given config pack directory path is not a config pack."));
            return;
        }

        args << "--config" << configPack;
    }

    if(episodePath.isEmpty())
    {
        QMessageBox::warning(this,
                             tr("Incorrect input"),
                             tr("You should give an input directory to start the conversion."));
        return;
    }

    QDir epDir(episodePath);
    if(!epDir.exists())
    {
        QMessageBox::warning(this,
                             tr("Incorrect input"),
                             tr("Given input directory path doesn't exist."));
        return;
    }

    if(ui->flagScanSubdirs->isChecked())
        args << "-d";

    if(ui->flagRemoveOld->isChecked())
        args << "-r";

    args << "--" << episodePath;

    m_gifs2png.setArguments(args);
    ui->outputLog->clear();
    m_gifs2png.start();
    toggleBusy(true);
}

void GIFs2PNG::on_configPackPathBrowse_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this,
                                                    tr("Select a config pack directory"),
                                                    ui->configPackPath->text());
    if(dir.isEmpty())
        return;
    ui->configPackPath->setText(dir);
}

void GIFs2PNG::on_configPackChoose_currentIndexChanged(int)
{
    QVariant d = ui->configPackChoose->currentData();
    if(d.isNull() || !d.isValid())
        return;
    ui->configPackPath->setText(d.toString());
}

void GIFs2PNG::on_inPathBrowse_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this,
                                                    tr("Select an input directory"),
                                                    ui->inPath->text());
    if(dir.isEmpty())
        return;
    ui->inPath->setText(dir);
}

void GIFs2PNG::processOutput()
{
    while(1)
    {
        QByteArray out = m_gifs2png.readLine(0);
        if(out.isEmpty())
            break;
        out.replace("\r\n", "");
        out.replace("\n", "");
        ui->outputLog->append(QString::fromLocal8Bit(out));
    }
}

void GIFs2PNG::processFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    ui->outputLog->append(tr("Process was exited with a code %1.").arg(exitCode));

    this->raise();
    this->setFocus(Qt::ActiveWindowFocusReason);

    if(exitCode != 0 || exitStatus != QProcess::NormalExit)
    {
        if(exitCode == SIGTERM)
        {
            QMessageBox::warning(this,
                                 tr("Process was terminated"),
                                 tr("Conversion has been termiated."));
        }
        else
        {
            QMessageBox::warning(this,
                                 tr("Unsuccess exit"),
                                 tr("Conversion was been completed with errors. "
                                    "Please look the output log for more detailed information."));
        }
    }
    toggleBusy(false);
}

void GIFs2PNG::findConfigPacks()
{
    QStringList possibleConfigPacks;
    QString path;
    if(AppPathManager::userDirIsAvailable())
    {
        path = AppPathManager::userAppDir() + "/configs";
        QDir dp(path);
        if(dp.exists())
        {
            for(QString &p : dp.entryList(QDir::Dirs|QDir::NoDotAndDotDot))
            {
                possibleConfigPacks.push_back(path + QStringLiteral("/") + p);
            }
        }
    }

    path = ApplicationPath + "/configs";
    QDir dpa(ApplicationPath + "/configs");
    if(dpa.exists())
    {
        for(QString &p : dpa.entryList(QDir::Dirs|QDir::NoDotAndDotDot))
        {
            possibleConfigPacks.push_back(path + QStringLiteral("/") + p);
        }
    }

    for(const QString &p : possibleConfigPacks)
    {
        QString main_ini = p + "/main.ini";
        if(!QFile::exists(main_ini))
            continue;  // Is not a config pack
        QSettings m(main_ini, QSettings::IniFormat);
        if(m.status() != QSettings::NoError)
            continue;  // Invalid main.ini

        m.beginGroup("main");
        QVariant title_v = m.value("config_name", QVariant());
        m.endGroup();

        if(title_v.isNull() || !title_v.isValid())
            continue;  //Invalid data

        ui->configPackChoose->addItem(title_v.toString(), p);
    }
}

void GIFs2PNG::toggleBusy(bool busy)
{
    ui->closeBox->setEnabled(!busy);
    ui->whatToConvert->setEnabled(!busy);
    if(busy)
    {
        ui->doMadJob->setText(tr("Stop"));
    } else {
        ui->doMadJob->setText(tr("Start"));
    }
}
