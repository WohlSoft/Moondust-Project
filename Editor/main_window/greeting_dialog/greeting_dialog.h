#ifndef GREETING_DIALOG_H
#define GREETING_DIALOG_H

#include <QDialog>

namespace Ui {
class GreetingDialog;
}

class GreetingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GreetingDialog(QWidget *parent = 0);
    ~GreetingDialog();

private:
    Ui::GreetingDialog *ui;
};

#endif // GREETING_DIALOG_H
