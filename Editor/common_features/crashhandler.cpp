/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
    catch(const std::exception& e)
    {
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
