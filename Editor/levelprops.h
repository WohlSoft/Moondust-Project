#ifndef LEVELPROPS_H
#define LEVELPROPS_H

#include <QDialog>

namespace Ui {
class LevelProps;
}

class LevelProps : public QDialog
{
    Q_OBJECT
    
public:
    explicit LevelProps(QWidget *parent = 0);
    ~LevelProps();
    
private:
    Ui::LevelProps *ui;
};

#endif // LEVELPROPS_H
