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

    bool    autoPlayMusic;
    bool    Animation;
    long    AnimationItemLimit;

    bool    Collisions;

    bool    logEnabled;
    int     loglevel;
    QString logfile;

    void applySettings();


private slots:
    void on_setLogFile_clicked();

    void on_buttonBox_accepted();

private:
    Ui::AppSettings *ui;
};

#endif // APPSETTINGS_H
