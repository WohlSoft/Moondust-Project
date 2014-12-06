#ifndef ADDITIONALSETTINGS_H
#define ADDITIONALSETTINGS_H

#include <QDialog>
#include "../scriptholder.h"
#include <QTreeWidget>

namespace Ui {
class AdditionalSettings;
}

class AdditionalSettings : public QDialog
{
    Q_OBJECT

public:
    explicit AdditionalSettings(const QString &name, ScriptHolder *script, QWidget *parent = 0);
    ~AdditionalSettings();


    struct SimpleAdditionalSetting{
        //Setting
        QVariant defaultValue;
        QString category;
        QString group;
        QString labelTxt;
        int controlType;
        int memAddr;

        //Optional
        int beginRange;
        int endRange;

        //Binded Controls
        QWidget* dataControl;
        QPushButton* resetControl;

        //Actual data
        QVariant value;
    };

    static QList<SimpleAdditionalSetting> loadSimpleAdditionalSettings(const QString &path);
    ScriptHolder *scriptHolder() const;
    void setScriptHolder(ScriptHolder *scriptHolder);

    void cleanup();

private slots:
    void on_SettingList_itemSelectionChanged();
    void resetValue();
    void spinValChanged();
    void on_btnReject_clicked();
    void on_btnAccept_clicked();

private:

    void loadValuesByScriptHolder(QList<SimpleAdditionalSetting> &data);

    void configGUI(QList<SimpleAdditionalSetting> &data);
    void selectFirstValidItem();
    void updateSelection();

    Ui::AdditionalSettings *ui;
    /* Key: Pointer to Widget = Data Widget (i.e. QSpinBox) | Value: SimpleAdditionalSetting */
    QMap<QWidget*, SimpleAdditionalSetting> m_data;
    QMap<QTreeWidgetItem*, QWidget*> m_page;

    ScriptHolder* m_scriptHolder;

};

#endif // ADDITIONALSETTINGS_H
