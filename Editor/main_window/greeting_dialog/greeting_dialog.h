#ifndef GREETING_DIALOG_H
#define GREETING_DIALOG_H

#include <QDialog>

namespace Ui {
class GreetingDialog;
}

class MainWindow;
class GreetingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GreetingDialog(QWidget *parent = 0);
    ~GreetingDialog();

    virtual void keyPressEvent(QKeyEvent *e);

private slots:
    void on_modern_clicked();
    void on_classic_clicked();
signals:
    void switchModern();
    void switchClassic();

private:
    Ui::GreetingDialog *ui;
};

#endif // GREETING_DIALOG_H
