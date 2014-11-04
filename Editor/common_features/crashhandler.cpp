#include "crashhandler.h"
#include "ui_crashhandler.h"
#include <QDesktopServices>

//temp
#include <QMessageBox>



CrashHandler::CrashHandler(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CrashHandler)
{
    ui->setupUi(this);
}


CrashHandler::~CrashHandler()
{
    delete ui;
}

void CrashHandler::crashByFlood()
{
    QMessageBox::warning(nullptr, QApplication::tr("Crash"), QApplication::tr("We're sorry, but PGE has crashed. Reason: Out of memory! :(\n"
                                                                              "To prevent this, try closing other uneccessary programs to free up more memory."));

    std::exit(1);
}

void CrashHandler::crashByUnhandledException()
{
    std::exception_ptr unhandledException = std::current_exception();
    try{
        std::rethrow_exception(unhandledException);
    }
    catch(const std::exception& e){
        std::string s1 = "We're sorry, but PGE has crashed. Reason: ";
        std::string s2 = e.what();
        std::string s3 = " :(\nPlease inform our forum staff so we can try to fix this problem, Thank you\n\nForum link: engine.wohlnet.ru/forum";

        QMessageBox::warning(nullptr, QApplication::tr("Crash"), QApplication::tr((s1 + s2 + s3).c_str()));
    }

    std::exit(2);
}

void CrashHandler::initCrashHandlers()
{
    std::set_new_handler(&crashByFlood);
    std::set_terminate(&crashByUnhandledException);
}

void CrashHandler::on_pgeForumButton_clicked()
{
    QDesktopServices::openUrl(QUrl("http://engine.wohlnet.ru/forum/"));
}

void CrashHandler::on_exitButton_clicked()
{
    this->close();
}
