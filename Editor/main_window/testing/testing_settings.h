#pragma once
#ifndef TESTING_SETTINGS_H
#define TESTING_SETTINGS_H

#include <QDialog>

namespace Ui {
class TestingSettings;
}

class MainWindow;

class TestingSettings : public QDialog
{
    Q_OBJECT

    int m_curItemP1 = 0;
    int m_curItemP2 = 0;
    MainWindow* m_mw = nullptr;
public:
    explicit TestingSettings(QWidget *parent = 0);
    ~TestingSettings();

private slots:
    void on_buttonBox_accepted();
    void reloadStates1(int index);
    void reloadStates2(int index);
    void on_p1_reserved_clicked();
    void on_p2_reserved_clicked();

signals:
    void windowShown();

protected:
    void showEvent(QShowEvent *event);

private:
    Ui::TestingSettings *ui;

    void refreshVehicleID();
    void refreshVehicleTypes();

    void updateNpcButton(QPushButton *button, int npcId);
};

#endif // TESTING_SETTINGS_H
