#pragma once
#ifndef TESTING_SETTINGS_H
#define TESTING_SETTINGS_H

#include <QDialog>

namespace Ui {
class TestingSettings;
}

class TestingSettings : public QDialog
{
    Q_OBJECT

public:
    explicit TestingSettings(QWidget *parent = 0);
    ~TestingSettings();

private slots:
    void on_buttonBox_accepted();
signals:
    void windowShown();

protected:
    void showEvent(QShowEvent *event);

private:
    Ui::TestingSettings *ui;
};

#endif // TESTING_SETTINGS_H
