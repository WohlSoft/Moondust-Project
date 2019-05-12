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
    void logMessage(const QString &text, const QString &chan);

private slots:
    void on_button_clearAllLogs_clicked();
    void clearCurrentLog();
    void on_button_send_clicked();
    void on_edit_command_returnPressed();

protected:
    void closeEvent(QCloseEvent *event) override;
    void focusInEvent(QFocusEvent *) override;
    void focusOutEvent(QFocusEvent *) override;

private:
    static DevConsole *currentDevConsole;
    explicit DevConsole(QWidget *parent = nullptr);
    ~DevConsole() override;
    Ui::DevConsole *ui;
    void logToConsole(const QString &logText, const QString &channel, bool raise = false);
    QPlainTextEdit *getEditByIndex(const int &index);
    QPlainTextEdit *getCurrentEdit();

    //Command area
    typedef void (DevConsole::*command)(const QStringList&);
    QMap<QString, QPair<command, QString> > commands;
    void registerCommand(const QString &commandName, command cmd, const QString &helpText = QString());
    void registerCommand(const std::initializer_list<QString> &commandNames, DevConsole::command cmd, const QString &helpText);
    void doCommand();

#ifdef ENABLE_CRASH_TESTS
    // Debug only commands, must be disabled in releases! (or Static Analyzers will swear!)
    void doFakeCrash(const QStringList &args);
    void doFlood(const QStringList &args);
    void doThrowUnhandledException(const QStringList&);
    void doSegmentationViolation(const QStringList&);
    void doMemLeakResearch(const QStringList &args);
#endif
    void doHelp(const QStringList &args);
    void doTest(const QStringList &args);
    void doPlayMusic(const QStringList &args);
    void doVersion(const QStringList&);
    void doQuit(const QStringList&);
    void doSaveSettings(const QStringList&);
    void doMd5(const QStringList &args);
    void doValidateStrArray(const QStringList &args);
    void doPgeXTest(const QStringList &args);
    void doSendCheat(const QStringList &args);
    void doOutputPaths(const QStringList &args);
};

#endif // DEVCONSOLE_H
