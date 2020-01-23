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

private:
    Ui::CaseFixer *ui;
};

#endif // CASE_FIXER_H
