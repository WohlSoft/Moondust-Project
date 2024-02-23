/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2024 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <QFontDatabase>
#include <QFontMetrics>
#include <QScrollBar>
#include <QStyle>
#include <QSettings>

#include "itemmsgbox.h"
#include <ui_itemmsgbox.h>

#include "main_window/global_settings.h"


ItemMsgBox::ItemMsgBox(Opened_By openedBy, QString text, bool isFriendly, QString label, QString title, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ItemMsgBox)
{
    QFontDatabase::addApplicationFont(":/fonts/PressStart2P.ttf");

    whoOpened = openedBy;
    currentText = text;
    ui->setupUi(this);

    ui->checkFriendly->setChecked(isFriendly);

    if(whoOpened != NPC)
        ui->checkFriendly->hide();

    if(!label.isEmpty())
        ui->NotesLabel->setText(label);

    if(!title.isEmpty())
        this->setWindowTitle(title);

    // Set default preview font to 8
    ui->previewBox->setFontSize(8);

    ui->previewFont->setValue(ui->previewBox->fontSize());

    QObject::connect(ui->previewShow, static_cast<void(QToolButton::*)(bool)>(&QToolButton::toggled),
                     this, &ItemMsgBox::updateDialogueSize);

    qApp->processEvents(QEventLoop::ExcludeUserInputEvents|QEventLoop::ExcludeSocketNotifiers);
    ui->previewArea->setVisible(ui->previewShow->isChecked());
    ItemMsgBox::updateDialogueSize(ui->previewShow->isChecked());

    QObject::connect(ui->msgTextBox, &QPlainTextEdit::textChanged, this,
    [this]()->void
    {
        ui->previewBox->setText(ui->msgTextBox->toPlainText());
    });

    QObject::connect(ui->previewFont, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
    [this](int val)->void
    {
        ui->previewBox->setFontSize(val);
    });

    QObject::connect(ui->previewEngine, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
    [this](int val)->void
    {
        ui->previewBox->setVanillaMode(val == 0);
        ui->previewMacrosGroup->setVisible(val > 0);
        ui->previewBox->setEnableMacros(val > 0 && ui->previewMacrosGroup->isChecked());
        updateEngineDesc();
    });

    updateEngineDesc();

    QObject::connect(ui->previewMacrosGroup, static_cast<void(QGroupBox::*)(bool)>(&QGroupBox::toggled), this,
    [this](bool macros)->void
    {
        ui->previewBox->setEnableMacros(macros && ui->previewEngine->currentIndex() > 0);
        ui->previewMacrosGroup->setVisible(ui->previewEngine->currentIndex() > 0);
    });

    QObject::connect(ui->previewMacroPlayer, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
    [this](int val)->void
    {
        ui->previewBox->setMacroPlayerNum(val);
    });

    QObject::connect(ui->previewMacroState, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
    [this](int val)->void
    {
        ui->previewBox->setMacroPlayerState(val);
    });

    ui->previewMacrosGroup->setChecked(false);
    ui->previewMacrosGroup->setVisible(ui->previewEngine->currentIndex() > 0);

#if 1 // Hide experimental feature
    ui->previewMacroState->setVisible(false);
    ui->previewMacroStateLabel->setVisible(false);
#endif

    ui->msgTextBox->setPlainText(currentText);
    updateGeometry();

    loadSetup();
}

ItemMsgBox::~ItemMsgBox()
{
    saveSetup();
    delete ui;
}

bool ItemMsgBox::isFriendlyChecked()
{
    return ui->checkFriendly->isChecked();
}

void ItemMsgBox::on_buttonBox_accepted()
{
    currentText = ui->msgTextBox->toPlainText();
    accept();
}

