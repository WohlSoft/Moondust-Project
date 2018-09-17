#ifndef USER_DATA_EDIT_H
#define USER_DATA_EDIT_H

#include <QDialog>

namespace Ui {
class UserDataEdit;
}

class UserDataEdit : public QDialog
{
    Q_OBJECT

public:
    explicit UserDataEdit(QWidget *parent = nullptr);
    explicit UserDataEdit(const QString &text, QWidget *parent = nullptr);

    QString getText();
    void setText(const QString &text);

    ~UserDataEdit();

protected:
    void changeEvent(QEvent *e);

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    Ui::UserDataEdit *ui;
};

#endif // USER_DATA_EDIT_H
