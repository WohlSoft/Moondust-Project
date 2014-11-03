#ifndef DEVCONSOLE_H
#define DEVCONSOLE_H

#include <QWidget>
#include <QPlainTextEdit>
#include <initializer_list>

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

    static void retranslate();

    void registerCommands();
    void retranslateP();

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

    //Command area
    typedef void (DevConsole::*command)(QStringList);
    QMap<QString,QPair<command, QString> > commands;
    void registerCommand(const QString commandName, command cmd, const QString helpText = QString());
    void registerCommand(const std::initializer_list<QString> commandNames, DevConsole::command cmd, const QString helpText);
    void doCommand();

    void doHelp(QStringList args);
    void doTest(QStringList args);
    void doVersion(QStringList);
    void doQuit(QStringList);
    void doSavesettings(QStringList);
    void doMd5(QStringList args);
    void doFlood(QStringList args);
    void doValidateStrArray(QStringList args);
    void doThrowUnhandledException(QStringList);
};

#endif // DEVCONSOLE_H
