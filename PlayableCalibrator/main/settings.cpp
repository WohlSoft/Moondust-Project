/*
 * Playble Character Calibrator, a free tool for playable character sprite setup tune
 * This is a part of the Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2017-2026 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <QSettings>
#include <QFile>
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
#include <QFileInfo>

#include "calibration_main.h"
#include "ui_calibration_main.h"
#include <pge_app_path.h>

#include "qfile_dialogs_default_options.hpp"


void CalibrationMain::createDirs()
{
    if(!AppPathManager::isPortable())
        return;

    QDir sprite(AppPathManager::dataDir() + "/calibrator/spriteconf");

    if(!sprite.exists())
        sprite.mkpath(".");
}

void CalibrationMain::loadAppSettings()
{
    QString inifile = AppPathManager::settingsFile();
    QSettings settings(inifile, QSettings::IniFormat);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    settings.setIniCodec("UTF-8");
#endif

    settings.beginGroup("Main");
    {
        m_lastOpenDir = settings.value("lastpath", ".").toString();
        m_currentFile = settings.value("lastfile", m_currentFile).toString();
        m_externalGfxEditorPath = settings.value("ext-gfx-editor-path").toString();
        m_frmX = settings.value("last-frame-x", ui->FrameX->value()).toInt();
        m_frmY = settings.value("last-frame-y", ui->FrameY->value()).toInt();
    }
    settings.endGroup();
}

void CalibrationMain::saveAppSettings()
{
    QString config = AppPathManager::settingsFile();
    QSettings opts(config, QSettings::IniFormat);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    opts.setIniCodec("UTF-8");
#endif

    opts.beginGroup("Main");
    {
        opts.setValue("lastpath", m_lastOpenDir);
        opts.setValue("lastfile", m_currentFile);
        opts.setValue("ext-gfx-editor-path", m_externalGfxEditorPath);
        opts.setValue("last-frame-x", m_frmX);
        opts.setValue("last-frame-y", m_frmY);
    }
    opts.endGroup();
}

QString CalibrationMain::getGfxEditorPath(QWidget *parent)
{
    bool choose = false;

    if(m_externalGfxEditorPath.isEmpty() || !QFile::exists(m_externalGfxEditorPath))
    {
        int reply = QMessageBox::question(parent ? parent : this,
                                          tr("Graphical editor is not specified"),
                                          tr("External graphics editor application is not specified. "
                                             "Do you want to select it now?"),
                                          QMessageBox::Yes|QMessageBox::No);

        if(reply != QMessageBox::Yes)
            return QString();

        choose = true;
    }

    if(choose)
    {
        if(!changeGfxEditor(parent))
            return QString();
    }

    return m_externalGfxEditorPath;
}

bool CalibrationMain::changeGfxEditor(QWidget *parent)
{
    QString p = QFileDialog::getOpenFileName(parent ? parent : this,
                                             tr("Select a path to the graphical editor executable",
                                                "File dialog title"),
                                             m_externalGfxEditorPath,
                                             QString(),
                                             nullptr,
                                             c_fileDialogOptions);
    if(p.isEmpty())
        return false;

    m_externalGfxEditorPath = p;

#ifdef __APPLE__
    // Automatically recognize an executable inside of an app bundle
    QFileInfo x(m_externalGfxEditorPath);
    if(!m_externalGfxEditorPath.isEmpty() && x.isDir() && m_externalGfxEditorPath.endsWith(".app", Qt::CaseInsensitive))
    {
        QDir bundleMac(m_externalGfxEditorPath + "/Contents/MacOS");
        if(bundleMac.exists())
        {
            auto f = bundleMac.entryList(QDir::Files);
            if(!f.isEmpty())
            {
                m_externalGfxEditorPath = bundleMac.absoluteFilePath(f.first());
            }
        }
    }
#endif

    return true;
}
