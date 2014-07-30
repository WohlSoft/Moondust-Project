#ifndef DEVCONSOLE_H
#define DEVCONSOLE_H

#include <QWidget>

namespace Ui {
class DevConsole;
}

class DevConsole : public QWidget
{
    Q_OBJECT

public:
    explicit DevConsole(QWidget *parent = 0);
    ~DevConsole();

private:
    Ui::DevConsole *ui;
};

#endif // DEVCONSOLE_H
