/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2021 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <PGE_File_Formats/lvl_filedata.h>
#include <common_features/json_settings_widget.h>

#include <mainwindow.h>

#include "levelprops.h"
#include <ui_levelprops.h>


LevelProps::LevelProps(LevelData &FileData, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LevelProps)
{
    if(parent)
    {
        if(std::strcmp(parent->metaObject()->className(), "MainWindow") == 0)
            m_mw = qobject_cast<MainWindow *>(parent);
        else
            m_mw = nullptr;
    }

    m_currentData = &FileData;
    ui->setupUi(this);
    ui->LVLPropLevelTitle->setText(m_currentData->LevelName);
    m_extraSettingsSpacer.reset(new QSpacerItem(100, 999999, QSizePolicy::Minimum, QSizePolicy::Expanding));
    initAdvancedSettings();
}

LevelProps::~LevelProps()
{
    if(m_extraSettings.get())
        ui->advancedSettings->layout()->removeWidget(m_extraSettings.get()->getWidget());
    ui->advancedSettings->layout()->removeItem(m_extraSettingsSpacer.get());
    m_extraSettings.reset();
    m_extraSettingsSpacer.reset();
    delete ui;
}

void LevelProps::on_LVLPropButtonBox_accepted()
{
    m_levelTitle = ui->LVLPropLevelTitle->text().trimmed().simplified().remove('\"');
    accept();
}

void LevelProps::on_LVLPropButtonBox_rejected()
{
    reject();
}

void LevelProps::initAdvancedSettings()
{
    QString defaultDir = m_mw->configs.config_dir;
    CustomDirManager uLVL(m_currentData->meta.path, m_currentData->meta.filename);
    uLVL.setDefaultDir(defaultDir);

    QString esLayoutFile = uLVL.getCustomFile("lvl_settings.json");
    if(esLayoutFile.isEmpty())
        return;

    QFile layoutFile(esLayoutFile);
    if(!layoutFile.open(QIODevice::ReadOnly))
        return;

    QByteArray rawLayout = layoutFile.readAll();

    ui->advancedSettings->setEnabled(!m_currentData->meta.smbx64strict);

    m_extraSettings.reset(new JsonSettingsWidget(ui->advancedSettings));
    if(m_extraSettings)
    {
        m_extraSettings->setSearchDirectories(m_currentData->meta.path, m_currentData->meta.filename);
        m_extraSettings->setConfigPack(&m_mw->configs);
        if(!m_extraSettings->loadLayout(m_currentData->custom_params.toUtf8(), rawLayout))
        {
            LogWarning(m_extraSettings->errorString());
            ui->advancedNone->setText(tr("Error in the file %1:\n%2")
                                      .arg(esLayoutFile)
                                      .arg(m_extraSettings->errorString()));
            ui->advancedNone->setStyleSheet("*{background-color: #FF0000;}");
        }
        auto *widget = m_extraSettings->getWidget();
        if(widget)
        {
            ui->advancedSettings->layout()->addWidget(widget);
            if(m_extraSettings->spacerNeeded())
                ui->advancedSettings->layout()->addItem(m_extraSettingsSpacer.get());
            JsonSettingsWidget::connect(m_extraSettings.get(), &JsonSettingsWidget::settingsChanged, this, &LevelProps::onExtraSettingsChanged);
            ui->advancedNone->setVisible(false);
        }
    }
    layoutFile.close();
}

void LevelProps::onExtraSettingsChanged()
{
    m_customParams = m_extraSettings->saveSettings();
    LogDebug("Extra Settings change: " + m_customParams);
}
