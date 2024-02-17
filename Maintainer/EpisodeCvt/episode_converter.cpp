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

#include "qfile_dialogs_default_options.hpp"


EpisodeConverterWorker::EpisodeConverterWorker(QObject *parent) :
    QObject(parent),
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

bool EpisodeConverterWorker::initJob(const JobSetup &setup)
{
    m_filesToConvert.clear();
    m_errorString.clear();
    m_episode.setPath(setup.path);

    m_targetFormat = setup.targetFormat;
    m_targetFormatVer = setup.fmtVer;
    m_targetFormatWld = setup.targetFormatWld;
    m_targetFormatVerWld = setup.fmtVerWld;
    m_applyTheXTechSetup = setup.applyTheXTechSetup;

    m_dstCpId = setup.cpId;

    if(!m_episode.setCurrent(setup.path))
    {
        m_errorString = tr("Can't resolve path \"%1\"").arg(setup.path);
        return false;
    }

    m_episodeBox.openEpisode(setup.path, setup.recursive);

    if(m_episodeBox.totalElements() == 0)
    {
        m_errorString = tr("No files to convert");
        return false;
    }

    emit totalElements(m_episodeBox.totalElements());

    return true;
}

static bool preparePath(QDir &episode, QString &relDirPath, QString &inPath, QString &backupTo, bool doBackup)
{
    relDirPath = episode.relativeFilePath(inPath);
    int idx = relDirPath.lastIndexOf('/');
    if(idx >= 0)
        relDirPath.remove(idx, relDirPath.size() - idx);
    else
        relDirPath.clear();

    bool ret = relDirPath.startsWith("pge_maintainer_backup-", Qt::CaseInsensitive);

    if(!ret && doBackup)
    {
        qDebug() << "Make backup";
        QFileInfo oldFile(inPath);
        episode.mkpath(backupTo + "/" + relDirPath);
        QFile::copy(inPath, backupTo + "/" + relDirPath + "/" + oldFile.fileName());
    }
    return ret;
}

static void renameExtension(QString &str, QString into)
{
    int dot = str.lastIndexOf('.');
    if(dot == -1)
        str.append(into);
    str.replace(dot, str.size() - dot, into);
}

