#ifndef CASE_FIXER_H
#define CASE_FIXER_H

#include <QDialog>

namespace Ui {
class CaseFixer;
}

class CaseFixer : public QDialog
{
    Q_OBJECT

public:
    explicit CaseFixer(QWidget *parent = nullptr);
    ~CaseFixer();

private slots:
    void on_episodeBrowse_clicked();
    void on_configPackBrowse_clicked();

private:
    Ui::CaseFixer *ui;
};

#endif // CASE_FIXER_H
