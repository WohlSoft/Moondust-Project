/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
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
#include <QSpinBox>
#include <QLabel>
#include <QPushButton>

#include <script/commands/memorycommand.h>

#include "additionalsettings.h"
#include <ui_additionalsettings.h>

AdditionalSettings::AdditionalSettings(const QString &name, ScriptHolder *script, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AdditionalSettings)
{
    ui->setupUi(this);
    m_scriptHolder = script;


    QList<AdditionalSettings::SimpleAdditionalSetting> confSetting = loadSimpleAdditionalSettings(name);
    loadValuesByScriptHolder(confSetting);
    configGUI(confSetting);
    selectFirstValidItem();
    updateSelection();
}

AdditionalSettings::~AdditionalSettings()
{
    delete ui;
}

QList<AdditionalSettings::SimpleAdditionalSetting> AdditionalSettings::loadSimpleAdditionalSettings(const QString &path)
{
    QList<AdditionalSettings::SimpleAdditionalSetting> collectedSettings;

    QSettings memSetting(path, QSettings::IniFormat);
    memSetting.setIniCodec("UTF-8");
    QStringList categories = memSetting.childGroups();
    foreach(QString category, categories){
        memSetting.beginGroup(category);

        QStringList groupList = memSetting.childGroups();
        foreach(QString group, groupList){
            memSetting.beginGroup(group);

            QStringList objList = memSetting.childGroups();
            foreach(QString obj, objList){
                QVariant var_memaddr;
                QVariant var_controlType;
                QVariant var_numRangeBegin;
                QVariant var_numRangeEnd;
                QVariant var_defaultVal;
                QVariant var_txtLabel;
                QVariant var_memAddrType;

                memSetting.beginGroup(obj);

                var_memaddr = memSetting.value("mem-address");
                var_controlType = memSetting.value("control-type");
                var_numRangeBegin = memSetting.value("range-begin", 0);
                var_numRangeEnd = memSetting.value("range-end", 20);
                var_defaultVal = memSetting.value("default-value");
                var_txtLabel = memSetting.value("txt-label");
                var_memAddrType = memSetting.value("mem-address-type", static_cast<int>(MemoryCommand::FIELD_WORD));

                if(var_memaddr.isNull() ||
                        var_controlType.isNull() ||
                        var_defaultVal.isNull() ||
                        var_txtLabel.isNull()){
                    memSetting.endGroup();
                    continue;
                }


                SimpleAdditionalSetting curSetting;
                curSetting.category = category;
                curSetting.group = group;
                curSetting.defaultValue = var_defaultVal;
                curSetting.controlType = var_controlType.toInt();
                curSetting.labelTxt = var_txtLabel.toString();
                curSetting.memAddr = (int)var_memaddr.toString().toUInt(0, 16);
                curSetting.memAddrType = var_memAddrType.toInt();

                curSetting.beginRange = var_numRangeBegin.toInt();
                curSetting.endRange = var_numRangeEnd.toInt();
                curSetting.value = curSetting.defaultValue;


                collectedSettings << curSetting;

                memSetting.endGroup();
            }


            memSetting.endGroup();
        }


        memSetting.endGroup();
    }

    return collectedSettings;
}

