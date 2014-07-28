#ifndef SAVINGNOTIFICATIONDIALOG_H
#define SAVINGNOTIFICATIONDIALOG_H

#include <QDialog>

namespace Ui {
class SavingNotificationDialog;
}

class SavingNotificationDialog : public QDialog
{
    Q_OBJECT

public:

    enum SaveMode{
        SAVE_OK,
        SAVE_DISCARD,
        SAVE_CANCLE
    };

    //any settings placed HERE
    SaveMode savemode;

    explicit SavingNotificationDialog(bool showDiscardButton, QWidget *parent = 0);
    ~SavingNotificationDialog();

private slots:
    void on_button_OK_clicked();

    void on_button_Discard_clicked();

    void on_button_Cancle_clicked();

private:
    Ui::SavingNotificationDialog *ui;
};

#endif // SAVINGNOTIFICATIONDIALOG_H
