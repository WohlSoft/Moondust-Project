#ifndef CONFIGSTATUS_H
#define CONFIGSTATUS_H

#include <QDialog>
#include "data_configs.h"
#include "../main_window/global_settings.h"

namespace Ui {
class ConfigStatus;
}

class ConfigStatus : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigStatus(dataconfigs &conf, QWidget *parent = 0);
    ~ConfigStatus();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::ConfigStatus *ui;
    dataconfigs * configs;
};

#endif // CONFIGSTATUS_H