void AdditionalSettings::configGUI(QList<AdditionalSettings::SimpleAdditionalSetting> &data)
{
    for(int i = 0; i < data.size(); ++i){
        AdditionalSettings::SimpleAdditionalSetting& selData = data[i];

        QTreeWidgetItem *selCatorgyItem = 0;
        for(int j = 0; j < ui->SettingList->topLevelItemCount(); ++j){
            QTreeWidgetItem *searchItem = ui->SettingList->topLevelItem(j);
            if(searchItem->text(0) == selData.category){
                selCatorgyItem = searchItem;
                break;
            }
        }

        if(!selCatorgyItem){
            selCatorgyItem = new QTreeWidgetItem(ui->SettingList);
        }

        selCatorgyItem->setText(0, selData.category);


        QTreeWidgetItem *selGroupItem = 0;
        for(int j = 0; j < selCatorgyItem->childCount(); ++j){
            QTreeWidgetItem *searchItem = selCatorgyItem->child(j);
            if(searchItem->text(0) == selData.group){
                selGroupItem = searchItem;
                break;
            }
        }

        if(!selGroupItem){
            selGroupItem = new QTreeWidgetItem(selCatorgyItem);
        }

        selGroupItem->setText(0, selData.group);


        QWidget *selWidget = 0;
        QGridLayout *selLayout = 0;
        if(!m_page.contains(selGroupItem)){
            selWidget = new QWidget();
            selWidget->setLayout(selLayout = new QGridLayout(selWidget));
            selLayout->setContentsMargins(3,3,3,3);
            selLayout->setHorizontalSpacing(2);
            selLayout->setVerticalSpacing(2);
            m_page[selGroupItem] = selWidget;
        }else{
            selWidget = m_page[selGroupItem];
        }

        selLayout = qobject_cast<QGridLayout*>(selWidget->layout());

        int selNextRow = selLayout->count();


        QWidget *targetWidget;
        QLabel *targetLabel = new QLabel(selData.labelTxt, selWidget);
        if(selData.controlType == 0){
            QSpinBox *spinBox = new QSpinBox(selWidget);
            spinBox->setMinimum(selData.beginRange);
            spinBox->setMaximum(selData.endRange);
            spinBox->setValue(selData.value.toInt());
            connect(spinBox, SIGNAL(valueChanged(int)), this, SLOT(spinValChanged()));
            targetWidget = (QWidget*)spinBox;
        }
        QPushButton *targetReset = new QPushButton("Reset Value",selWidget);

        selData.dataControl = targetWidget;
        selData.resetControl = targetReset;
        m_data[targetWidget] = selData;

        selLayout->addWidget(targetLabel, selNextRow, 0);
        selLayout->addWidget(targetWidget, selNextRow, 1);
        selLayout->addWidget(targetReset, selNextRow, 2);

        connect(targetReset, SIGNAL(clicked()), this, SLOT(resetValue()));
    }


    //Append a Space on the bottom
    for(QMap<QTreeWidgetItem*, QWidget*>::iterator it = m_page.begin(); it != m_page.end(); it++){
        QGridLayout* targetLayout = (QGridLayout*)((QWidget*)(*it))->layout();
        QSpacerItem* horizontalSpacer = new QSpacerItem(1, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
        targetLayout->addItem(horizontalSpacer, targetLayout->rowCount(), 0, 1, -1);
    }
}

void AdditionalSettings::selectFirstValidItem()
{
    for(int i = 0; i < ui->SettingList->topLevelItemCount(); ++i){
        QTreeWidgetItem* baseItem = ui->SettingList->topLevelItem(i);
        for(int j = 0; j < baseItem->childCount(); ++j){
            baseItem->child(j)->setSelected(true);
            baseItem->setExpanded(true);
            return;
        }
    }

}

void AdditionalSettings::updateSelection()
{
    if(ui->SettingList->selectedItems().isEmpty()) return;

    if(ui->contentLayout->count()){
        for(int i = 0; ui->contentLayout->count(); ++i){
            QWidgetItem* item = dynamic_cast<QWidgetItem*>(ui->contentLayout->takeAt(i));
            if(item){
                item->widget()->hide();
                ui->contentLayout->removeWidget(item->widget());
                break;
            }
        }
    }

    QTreeWidgetItem* tarItem = ui->SettingList->selectedItems()[0];
    if(!m_page.contains(tarItem))
        return;

    QWidget* page = m_page[tarItem];
    ui->contentLayout->addWidget(page, 0, 0);
    page->show();
}
ScriptHolder *AdditionalSettings::scriptHolder() const
{
    return m_scriptHolder;
}

void AdditionalSettings::setScriptHolder(ScriptHolder *scriptHolder)
{
    m_scriptHolder = scriptHolder;
}

void AdditionalSettings::cleanup()
{
    qDeleteAll(m_page.values());
}


void AdditionalSettings::on_SettingList_itemSelectionChanged()
{
    updateSelection();
    update();
}

void AdditionalSettings::resetValue()
{
    QPushButton* prButton = qobject_cast<QPushButton*>(sender());
    if(!prButton)
        return;

    for(QMap<QWidget*, SimpleAdditionalSetting>::iterator it = m_data.begin(); it != m_data.end(); it++){
        const SimpleAdditionalSetting& sas = (SimpleAdditionalSetting)(*it);
        if(sas.resetControl == prButton){
            if(sas.controlType == 0){
                QSpinBox* spBox = qobject_cast<QSpinBox*>(sas.dataControl);
                if(!spBox)
                    return;

                spBox->setValue(sas.defaultValue.toInt());
            }
        }
    }
}

void AdditionalSettings::spinValChanged()
{
    QSpinBox* prSpin = qobject_cast<QSpinBox*>(sender());
    if(!prSpin)
        return;

    if(!m_data.contains((QWidget*)prSpin))
        return;

    m_data[(QWidget*)prSpin].value = QVariant(prSpin->value());
}

void AdditionalSettings::on_btnReject_clicked()
{
    this->reject();
}

void AdditionalSettings::on_btnAccept_clicked()
{
    EventCommand* evCmd = 0;
    evCmd = m_scriptHolder->findEvent(EventCommand::EVENTTYPE_LOAD);
    if(!evCmd){
        evCmd = new EventCommand(EventCommand::EVENTTYPE_LOAD);
        m_scriptHolder->revents() << evCmd;
    }

    //Clear
    QList<BasicCommand*> cmds = evCmd->findCommandsByMarker("AdditionalSetting");
    foreach(BasicCommand* cmd, cmds){
        evCmd->deleteBasicCommand(cmd);
    }

    //Write
    for(QMap<QWidget*, SimpleAdditionalSetting>::iterator it = m_data.begin(); it != m_data.end(); it++){
        if(((SimpleAdditionalSetting)(*it)).defaultValue == ((SimpleAdditionalSetting)(*it)).value)
            continue;
        MemoryCommand* memCmd = new MemoryCommand(((SimpleAdditionalSetting)(*it)).memAddr, MemoryCommand::FIELD_WORD, (double)((SimpleAdditionalSetting)(*it)).value.toDouble());
        memCmd->setMarker("AdditionalSetting");
        memCmd->setFieldType(static_cast<MemoryCommand::FieldType>(((SimpleAdditionalSetting)(*it)).memAddrType));
        evCmd->addBasicCommand(memCmd);
    }

    //Check if event list itself is empty and delete if necessary:
    if(!evCmd->countCommands()){
        m_scriptHolder->deleteEvent(evCmd);
    }

    this->accept();
}

void AdditionalSettings::loadValuesByScriptHolder(QList<SimpleAdditionalSetting> &data)
{
    EventCommand* evCmd = m_scriptHolder->findEvent(EventCommand::EVENTTYPE_LOAD);
    if(!evCmd)
        return;

    QList<BasicCommand*> basicCmds = evCmd->findCommandsByMarker("AdditionalSetting");
    foreach (BasicCommand* basicCmd, basicCmds) {
        MemoryCommand* memCmd = qobject_cast<MemoryCommand*>(basicCmd);
        if(!memCmd)
            continue;

        for(int i = 0; i < data.size(); ++i){
            if(data[i].memAddr == memCmd->hexValue()){
                data[i].value = memCmd->getValue();
            }
        }

    }
}