void ItemMsgBox::updateDialogueSize(bool showPreview)
{
    QRect self = this->geometry();
    QRect selfL = ui->dialogueLayout->geometry();

    if(showPreview)
    {
        ui->previewArea->setVisible(showPreview);
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents|QEventLoop::ExcludeSocketNotifiers);
        QSize area = ui->previewArea->size();
        self.setWidth(self.width() + area.width());
        selfL.setWidth(selfL.width() + area.width());
        ui->dialogueLayout->setGeometry(selfL);
        this->setGeometry(self);
    }
    else
    {
        QSize area = ui->previewArea->size();
        ui->previewArea->setVisible(showPreview);
        this->update();
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents|QEventLoop::ExcludeSocketNotifiers);
        self.setWidth(self.width() - area.width());
        selfL.setWidth(selfL.width() - area.width());
        ui->dialogueLayout->setGeometry(selfL);
        this->setGeometry(self);
    }
}

void ItemMsgBox::updateEngineDesc()
{
    switch(ui->previewEngine->currentIndex())
    {
    case 0: // SMBX 1.3 / X2
        ui->engineDesc->setText(
            tr("The basic message box of <b>SMBX 1.3 / X2</b> has the next restrictions:\n"
               "<ul>\n"
               "  <li>New line <b>CAN NOT be done via the \"Return\"/\"Enter\" key</b><br/>\n"
               "(you should use spaces as a workaround).<li>\n"
               "  <li>Line breaks happen automatically after <b>27 characters</b>.</li>\n"
               "  <li>Very long words doesn't breaks and gets printed out of screen.<br/>\n"
               "So, please break these words to avoid this.</li>\n"
               "  <li>You can't use any Unicode characters: <b>ASCII only</b> works.</li>\n"
               "</ul>", "Message box behaviour explanation")
        );
        break;

    case 1: // Moondust / TheXTech
        ui->engineDesc->setText(
            tr("The message box of <b>Moondust / TheXTech</b> has the next restrictions:\n"
               "<ul>\n"
               "  <li>New line <b>CAN be done by \"Return\"/\"Enter\" key</b> easily.</li>\n"
               "  <li>Line breaks also happen automatically after <b>27 characters</b>.</li>\n"
               "  <li>Very long words gets broken forcibly.</li>\n"
               "  <li>You can use <b>any Unicode</b> characters.</li>\n"
               "  <li><b>Preprocessor's macros</b> can be used.</li>\n"
               "</ul>", "Message box behaviour explanation")
            );
        break;
    }
}

void ItemMsgBox::loadSetup()
{
    QSettings settings(ConfStatus::configLocalSettingsFile, QSettings::IniFormat);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    settings.setIniCodec("UTF-8");
#endif

    settings.beginGroup("MsgBoxEditor");
    {
        if(settings.contains("show-preview"))
            ui->previewShow->setChecked(settings.value("show-preview", false).toBool());

        ui->previewEngine->setCurrentIndex(settings.value("engine", 0).toInt());
        if(settings.contains("font-size"))
            ui->previewFont->setValue(settings.value("font-size").toInt());

        ui->previewMacrosGroup->setChecked(settings.value("macros", false).toBool());
        ui->previewMacroPlayer->setValue(settings.value("macros-player", 0).toInt());

        restoreGeometry(settings.value("geometry").toByteArray());
    }
    settings.endGroup();
}

void ItemMsgBox::saveSetup()
{
    QSettings settings(ConfStatus::configLocalSettingsFile, QSettings::IniFormat);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    settings.setIniCodec("UTF-8");
#endif

    settings.beginGroup("MsgBoxEditor");
    {
        settings.setValue("show-preview", ui->previewShow->isChecked());
        settings.setValue("engine", ui->previewEngine->currentIndex());
        settings.setValue("font-size", ui->previewFont->value());
        settings.setValue("macros", ui->previewMacrosGroup->isChecked());
        settings.setValue("macros-player", ui->previewMacroPlayer->value());

        settings.setValue("geometry", saveGeometry());
    }
    settings.endGroup();
}
