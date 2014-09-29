#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <QDialog>
#include <QListWidget>
#include <QSettings>
#include <QPixmap>

namespace Ui {
class ConfigManager;
}

class ConfigManager : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigManager(QWidget *parent = 0);
    ~ConfigManager();
    QString currentConfig;
    QString themePack;
    QString isPreLoaded();

private slots:
    void on_configList_itemDoubleClicked(QListWidgetItem *item);
    void on_buttonBox_accepted();

private:
    Ui::ConfigManager *ui;
};

#endif // CONFIG_MANAGER_H
