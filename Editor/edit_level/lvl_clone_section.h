#ifndef LVL_CLONE_SECTION_H
#define LVL_CLONE_SECTION_H

#include <QDialog>

namespace Ui {
class LvlCloneSection;
}

class LvlCloneSection : public QDialog
{
    Q_OBJECT

public:
    explicit LvlCloneSection(QWidget *parent = 0);
    ~LvlCloneSection();

private:
    Ui::LvlCloneSection *ui;
};

#endif // LVL_CLONE_SECTION_H
