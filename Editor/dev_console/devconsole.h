#ifndef DEVCONSOLE_H
#define DEVCONSOLE_H

#include <QWidget>
#include <QPlainTextEdit>

namespace Ui {
class DevConsole;
}

class DevConsole : public QWidget
{
    Q_OBJECT

public:
    static void init();
    static void show();
    static void log(const QString &logText, const QString &channel = QString("System"), bool raise=false);
    static bool isConsoleShown();

private slots:
    void on_button_clearAllLogs_clicked();
    void clearCurrentLog();
    void on_button_send_clicked();
    void on_edit_command_returnPressed();

protected:
    void closeEvent ( QCloseEvent * event);

private:
    static DevConsole *currentDevConsole;
    explicit DevConsole(QWidget *parent = 0);
    ~DevConsole();
    Ui::DevConsole *ui;
    void logToConsole(const QString &logText, const QString &channel, bool raise=false);
    QPlainTextEdit* getEditByIndex(const int &index);
    QPlainTextEdit* getCurrentEdit();

    void doCommand();
};

#endif // DEVCONSOLE_H
