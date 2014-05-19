#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QDialog>

namespace Ui {
class AppSettings;
}

class AppSettings : public QDialog
{
    Q_OBJECT

public:
    explicit AppSettings(QWidget *parent = 0);
    ~AppSettings();

private:
    Ui::AppSettings *ui;
};

#endif // APPSETTINGS_H
