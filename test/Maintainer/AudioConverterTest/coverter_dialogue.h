#ifndef COVERTER_DIALOGUE_H
#define COVERTER_DIALOGUE_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
class CoverterDialogue;
}
QT_END_NAMESPACE

class CoverterDialogue : public QDialog
{
    Q_OBJECT

public:
    CoverterDialogue(QWidget *parent = nullptr);
    ~CoverterDialogue();

private:
    Ui::CoverterDialogue *ui;
};
#endif // COVERTER_DIALOGUE_H
