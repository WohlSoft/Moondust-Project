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
        SAVE_SAVE,
        SAVE_DISCARD,
        SAVE_CANCLE
    };

    enum DialogType
    {
        D_WARN,
        D_QUESTION
    };

    //any settings placed HERE
    SaveMode savemode;

    explicit SavingNotificationDialog(bool showDiscardButton, DialogType dType=D_QUESTION, QWidget *parent = 0);
    ~SavingNotificationDialog();

    void setSavingTitle(const QString &title);
    void setAdjustSize(const int &width, const int &height);
    void addUserItem(const QString &desc, QWidget* wid);

private slots:
    //void on_button_OK_clicked();
    void on_button_Discard_clicked();
    void on_button_Cancle_clicked();

    void on_button_Save_clicked();

private:
    Ui::SavingNotificationDialog *ui;
};

#endif // SAVINGNOTIFICATIONDIALOG_H
