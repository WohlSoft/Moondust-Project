#ifndef ITEMMSGBOX_H
#define ITEMMSGBOX_H

#include <QDialog>

namespace Ui {
class ItemMsgBox;
}

class ItemMsgBox : public QDialog
{
    Q_OBJECT

public:
    QString currentText;
    explicit ItemMsgBox(QString text="", QString label="", QString title="", QWidget *parent = 0);
    ~ItemMsgBox();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::ItemMsgBox *ui;
};

#endif // ITEMMSGBOX_H
