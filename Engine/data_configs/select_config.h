#ifndef SELECT_CONFIG_H
#define SELECT_CONFIG_H

#include <QDialog>
#include <QListWidget>
#include <QSettings>
#include <QPixmap>


namespace Ui {
class SelectConfig;
}

class SelectConfig : public QDialog
{
    Q_OBJECT

public:
    explicit SelectConfig(QWidget *parent = 0);
    ~SelectConfig();
    QString currentConfig;
    QString themePack;
    QString isPreLoaded(QString openConfig="");

private slots:
    void on_configList_itemDoubleClicked(QListWidgetItem *item);
    void on_buttonBox_accepted();

private:
    Ui::SelectConfig *ui;
};

#endif // SELECT_CONFIG_H
