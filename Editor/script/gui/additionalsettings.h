#ifndef ADDITIONALSETTINGS_H
#define ADDITIONALSETTINGS_H

#include <QDialog>

namespace Ui {
class AdditionalSettings;
}

class AdditionalSettings : public QDialog
{
    Q_OBJECT

public:
    explicit AdditionalSettings(QWidget *parent = 0);
    ~AdditionalSettings();

private:
    Ui::AdditionalSettings *ui;
};

#endif // ADDITIONALSETTINGS_H
