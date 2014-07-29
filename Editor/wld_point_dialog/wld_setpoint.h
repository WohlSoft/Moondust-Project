#ifndef WLD_SETPOINT_H
#define WLD_SETPOINT_H

#include <QDialog>

namespace Ui {
class WLD_SetPoint;
}

class WLD_SetPoint : public QDialog
{
    Q_OBJECT

public:
    explicit WLD_SetPoint(QWidget *parent = 0);
    ~WLD_SetPoint();
    QPoint mapPoint;

private:
    Ui::WLD_SetPoint *ui;
};

#endif // WLD_SETPOINT_H
