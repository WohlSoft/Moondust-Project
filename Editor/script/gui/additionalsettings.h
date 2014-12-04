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
    explicit AdditionalSettings(const QString &name, const ScriptHolder &script, QWidget *parent = 0);
    ~AdditionalSettings();


    struct SimpleAdditionalSetting{
        //Setting
        QVariant defaultValue;
        QString category;
        QString group;

        //Optional
        int beginRange;
        int endRange;

        //Actual data
        QVariant value;
    };

    static QList<SimpleAdditionalSetting> loadSimpleAdditionalSettings(const QString &path);
private:

    void configGUI(const QList<SimpleAdditionalSetting> &data);

    Ui::AdditionalSettings *ui;
    QMap<QWidget*, SimpleAdditionalSetting> m_data;
    QMap<QTreeWidgetItem*, QWidget*> m_page;

};

#endif // ADDITIONALSETTINGS_H
