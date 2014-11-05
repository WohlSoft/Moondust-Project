#ifndef CRASHHANDLER_H
#define CRASHHANDLER_H

#include <QDialog>

namespace Ui {
class CrashHandler;
}

class CrashHandler : public QDialog
{
    Q_OBJECT

public:
    explicit CrashHandler(QString& crashText, QWidget *parent = 0);
    ~CrashHandler();

    //Crash Handlers
    static void crashByFlood();
    static void crashByUnhandledException();

    //Crash Handlers end

    static void initCrashHandlers();


private slots:
    void on_pgeForumButton_clicked();
    void on_exitButton_clicked();

private:
    Ui::CrashHandler *ui;
};

#endif // CRASHHANDLER_H
