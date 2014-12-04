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
    explicit AdditionalSettings(QWidget *parent = 0);
    explicit AdditionalSettings(const QString &name, const ScriptHolder *script, QWidget *parent = 0);
    ~AdditionalSettings();


    struct SimpleAdditionalSetting{
        //Setting
        QVariant defaultValue;
        QString category;
        QString group;
        QString labelTxt;
        int controlType;

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
private:

    void configGUI(QList<SimpleAdditionalSetting> &data);
    void selectFirstValidItem();

    Ui::AdditionalSettings *ui;
    /* Key: Pointer to Widget = Data Widget (i.e. QSpinBox) | Value: SimpleAdditionalSetting */
    QMap<QWidget*, SimpleAdditionalSetting> m_data;
    QMap<QTreeWidgetItem*, QWidget*> m_page;

};

#endif // ADDITIONALSETTINGS_H
