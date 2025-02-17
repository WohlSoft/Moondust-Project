/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2025 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "musicfilelist.h"
#include "../custom_music_setup/custom_music_setup.h"

#include <QIcon>
#include <QPushButton>
#include <QSpacerItem>
#include <QLineEdit>
#include <QGridLayout>


MusicFileList::MusicFileList(QString searchDirectory, QString curFile, QWidget *parent, bool sfxMode) :
    FileListBrowser(searchDirectory, curFile, parent),
    m_sfxMode(sfxMode)
{
    QStringList filters;

    setIcon(QIcon(":/moondust/playmusic.png"));

    if(m_sfxMode)
    {
        setWindowTitle(tr("Select SFX file"));
        setDescription(tr("Please select SFX file to use"));
        filters // MPEG 1 Layer III (LibMAD)
                                   << "*.mp3"
                                   // OGG Vorbis, FLAC, amd Opus (LibOGG, LibVorbis, LibFLAC, libOpus)
                                   << "*.ogg" << "*.flac" << "*.opus"
                                   // Uncompressed audio data
                                   << "*.wav" << "*.voc" << "*.aiff"
                                   // Some chiptunes can be used as SFX
                                   << "*.spc";
        setFilters(filters);
    }
    else
    {
        setWindowTitle(tr("Select Custom music"));
        setDescription(tr("Please select music file to use as custom"));
        filters // MPEG 1 Layer III (LibMAD)
                << "*.mp3"
                // OGG Vorbis, FLAC, amd Opus (LibOGG, LibVorbis, LibFLAC, libOpus)
                << "*.ogg" << "*.flac" << "*.opus"
                // Uncompressed audio data
                << "*.wav" << "*.voc" << "*.aiff"
                // MIDI
                << "*.mid" << "*.midi" << "*.rmi" << "*.mus" << "*.kar" << "*.xmi" << "*.cmf"
                // Id Music File (OPL2 raw) / Imago Orpheus (Tracker music)
                << "*.imf" << "*.wlf"
                // Modules (Tracker music)
                << "*.mod" << "*.it" << "*.s3m" << "*.669" << "*.med" << "*.xm" << "*.amf"
                << "*.apun" << "*.dsm" << "*.far" << "*.gdm" << "*.mtm"
                << "*.okt" << "*.stm" << "*.stx" << "*.ult" << "*.uni" << "*.mptm"
                // GAME EMU (Chiptunes)
                << "*.ay" << "*.gbs" << "*.gym" << "*.hes" << "*.kss" << "*.nsf"
                << "*.nsfe" << "*.sap" << "*.spc" << "*.vgm" << "*.vgz"
                // PXTONE
                << "*.pttune" << "*.ptcop";
        setFilters(filters);
    }

    QGridLayout *previewLayout = new QGridLayout();
    m_previewButton = new QPushButton();

    if(m_sfxMode)
    {
        m_previewButton->setText(tr("Test SFX", "Preview selected sound file"));
        QObject::connect(m_previewButton, static_cast<void (QPushButton::*)(bool)>(&QPushButton::clicked),
                         this, [this](bool)->void
        {
            QString file = currentSelectedFile();
            if(file.isEmpty())
                return;

            emit playSoundFile(directoryPath() + file);
        });
    }
    else
    {
        int args = curFile.indexOf('|');
        if(args >= 0)
            m_musicArguments = curFile.mid(args + 1);

        m_previewButton->setText(tr("Music play/stop", "Preview selected music file"));
        m_previewButton->setCheckable(true);

        bool hasExtraSettings = CustomMusicSetup::settingsNeeded(curFile);

        m_musicSetupButton = new QPushButton();
        m_musicSetupButton->setText(tr("Settings..."));
        previewLayout->addWidget(m_musicSetupButton, 0, 2);
        m_musicSetupButton->setVisible(hasExtraSettings);

        const QString argsNotSpecified = tr("Extra settings are not specified");
        const QString argsNotSupported = tr("Extra settings are not supported for this music file format");

        m_musicArgumentsPreview = new QLineEdit();
        m_musicArgumentsPreview->setPlaceholderText(hasExtraSettings ? argsNotSpecified : argsNotSupported);
        previewLayout->addWidget(m_musicArgumentsPreview, 1, 0, 1, 3);
        m_musicArgumentsPreview->setText(m_musicArguments);
        m_musicArgumentsPreview->setToolTip(tr("Music settings arguments:\n"
                                               "This is a special string that allows to set an extra settings for this music file to alternate its playing behaviour and/or sounding.\n"
                                               "These settings are different depending on the file format.\n"
                                               "\n"
                                               "You can't set these settings if given file format doesn't support them."));
        m_musicArgumentsPreview->setEnabled(hasExtraSettings);

        QObject::connect(m_previewButton, static_cast<void (QPushButton::*)(bool)>(&QPushButton::clicked),
                         this, &MusicFileList::musicButtonClicked);

        QObject::connect(this, &FileListBrowser::itemSelected,
                         this, [this, argsNotSpecified, argsNotSupported](const QString &item)->void
        {
            bool setupNeeded = CustomMusicSetup::settingsNeeded(item);
            emit musicFileChanged(m_musicArguments.isEmpty() || !setupNeeded ?
                                      item :
                                      item + "|" + m_musicArguments);
            emit updateSongPlay();
            m_musicSetupButton->setVisible(setupNeeded);
            m_musicArgumentsPreview->setEnabled(setupNeeded);
            m_musicArgumentsPreview->setPlaceholderText(setupNeeded ? argsNotSpecified : argsNotSupported);
            if(!setupNeeded)
            {
                m_musicArguments.clear();
                m_musicArgumentsPreview->setText(m_musicArguments);
            }
        });

        QObject::connect(m_musicArgumentsPreview, &QLineEdit::editingFinished, this, [this]()->void
        {
            QString musicPath = currentSelectedFile();
            m_musicArguments = m_musicArgumentsPreview->text();
            emit musicFileChanged(musicPath + "|" + m_musicArguments);
            emit updateSongPlay();
        });

        QObject::connect(m_musicSetupButton, static_cast<void (QPushButton::*)(bool)>(&QPushButton::clicked),
                         this, [this](bool)->void
        {
            CustomMusicSetup set(this);
            set.initLists();

            QString musicPath = currentSelectedFile();
            set.setMusicPath(musicPath + "|" + m_musicArguments);
            set.setDirectory(directoryPath());
            set.setMusicPlayState(m_previewButton->isChecked());

            QObject::connect(&set, &CustomMusicSetup::musicSetupChanged, this, [this, &musicPath](const QString &music)->void
            {
                musicPath = music;
                emit musicFileChanged(musicPath);
            });

            QObject::connect(&set, &CustomMusicSetup::updateSongPlay, this, [this]()->void
            {
                emit updateSongPlay();
            });

            QObject::connect(&set, &CustomMusicSetup::updateSongTempo, this, [this](double tempo)->void
            {
                emit musicTempoChanged(tempo);
            });

            QObject::connect(&set, &CustomMusicSetup::updateSongGain, this, [this](double gain)->void
            {
                emit musicGainChanged(gain);
            });

            QObject::connect(&set, &CustomMusicSetup::musicButtonClicked, this, [this](bool st)->void
            {
                m_previewButton->setChecked(st);
                emit m_previewButton->clicked(st);
            });

            set.show();
            set.adjustSize();
            int ret = set.exec();

            if(ret == QDialog::Accepted)
            {
                int args = musicPath.indexOf('|');
                if(args >= 0)
                {
                    m_musicArguments = musicPath.mid(args + 1);
                    m_musicArgumentsPreview->setText(m_musicArguments);
                }
                emit musicFileChanged(musicPath);
            }
        });
    }

    previewLayout->addWidget(m_previewButton, 0, 0);
    previewLayout->setContentsMargins(0, 0, 0, 0);
    previewLayout->setColumnStretch(0, 0);
    previewLayout->setColumnStretch(1, 10000);

    previewLayout->addItem(new QSpacerItem(200, 20, QSizePolicy::Minimum, QSizePolicy::Minimum), 0, 1);

    setExtraWidgetLayout(previewLayout);

    startListBuilder();
}

MusicFileList::~MusicFileList()
{}

void MusicFileList::setMusicPlayState(bool checked)
{
    if(m_previewButton)
        m_previewButton->setChecked(checked);
}

bool MusicFileList::hasMusicArguments() const
{
    return !m_musicArguments.isEmpty();
}

QString MusicFileList::musicArguments() const
{
    return m_musicArguments;
}

QString MusicFileList::currentFile()
{
    if(hasMusicArguments())
        return FileListBrowser::currentFile() + "|" + musicArguments();
    else
        return FileListBrowser::currentFile();
}

