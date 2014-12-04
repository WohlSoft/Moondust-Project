#include "additionalsettings.h"
#include "ui_additionalsettings.h"

#include <QSettings>

AdditionalSettings::AdditionalSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AdditionalSettings)
{
    ui->setupUi(this);
}

AdditionalSettings::AdditionalSettings(const QString &name, const ScriptHolder &script, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AdditionalSettings)
{
    ui->setupUi(this);
    configGUI(loadSimpleAdditionalSettings(name));
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

                memSetting.beginGroup(obj);

                var_memaddr = memSetting.value("mem-address");
                var_controlType = memSetting.value("control-type");
                var_numRangeBegin = memSetting.value("range-begin", 0);
                var_numRangeEnd = memSetting.value("range-end", 0);
                var_defaultVal = memSetting.value("default-value");
                var_txtLabel = memSetting.value("txt-label");

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

                curSetting.beginRange = var_numRangeBegin.toInt();
                curSetting.endRange = var_numRangeEnd.toInt();

                collectedSettings << curSetting;

                memSetting.endGroup();
            }


            memSetting.endGroup();
        }


        memSetting.endGroup();
    }

    return collectedSettings;
}

void AdditionalSettings::configGUI(const QList<AdditionalSettings::SimpleAdditionalSetting> &data)
{
    for(int i = 0; i < data.size(); ++i){
        const AdditionalSettings::SimpleAdditionalSetting& selData = data[i];

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
            QTreeWidgetItem *searchItem = selGroupItem->child(j);
            if(searchItem->text(0) == selData.group){
                selCatorgyItem = searchItem;
                break;
            }
        }

        if(!selGroupItem){
            selGroupItem = new QTreeWidgetItem(selCatorgyItem);
        }


        QWidget *selWidget = 0;
        QGridLayout *selLayout = 0;
        if(!m_page.contains(selGroupItem)){
            selWidget = new QWidget();;
            selWidget->setLayout(selLayout = new QGridLayout(selWidget));
        }

        selWidget = m_page[selGroupItem];
        selLayout = qobject_cast<QGridLayout*>(selWidget->layout());
    }
}
