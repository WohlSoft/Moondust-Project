#pragma once
#ifndef DEVCONSOLE_H
#define DEVCONSOLE_H

#include <QWidget>
#include <QPlainTextEdit>
#include <initializer_list>

#if 1//#ifdef DEBUG_BUILD
#define ENABLE_CRASH_TESTS
#endif

namespace Ui
{
    class DevConsole;
}

class DevConsole : public QWidget
{
    Q_OBJECT

public:
    static void init();
    static void show();
    static void log(const QString &logText, const QString &channel = QString("System"), bool raise = false);
    static bool isConsoleShown();
    static void closeIfPossible();

    static void retranslate();

    void registerCommands();
    void retranslateP();
public slots:
    void logMessage(QString text, QString chan);

private slots:
    void on_button_clearAllLogs_clicked();
    void clearCurrentLog();
    void on_button_send_clicked();
    void on_edit_command_returnPressed();

protected:
    void closeEvent(QCloseEvent *event);
    void focusInEvent(QFocusEvent *);
    void focusOutEvent(QFocusEvent *);

private:
    static DevConsole *currentDevConsole;
    explicit DevConsole(QWidget *parent = 0);
    ~DevConsole();
    Ui::DevConsole *ui;
    void logToConsole(const QString &logText, const QString &channel, bool raise = false);
    QPlainTextEdit *getEditByIndex(const int &index);
    QPlainTextEdit *getCurrentEdit();

    //Command area
    typedef void (DevConsole::*command)(QStringList);
    QMap<QString, QPair<command, QString> > commands;
    void registerCommand(const QString commandName, command cmd, const QString helpText = QString());
    void registerCommand(const std::initializer_list<QString> commandNames, DevConsole::command cmd, const QString helpText);
    void doCommand();

    #ifdef ENABLE_CRASH_TESTS
    // Debug only commands, must be disabled in releases! (or Static Analyzers will swear!)
    void doFakeCrash(QStringList args);
    void doFlood(QStringList args);
    void doThrowUnhandledException(QStringList);
    void doSegmentationViolation(QStringList);
    void doMemLeakResearch(QStringList args);
    #endif
    void doHelp(QStringList args);
    void doTest(QStringList args);
    void doPlayMusic(QStringList args);
    void doVersion(QStringList);
    void doQuit(QStringList);
    void doSavesettings(QStringList);
    void doMd5(QStringList args);
    void doValidateStrArray(QStringList args);
    void doPgeXTest(QStringList args);
    void doSendCheat(QStringList args);
    void doOutputPaths(QStringList args);
};

#endif // DEVCONSOLE_H
