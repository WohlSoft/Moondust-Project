#ifndef LOADINGPROCESS_H
#define LOADINGPROCESS_H

#include <QDialog>

namespace Ui {
class LoadingProcess;
}

class LoadingProcess : public QDialog
{
    Q_OBJECT
    
public:
    explicit LoadingProcess(QWidget *parent = 0);
    ~LoadingProcess();
    
private:
    Ui::LoadingProcess *ui;
};

#endif // LOADINGPROCESS_H