static void applyTheXTechRules(LevelData &lvl)
{
    int ver = lvl.meta.RecentFormatVersion;

    for(auto &npc : lvl.npc)
    {
        switch(npc.id)
        {
        case 22: // NPCID_CANNONITEM
            if(ver < 28)
                npc.special_data = 2; // SMBX 1.1.x and 1.0.x behavior
            else if(ver < 51)
                npc.special_data = 1; // SMBX 1.2 behavior
            else
                npc.special_data = 0; // SMBX 1.2.1 and newer behavior, customizable behavior
            break;

        case 37: // NPCID_STONE_S3
            if(ver < 9)
                npc.special_data = 1; // Fall always
            else
                npc.special_data = 0; // Fall when lower than
            break;

        case 86: // NPCID_VILLAIN_S3
            if(ver < 30)
                npc.special_data = 1; // Hit all blocks and expand section
            else
                npc.special_data = 0;
            break;

        case 60: // NPCID_YEL_PLATFORM
        case 62: // NPCID_BLU_PLATFORM
        case 64: // NPCID_GRN_PLATFORM
        case 66: // NPCID_RED_PLATFORM
        case 104: // NPCID_PLATFORM_S3
        case 179: // NPCID_SAW
            if(ver < 30)
                npc.special_data = 1; // Hot spot is center of 96x32 rectangle
            else
                npc.special_data = 0; // Hot spot is center of 96x32 rectangle with 1 pixel offset down
            break;
        }
    }
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
        for(int i = 0; i < m_episodeBox.d.size(); i++)
        {
            EpisodeBox_level &lvl = m_episodeBox.d[i];
            QString relDirPath;
            QString oldPath = lvl.fPath;

            if(preparePath(m_episode, relDirPath, lvl.fPath, BackupDirectory, m_doBackup))
                continue;

            switch(m_targetFormat)
            {
            case 0://PGE-X
                qDebug() << "Make LVLX";
                lvl.ftype = EpisodeBox_level::F_LVLX;
                renameExtension(lvl.fPath, ".lvlx");

                if(!m_dstCpId.isEmpty())
                    lvl.d.meta.configPackId = m_dstCpId;

                if(m_applyTheXTechSetup && lvl.d.meta.RecentFormat == LevelData::SMBX64)
                    applyTheXTechRules(lvl.d);

                if(!FileFormats::SaveLevelFile(lvl.d, lvl.fPath, FileFormats::LVL_PGEX))
                    throw(lvl.d.meta.ERROR_info);

                break;
            case 1://SMBX 1...64
                qDebug() << "Make LVL SMBX" << lvl.ftypeVer;
                lvl.ftype = EpisodeBox_level::F_LVL;
                lvl.ftypeVer = m_targetFormatVer;
                renameExtension(lvl.fPath, ".lvl");

                if(!FileFormats::SaveLevelFile(lvl.d, lvl.fPath, FileFormats::LVL_SMBX64, static_cast<unsigned int>(m_targetFormatVer)))
                    throw(lvl.d.meta.ERROR_info);

                break;
            case 2://SMBX-38A
                qDebug() << "Make LVL SMBX-38a";
                lvl.ftype = EpisodeBox_level::F_LVL38A;
                renameExtension(lvl.fPath, ".lvl");

                if(!FileFormats::SaveLevelFile(lvl.d, lvl.fPath, FileFormats::LVL_SMBX38A, static_cast<unsigned int>(m_targetFormatVer)))
                    throw(lvl.d.meta.ERROR_info);

                break;
            }

            if(oldPath != lvl.fPath)
            {
                m_episodeBox.renameLevel(oldPath, lvl.fPath);
                QFile::remove(oldPath);
            }
            qDebug() << "Successfully!";
            m_currentValue++;
        }

        for(int i = 0; i < m_episodeBox.dw.size(); i++)
        {
            EpisodeBox_world &wld = m_episodeBox.dw[i];
            qDebug() << "Open world map" << wld.fPath;
            QString relDirPath;
            if(preparePath(m_episode, relDirPath, wld.fPath, BackupDirectory, m_doBackup))
                continue;

            qDebug() << "Will be processed";

            if(m_doBackup)
            {
                qDebug() << "Make backup";
                QFileInfo oldFile(wld.fPath);
                m_episode.mkpath(BackupDirectory + "/" + relDirPath);
                QFile::copy(wld.fPath, BackupDirectory + "/" + relDirPath + "/" + oldFile.fileName());
            }

            QString oldPath = wld.fPath;

            switch(m_targetFormatWld)
            {
            case 0://PGE-X
                qDebug() << "Make WLDX";
                wld.ftype = EpisodeBox_world::F_WLDX;
                renameExtension(wld.fPath, ".wldx");

                if(!m_dstCpId.isEmpty())
                    wld.d.meta.configPackId = m_dstCpId;

                if(!FileFormats::SaveWorldFile(wld.d, wld.fPath, FileFormats::WLD_PGEX))
                    throw(wld.d.meta.ERROR_info);

                break;
            case 1://SMBX 1...64
                qDebug() << "Make WLD SMBX " << wld.ftypeVer;
                wld.ftype = EpisodeBox_world::F_WLD;
                wld.ftypeVer = m_targetFormatVerWld;
                renameExtension(wld.fPath, ".wld");

                if(!FileFormats::SaveWorldFile(wld.d, wld.fPath, FileFormats::WLD_SMBX64, static_cast<unsigned int>(m_targetFormatVerWld)))
                    throw(wld.d.meta.ERROR_info);

                break;
            case 2://SMBX-38A
                qDebug() << "Make WLD SMBX-38a";
                wld.ftype = EpisodeBox_world::F_WLD38A;
                renameExtension(wld.fPath, ".wld");

                if(!FileFormats::SaveWorldFile(wld.d, wld.fPath, FileFormats::WLD_SMBX38A, static_cast<unsigned int>(m_targetFormatVerWld)))
                    throw(wld.d.meta.ERROR_info);

                break;
            }

            if(oldPath != wld.fPath)
            {
                qDebug() << "Delete old file";
                //m_episodeBox.renameLevel(oldPath, wld.fPath);
                QFile::remove(oldPath);
            }
            qDebug() << "Successfully!";
            m_currentValue++;
        }

        m_isFine = true;
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






void EpisodeConverter::buildEnginesList()
{
    m_enginesList.clear();
    int idx = 0;

    ui->respectPre13OptionsSimple->setEnabled(false);

    m_enginesList.push_back({"none",                        "",         idx++,  LevelData::PGEX,    -1,     WorldData::PGEX,    -1, false});
    m_enginesList.push_back({"Moondust Engine",             "",         idx++,  LevelData::PGEX,    -1,     WorldData::PGEX,    -1, false});
    m_enginesList.push_back({"TheXTech Engine",             "TheXTech", idx++,  LevelData::PGEX,    -1,     WorldData::PGEX,    -1, true});

    m_enginesList.push_back({"SMBX2 (X2) Beta 3/4/5",       "SMBX2",    idx++,  LevelData::PGEX,    -1,     WorldData::SMBX64,  64, false});

    m_enginesList.push_back({"SMBX-38A v1.4.5 (69)",        "",         idx++,  LevelData::SMBX38A, 69,     WorldData::SMBX38A, 69, false});
    m_enginesList.push_back({"SMBX-38A v1.4.4 (68)",        "",         idx++,  LevelData::SMBX38A, 68,     WorldData::SMBX38A, 68, false});
    m_enginesList.push_back({"SMBX-38A v1.4.3 (67)",        "",         idx++,  LevelData::SMBX38A, 67,     WorldData::SMBX38A, 67, false});
    m_enginesList.push_back({"SMBX-38A v1.4.2 (66)",        "",         idx++,  LevelData::SMBX38A, 66,     WorldData::SMBX38A, 66, false});
    m_enginesList.push_back({"SMBX-38A v1.4.1 (65)",        "",         idx++,  LevelData::SMBX38A, 65,     WorldData::SMBX38A, 65, false});
    m_enginesList.push_back({"SMBX-38A v1.4 (64)",          "",         idx++,  LevelData::SMBX38A, 64,     WorldData::SMBX38A, 64, false});

    m_enginesList.push_back({"SMBX v1.3 (v1.3.0.1) (64)",   "",         idx++, LevelData::SMBX64,  64,     WorldData::SMBX64,  64, false});
    m_enginesList.push_back({"SMBX v1.2.2 (Beta 59)",       "",         idx++,  LevelData::SMBX64,  59,     WorldData::SMBX64,  59, false});
    m_enginesList.push_back({"SMBX v1.2.2 (58)",            "",         idx++,  LevelData::SMBX64,  58,     WorldData::SMBX64,  58, false});
    m_enginesList.push_back({"SMBX v1.2.1 (51)",            "",         idx++,  LevelData::SMBX64,  51,     WorldData::SMBX64,  51, false});
    m_enginesList.push_back({"SMBX v1.2 (50)",              "",         idx++,  LevelData::SMBX64,  50,     WorldData::SMBX64,  50, false});
    m_enginesList.push_back({"SMBX v1.2.0 Beta 4 (32)",     "",         idx++,  LevelData::SMBX64,  32,     WorldData::SMBX64,  32, false});
    m_enginesList.push_back({"SMBX v1.2.0 Beta 3 (28)",     "",         idx++,  LevelData::SMBX64,  28,     WorldData::SMBX64,  28, false});
    m_enginesList.push_back({"SMBX v1.1.2 Beta 3 (20)",     "",         idx++,  LevelData::SMBX64,  20,     WorldData::SMBX64,  20, false});
    m_enginesList.push_back({"SMBX v1.1.1 (18)",            "",         idx++,  LevelData::SMBX64,  18,     WorldData::SMBX64,  18, false});
    m_enginesList.push_back({"SMBX v1.0.2 (2)",             "",         idx++,  LevelData::SMBX64,   2,     WorldData::SMBX64,   2, false});
    m_enginesList.push_back({"SMBX v1.0 (1)",               "",         idx++,  LevelData::SMBX64,   1,     WorldData::SMBX64,   1, false});

    ui->targetEngine->clear();
    ui->targetEngine->addItem(tr("[Select an engine]"));

    for(int i = 1; i < m_enginesList.size(); ++i)
        ui->targetEngine->addItem(m_enginesList[i].engineName);
}

EpisodeConverter::EpisodeConverter(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EpisodeConverter)
{
    ui->setupUi(this);
    ui->progressBar->reset();

    QObject::connect(&m_progressWatcher, &QTimer::timeout,
                     this, &EpisodeConverter::refreshProgressBar,
                     Qt::QueuedConnection);

    QObject::connect(&m_worker, &EpisodeConverterWorker::totalElements,
                     ui->progressBar, &QProgressBar::setMaximum,
                     Qt::QueuedConnection);

    QObject::connect(&m_worker, &EpisodeConverterWorker::workFinished,
                     this, &EpisodeConverter::workFinished, Qt::QueuedConnection);

    QObject::connect(this, &EpisodeConverter::setLocked, ui->DoMadJob, &QPushButton::setDisabled);
    QObject::connect(this, &EpisodeConverter::setLocked, ui->lookForSubDirs, &QCheckBox::setDisabled);
    QObject::connect(this, &EpisodeConverter::setLocked, ui->makeBacup, &QCheckBox::setDisabled);
    QObject::connect(this, &EpisodeConverter::setLocked, ui->closeBox, &QPushButton::setDisabled);
    QObject::connect(this, &EpisodeConverter::setLocked, ui->episodePath, &QLineEdit::setDisabled);
    QObject::connect(this, &EpisodeConverter::setLocked, ui->browse, &QPushButton::setDisabled);
    QObject::connect(this, &EpisodeConverter::setLocked, ui->targetFormat, &QComboBox::setDisabled);
    QObject::connect(this, &EpisodeConverter::setLocked, ui->targetFormatVer, &QSpinBox::setDisabled);

    QObject::connect(this, &EpisodeConverter::setLocked, ui->convertModes, &QTabWidget::setDisabled);

    QObject::connect(ui->episodePath, &QLineEdit::editingFinished,
                     this, &EpisodeConverter::detectEpisodeFormat);

    ui->respectPre13Options->setEnabled(ui->targetFormat->currentIndex() == LevelData::PGEX &&
                                        ui->targetFormatWld->currentIndex() - 1 == WorldData::PGEX);

    QObject::connect(ui->targetFormat, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
    [this](int idx)->void
    {
        switch(idx)
        {
        case LevelData::SMBX64:
            ui->targetFormatVer->setEnabled(true);
            ui->targetFormatVerLabel->setEnabled(true);
            ui->respectPre13Options->setEnabled(false);
            ui->targetFormatVer->setMinimum(0);
            ui->targetFormatVer->setMaximum(64);
            break;

        case LevelData::SMBX38A:
            ui->targetFormatVer->setEnabled(true);
            ui->targetFormatVerLabel->setEnabled(true);
            ui->respectPre13Options->setEnabled(false);
            ui->targetFormatVer->setMinimum(64);
            ui->targetFormatVer->setMaximum(69);
            break;

        default:
            ui->targetFormatVer->setEnabled(false);
            ui->targetFormatVerLabel->setEnabled(false);
            ui->respectPre13OptionsSimple->setEnabled(ui->targetFormat->currentIndex() == LevelData::PGEX &&
                                                      ui->targetFormatWld->currentIndex() - 1 == WorldData::PGEX);
            break;
        }
    });

    QObject::connect(ui->targetFormatWld, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
    [this](int idx)->void
    {
        switch(idx - 1)
        {
        case WorldData::SMBX64:
            ui->targetFormatWldVer->setEnabled(true);
            ui->targetFormatWldVerLabel->setEnabled(true);
            ui->respectPre13Options->setEnabled(false);
            ui->targetFormatWldVer->setMinimum(0);
            ui->targetFormatWldVer->setMaximum(64);
            break;

        case WorldData::SMBX38A:
            ui->targetFormatWldVer->setEnabled(true);
            ui->targetFormatWldVerLabel->setEnabled(true);
            ui->respectPre13Options->setEnabled(false);
            ui->targetFormatWldVer->setMinimum(64);
            ui->targetFormatWldVer->setMaximum(69);
            break;

        default:
            ui->targetFormatWldVer->setEnabled(false);
            ui->targetFormatWldVerLabel->setEnabled(false);
            ui->respectPre13Options->setEnabled(ui->targetFormat->currentIndex() == LevelData::PGEX &&
                                                ui->targetFormatWld->currentIndex() - 1 == WorldData::PGEX);
            break;
        }
    });

    buildEnginesList();

    QObject::connect(ui->targetEngine, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
                     this, &EpisodeConverter::targetEngineSelected);
}

EpisodeConverter::~EpisodeConverter()
{
    delete ui;
}

void EpisodeConverter::targetEngineSelected(int idx)
{
    const auto &engine = m_enginesList[idx];

    ui->respectPre13OptionsSimple->setEnabled(engine.showTheXTechOptions);

    if(idx == 0)
        return;

    ui->targetFormat->setCurrentIndex(engine.levelFormat);
    ui->targetFormatWld->setCurrentIndex(engine.worldFormat + 1);

    ui->targetFormatVer->setValue(engine.levelFormatVersion);
    ui->targetFormatWldVer->setValue(engine.worldFormatVersion);

    ui->targetCPID->setText(engine.cpIdTag);
}

void EpisodeConverter::detectEpisodeFormat()
{
#ifdef DEBUG_BUILD
    // FIXME: Continue the experiment and implement the PROPER heuristic detector of the episode format

    QString dir = ui->episodePath->text();
    QDir d(dir);

    const QString thextechKey = "TheXTech";
    const QString smbx2Key = "SMBX2";

    ui->detectedFormatLabel->clear();

    if(!d.exists())
        return;

    int numWLDX = 0;
    int numLVLX = 0;

    int numLVL64 = 0;
    int numWLD64 = 0;
    unsigned int maxVerLVL64 = 0;
    unsigned int maxVerWLD64 = 0;

    int numLVL38A = 0;
    int numWLD38A = 0;
    unsigned int maxVerLVL38A = 0;
    unsigned int maxVerWLD38A = 0;

    int numSMBX2 = 0;
    int numTheXTech = 0;

    int numGIFs = 0;
    int numPNGs = 0;

    int numLunaDll = 0;
    int numLunaDllLua = 0;
    int numLunaLua = 0;

    int numMP3s = 0;
    int numNonMP3s = 0;

    const QStringList filter =
    {
        "*.lvl", "*.lvlx", "*.wld", "*.wldx",

        "block-*.gif",
        "background-*.gif",
        "background2-*.gif",
        "npc-*.gif",
        "tile-*.gif",
        "scene-*.gif",
        "path-*.gif",
        "level-*.gif",
        "player-*.gif",
        "yoshit-*.gif",
        "yoshib-*.gif",

        "block-*.png",
        "background-*.png",
        "background2-*.png",
        "npc-*.png",
        "tile-*.png",
        "scene-*.png",
        "path-*.png",
        "level-*.png",
        "player-*.png",
        "yoshit-*.png",
        "yoshib-*.png",

        "lunadll.txt",
        "lunaworld.txt",

        "lunadll.lua",
        "lunaworld.lua",
        "lunaoverworld.lua",
        "luna.lua",
        "map.lua"
    };

    QStringList files = d.entryList(filter, QDir::Files);

    for(const QString &file : files)
    {
        if(file.endsWith(".gif", Qt::CaseInsensitive))
            numGIFs++;
        else if(file.endsWith(".png", Qt::CaseInsensitive))
            numPNGs++;
        else if(file.endsWith(".mp3", Qt::CaseInsensitive) ||
                file.endsWith(".wav", Qt::CaseInsensitive) ||
                file.endsWith(".wma", Qt::CaseInsensitive))
            numMP3s++;
        else if(file.endsWith(".ogg", Qt::CaseInsensitive) ||
                 file.endsWith(".flac", Qt::CaseInsensitive) ||
                 file.endsWith(".spc", Qt::CaseInsensitive) ||
                 file.endsWith(".vgm", Qt::CaseInsensitive) ||
                 file.endsWith(".vgz", Qt::CaseInsensitive) ||
                 file.endsWith(".nsf", Qt::CaseInsensitive) ||
                 file.endsWith(".nsfe", Qt::CaseInsensitive) ||
                 file.endsWith(".hes", Qt::CaseInsensitive) ||
                 file.endsWith(".gbs", Qt::CaseInsensitive) ||
                 file.endsWith(".ay", Qt::CaseInsensitive) ||
                 file.endsWith(".mid", Qt::CaseInsensitive) ||
                 file.endsWith(".midi", Qt::CaseInsensitive) ||
                 file.endsWith(".rmi", Qt::CaseInsensitive) ||
                 file.endsWith(".xmi", Qt::CaseInsensitive) ||
                 file.endsWith(".imf", Qt::CaseInsensitive) ||
                 file.endsWith(".it", Qt::CaseInsensitive) ||
                 file.endsWith(".xm", Qt::CaseInsensitive) ||
                 file.endsWith(".mod", Qt::CaseInsensitive) ||
                 file.endsWith(".mod", Qt::CaseInsensitive))
            numNonMP3s++;
        else if(!file.compare("luna.lua", Qt::CaseInsensitive) ||
                !file.compare("map.lua", Qt::CaseInsensitive))
            numLunaLua++;
        else if(!file.compare("lunadll.lua", Qt::CaseInsensitive) ||
                !file.compare("lunaworld.lua", Qt::CaseInsensitive) ||
                !file.compare("lunaoverworld.lua", Qt::CaseInsensitive))
            numLunaDllLua++;
        else if(!file.compare("lunadll.txt", Qt::CaseInsensitive) ||
                !file.compare("lunaworld.txt", Qt::CaseInsensitive))
            numLunaDll++;
        else if(file.endsWith(".lvl", Qt::CaseInsensitive) || file.endsWith(".lvlx", Qt::CaseInsensitive))
        {
            LevelData head;
            FileFormats::OpenLevelFileHeader(d.absoluteFilePath(file), head);

            if(!head.meta.ReadFileValid)
                continue; // Skip invalid files

            switch(head.meta.RecentFormat)
            {
            case LevelData::SMBX64:
                numLVL64++;
                if(maxVerLVL64 < head.meta.RecentFormatVersion)
                    maxVerLVL64 = head.meta.RecentFormatVersion;
                break;

            case LevelData::SMBX38A:
                numLVL38A++;
                if(maxVerLVL38A < head.meta.RecentFormatVersion)
                    maxVerLVL38A = head.meta.RecentFormatVersion;
                break;

            case LevelData::PGEX:
                numLVLX++;
                if(head.meta.configPackId == thextechKey)
                    numTheXTech++;
                else if(head.meta.configPackId == smbx2Key)
                    numSMBX2++;
                break;
            }

        }
        else if(file.endsWith(".wld", Qt::CaseInsensitive) || file.endsWith(".wldx", Qt::CaseInsensitive))
        {
            WorldData head;
            FileFormats::OpenWorldFileHeader(d.absoluteFilePath(file), head);

            if(!head.meta.ReadFileValid)
                continue; // Skip invalid files

            switch(head.meta.RecentFormat)
            {
            case WorldData::SMBX64:
                numWLD64++;
                if(maxVerWLD64 < head.meta.RecentFormatVersion)
                    maxVerWLD64 = head.meta.RecentFormatVersion;
                break;

            case WorldData::SMBX38A:
                numWLD38A++;
                if(maxVerWLD38A < head.meta.RecentFormatVersion)
                    maxVerWLD38A = head.meta.RecentFormatVersion;
                break;

            case WorldData::PGEX:
                numWLDX++;
                if(head.meta.configPackId == thextechKey)
                    numTheXTech++;
                else if(head.meta.configPackId == smbx2Key)
                    numSMBX2++;
                break;
            }
        }
    }

    int diffLevelFormats = 0;
    int diffWorldFormats = 0;

    diffLevelFormats += (numLVL38A > 0);
    diffLevelFormats += (numLVL64 > 0);
    diffLevelFormats += (numLVLX > 0);

    diffWorldFormats += (numWLD38A > 0);
    diffWorldFormats += (numWLD64 > 0);
    diffWorldFormats += (numWLDX > 0);

    m_episodeStats.mixed = (diffWorldFormats > 1) || (diffLevelFormats > 1);
    m_episodeStats.all38A = numLVL38A > 0 && numWLD38A > 0 && !m_episodeStats.mixed;
    m_episodeStats.allPGEX = numLVLX > 0 && numWLDX > 0 && !m_episodeStats.mixed;
    m_episodeStats.allSMBX64 = numLVL64 > 0 && numWLD64 > 0 && !m_episodeStats.mixed;

    m_episodeStats.max38Aver = std::max(maxVerLVL38A, maxVerWLD38A);
    m_episodeStats.max64ver = std::max(maxVerLVL64, maxVerWLD64);


    QString guessFormat;


    if(numLVL38A == 0 && numLVL64 == 0 && numLVLX == 0 &&
       numWLD38A == 0 && numWLD64 == 0 && numWLDX == 0)
    {
        guessFormat = tr("<Not an episode nor a set of levels>");
    }
    else if(numLVL38A == 0 && numLVL64 == 0 && numLVLX == 0 &&
           (numWLD38A > 0 || numWLD64 > 0 || numWLDX > 0))
    {
        QString d_format;

        if(m_episodeStats.mixed)
            d_format = tr("<Many>");
        else if(numWLD38A > 0)
            d_format = "SMBX-38A";
        else if(numWLD64 > 0)
            d_format = "SMBX64 v" + QString::number(maxVerWLD64);
        else if(numWLDX > 0)
        {
            if(numTheXTech > 0)
                d_format = "TheXTech";
            else if(numSMBX2 > 0)
                d_format = "SMBX2 (X2)";
            else
                d_format = "Moondust";
        }

        guessFormat = tr("World map(s) without an episode, format: %1").arg(d_format);
    }
    else if((numLVL38A > 0 && numLVL64 > 0 && numLVLX > 0) &&
            numWLD38A == 0 && numWLD64 == 0 && numWLDX == 0)
    {
        QString d_format;

        if(m_episodeStats.mixed)
            d_format = tr("<Many>");
        else if(numLVL38A > 0)
            d_format = "SMBX-38A";
        else if(numLVL64 > 0)
            d_format = "SMBX64 v" + QString::number(maxVerLVL64);
        else if(numLVLX > 0)
        {
            if(numTheXTech > 0)
                d_format = "TheXTech";
            else if(numSMBX2 > 0)
                d_format = "SMBX2 (X2)";
            else
                d_format = "Moondust";
        }

        guessFormat = tr("Level(s) without an episode, format: %1").arg(d_format);
    }
    else if(numLVL38A == 0 && numWLD38A == 0 &&
        numLVL64 == 0 && numWLD64 == 0 &&
        numLunaDll == 0 && numLunaLua == 0 && numLunaDllLua == 0 &&
        numLVLX > 0 && numWLDX > 0 &&
        numSMBX2 == 0 && numTheXTech == 0)
    {
        guessFormat = "Moondust Engine";
    }
    else if(numLVL38A > 0 && numWLD38A > 0 &&
       numLVL64 == 0 && numWLD64 == 0 &&
       numGIFs == 0 &&
       numLunaDll == 0 && numLunaLua == 0 && numLunaDllLua == 0 &&
       numLVLX == 0 && numWLDX == 0 &&
       numSMBX2 == 0 && numTheXTech == 0)
    {
        guessFormat = "SMBX-38A";
    }
    else if(numLVL38A == 0 && numWLD38A == 0 &&
       numLVL64 >= 0 && numWLD64 == 0 &&
       numLunaDll >= 0 && numLunaLua == 0 && numLunaDllLua == 0 &&
       numLVLX > 0 && numWLDX > 0 &&
       numSMBX2 == 0 && numTheXTech > 0)
    {
        guessFormat = "TheXTech";
    }
    else if(numLVL38A == 0 && numWLD38A == 0 &&
            numLVL64 > 0 && numWLD64 > 0 && (numPNGs > 0 || numNonMP3s > 0) &&
            numLunaDll >= 0 && numLunaLua == 0 && numLunaDllLua >= 0 &&
            numLunaLua == 0 && numLunaDllLua >= 0 &&
            numLVLX == 0 && numWLDX == 0 &&
            numSMBX2 == 0 && numTheXTech == 0)
    {
        guessFormat = "SMBX2 Beta 3";
    }
    else if(numLVL38A == 0 && numWLD38A == 0 &&
       numLVL64 >= 0 && numWLD64 > 0 &&
       numLunaDll == 0 && numLunaLua > 0 && numLunaDllLua == 0 &&
       numLVLX > 0 && numWLDX == 0 &&
       numSMBX2 > 0 && numTheXTech == 0)
    {
        guessFormat = "SMBX2 Beta 3/4/5";
    }
    else if(numLVL38A == 0 && numWLD38A == 0 &&
       numLVL64 >= 0 && numWLD64 > 0 &&
       numLunaDll == 0 && numLunaLua >= 0 && numLunaDllLua >= 0 &&
       numLVLX > 0 && numWLDX == 0 &&
       numSMBX2 == 0 && numTheXTech == 0)
    {
        guessFormat = "SMBX2 Beta 3/4/5";
    }
    else if(numLVL38A == 0 && numWLD38A == 0 &&
       numLVL64 == 0 && numWLD64 > 0 &&
       numLVLX > 0 && numWLDX == 0 &&
       numLunaDll == 0 && numLunaLua >= 0 && numLunaDllLua >= 0 &&
       numSMBX2 == 0 && numTheXTech == 0)
    {
        guessFormat = "SMBX2 Beta 3/4/5";
    }
    else if(numLVL38A == 0 && numWLD38A == 0 &&
       numLVL64 == 0 && numWLD64 > 0 &&
       numLunaDll == 0 && numLunaLua >= 0 && numLunaDllLua >= 0 &&
       numLVLX > 0 && numWLDX == 0 &&
       numSMBX2 == 0 && numTheXTech == 0)
    {
        guessFormat = "SMBX2 Beta 3/4/5";
    }
    else if(numLVL38A == 0 && numWLD38A == 0 &&
            numLVL64 > 0 && numWLD64 > 0 && numPNGs > 0 &&
            numLunaDll == 0 && numLunaLua >= 0 && numLunaDllLua >= 0 &&
            numLVLX == 0 && numWLDX == 0 &&
            numSMBX2 == 0 && numTheXTech == 0)
    {
        guessFormat = "SMBX2 Beta 3/4/5";
    }
    else if(numLVL38A == 0 && numWLD38A == 0 &&
             numLVL64 > 0 && numWLD64 > 0 && numPNGs == 0 && numGIFs == 0 &&
            (maxVerLVL64 <= 1 || maxVerWLD64 <= 1) &&
             numLVLX == 0 && numWLDX == 0 &&
             numSMBX2 == 0 && numTheXTech == 0)
    {
        guessFormat = "SMBX 1.0";
    }
    else if(numLVL38A == 0 && numWLD38A == 0 &&
             numLVL64 > 0 && numWLD64 > 0 && numPNGs == 0 && numGIFs == 0 &&
             (maxVerLVL64 <= 2 || maxVerWLD64 <= 2) &&
             numLVLX == 0 && numWLDX == 0 &&
             numSMBX2 == 0 && numTheXTech == 0)
    {
        guessFormat = "SMBX 1.0.2";
    }
    else if(numLVL38A == 0 && numWLD38A == 0 &&
             numLVL64 > 0 && numWLD64 > 0 && numPNGs == 0 && numGIFs == 0 &&
             (maxVerLVL64 <= 18 || maxVerWLD64 <= 18) &&
             numLVLX == 0 && numWLDX == 0 &&
             numSMBX2 == 0 && numTheXTech == 0)
    {
        guessFormat = "SMBX 1.1.1";
    }
    else if(numLVL38A == 0 && numWLD38A == 0 &&
             numLVL64 > 0 && numWLD64 > 0 && numPNGs == 0 && numGIFs == 0 &&
             (maxVerLVL64 <= 20 || maxVerWLD64 <= 20) &&
             numLVLX == 0 && numWLDX == 0 &&
             numSMBX2 == 0 && numTheXTech == 0)
    {
        guessFormat = "SMBX 1.1.2";
    }
    else if(numLVL38A == 0 && numWLD38A == 0 &&
             numLVL64 > 0 && numWLD64 > 0 && numPNGs == 0 && numGIFs == 0 &&
             (maxVerLVL64 <= 28 || maxVerWLD64 <= 28) &&
             numLVLX == 0 && numWLDX == 0 &&
             numSMBX2 == 0 && numTheXTech == 0)
    {
        guessFormat = "SMBX 1.2.0 Beta 3";
    }
    else if(numLVL38A == 0 && numWLD38A == 0 &&
             numLVL64 > 0 && numWLD64 > 0 && numPNGs == 0 && numGIFs == 0 &&
             (maxVerLVL64 <= 32 || maxVerWLD64 <= 32) &&
             numLVLX == 0 && numWLDX == 0 &&
             numSMBX2 == 0 && numTheXTech == 0)
    {
        guessFormat = "SMBX 1.2.0 Beta 4";
    }
    else if(numLVL38A == 0 && numWLD38A == 0 &&
             numLVL64 > 0 && numWLD64 > 0 && numPNGs == 0 && numGIFs == 0 &&
             (maxVerLVL64 <= 50 || maxVerWLD64 <= 50) &&
             numLVLX == 0 && numWLDX == 0 &&
             numSMBX2 == 0 && numTheXTech == 0)
    {
        guessFormat = "SMBX 1.2.0";
    }
    else if(numLVL38A == 0 && numWLD38A == 0 &&
             numLVL64 > 0 && numWLD64 > 0 && numPNGs == 0 &&
            (maxVerLVL64 <= 58 || maxVerWLD64 <= 58) &&
             numLVLX == 0 && numWLDX == 0 &&
             numSMBX2 == 0 && numTheXTech == 0)
    {
        guessFormat = "SMBX 1.2.2";
    }
    else if(numLVL38A == 0 && numWLD38A == 0 &&
             numLVL64 > 0 && numWLD64 > 0 && numPNGs == 0 &&
             maxVerLVL64 <= 59 && maxVerWLD64 <= 59 &&
             numLVLX == 0 && numWLDX == 0 &&
             numSMBX2 == 0 && numTheXTech == 0)
    {
        guessFormat = "SMBX 1.2.2 Beta 59";
    }
    else if(numLVL38A == 0 && numWLD38A == 0 &&
            numLVL64 > 0 && numWLD64 > 0 && numPNGs == 0 &&
            maxVerLVL64 <= 64 && maxVerWLD64 <= 64 &&
            numLVLX == 0 && numWLDX == 0 &&
            numSMBX2 == 0 && numTheXTech == 0)
    {
        guessFormat = "SMBX 1.3";
    }
    else
        guessFormat = tr("Unknown episode",
                         "Heuristicly detected episode format");


    ui->detectedFormatLabel->setText(tr("Guessed episode format: %1").arg(guessFormat));

    qDebug()
        << "numWLDX=" << numWLDX << "\n"
        << "numLVLX=" << numLVLX << "\n"
        << "\n"
        << "numLVL64=" << numLVL64 << "\n"
        << "numWLD64=" << numWLD64 << "\n"
        << "maxVerLVL64=" << maxVerLVL64 << "\n"
        << "maxVerWLD64=" << maxVerWLD64 << "\n"
        << "\n"
        << "numLVL38A=" << numLVL38A << "\n"
        << "numWLD38A=" << numWLD38A << "\n"
        << "maxVerLVL38A=" << maxVerLVL38A << "\n"
        << "maxVerWLD38A=" << maxVerWLD38A << "\n"
        << "\n"
        << "SMBX2 tags=" << numSMBX2 << "\n"
        << "TheXTech tags=" << numTheXTech << "\n"
        << "\n"
        << "numGIFs=" << numGIFs << "\n"
        << "numPNGs=" << numPNGs << "\n"
        << "\n"
        << "numLunaDll=" << numLunaDll << "\n"
        << "numLunaDllLua=" << numLunaDllLua << "\n"
        << "numLunaLua=" << numLunaLua << "\n"
        << "\n"
        << "numMP3s=" << numMP3s << "\n"
        << "numNonMP3s=" << numNonMP3s << "\n"
    ;

#endif // DEBUG_BUILD

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

    int lvlDstFormat = ui->targetFormat->currentIndex();
    int wldDstFormat = ui->targetFormatWld->currentIndex() > 0 ? ui->targetFormatWld->currentIndex() - 1 : lvlDstFormat;

    int lvlDstVer = ui->targetFormatWldVer->value();
    int wldDstVer = ui->targetFormatWld->currentIndex() > 0 ? ui->targetFormatWldVer->value() : lvlDstVer;

    // QEventLoop loop;

    EpisodeConverterWorker::JobSetup jSetup =
    {
        ui->episodePath->text(), ui->lookForSubDirs->isChecked(),
        lvlDstFormat, lvlDstVer,
        wldDstFormat, wldDstVer,
        ui->targetCPID->text(),
        ui->respectPre13Options->isChecked()
    };

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    m_process = QtConcurrent::run<bool>(&m_worker, &EpisodeConverterWorker::initJob, jSetup);
#else
    m_process = QtConcurrent::run(&EpisodeConverterWorker::initJob, &m_worker, jSetup);
#endif

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

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    m_process = QtConcurrent::run<bool>(&m_worker, &EpisodeConverterWorker::runJob);
#else
    m_process = QtConcurrent::run(&EpisodeConverterWorker::runJob, &m_worker);
#endif

    m_progressWatcher.start(100);
}

void EpisodeConverter::on_browse_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open target episode path"),
                  (ui->episodePath->text().isEmpty() ? ApplicationPath : ui->episodePath->text()),
                  c_dirDialogOptions);
    if(dir.isEmpty())
        return;

    ui->episodePath->setText(dir);
    detectEpisodeFormat();
}

void EpisodeConverter::workFinished(bool isFine)
{
    m_process.waitForFinished();
    m_progressWatcher.stop();
    ui->progressBar->reset();
    emit setLocked(false);

    detectEpisodeFormat();

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
