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

#pragma once
#ifndef MUSICFILELIST_H
#define MUSICFILELIST_H

#include "file_list_browser.h"

class MusicFileList : public FileListBrowser
{
    Q_OBJECT

public:
    explicit MusicFileList(QString searchDirectory, QString curFile = QString(), QWidget *parent = nullptr, bool sfxMode = false);
    ~MusicFileList();

    void setMusicPlayState(bool checked);

    bool hasMusicArguments() const;
    QString musicArguments() const;

    QString currentFile() override;

signals:
    void musicFileChanged(const QString &music);
    void updateSongPlay();
    void musicButtonClicked(bool checked);
    void musicTempoChanged(double tempo);
    void musicGainChanged(double gain);

    void playSoundFile(const QString &sfx);

private:
    bool m_sfxMode = false;
    QString m_musicArguments;
    QPushButton *m_previewButton = nullptr;
    QPushButton *m_musicSetupButton = nullptr;
    QLineEdit *m_musicArgumentsPreview = nullptr;
};

#endif // MUSICFILELIST_H
