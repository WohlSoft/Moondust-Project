#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QMainWindow>

namespace Ui {
class Calculator;
}

class Calculator : public QMainWindow
{
    Q_OBJECT

public:
    explicit Calculator(QWidget *parent = 0);
    ~Calculator();

private slots:
    void on_time_direction_currentIndexChanged(int);
    void on_time_source_editingFinished();

    void on_speed_direction_currentIndexChanged(int);

    void on_speed_source_editingFinished();

private:
    Ui::Calculator *ui;
};

#endif // CALCULATOR_H
