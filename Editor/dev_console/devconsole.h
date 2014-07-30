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
    static void init();
    static void show();
    static void log(const QString &logText, const QString &channel = QString("System"));

private:
    static DevConsole *currentDevConsole;
    explicit DevConsole(QWidget *parent = 0);
    ~DevConsole();
    Ui::DevConsole *ui;
    void logToConsole(const QString &logText, const QString &channel);
};

#endif // DEVCONSOLE_H
