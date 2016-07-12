/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2016 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "safe_msg_box.h"
#include <QMessageBox>

SafeMsgBox::SafeMsgBox(QObject *parent) :
    QObject(parent)
{}

void SafeMsgBox::info(QString title, QString text, int buttons, int *reply)
{
    QWidget* p = qobject_cast<QWidget*>(parent());
    QMessageBox::StandardButton ret = QMessageBox::information(p, title, text, QMessageBox::StandardButton(buttons));
    if(reply)
        *reply=ret;
}

void SafeMsgBox::question(QString title, QString text, int buttons, int *reply)
{
    QWidget* p = qobject_cast<QWidget*>(parent());
    int ret = QMessageBox::question(p, title, text, QMessageBox::StandardButton(buttons));
    if(reply)
        *reply=ret;
}

void SafeMsgBox::warning(QString title, QString text, int buttons, int *reply)
{
    QWidget* p = qobject_cast<QWidget*>(parent());
    int ret = QMessageBox::warning(p, title, text, QMessageBox::StandardButton(buttons));
    if(reply)
        *reply=ret;
}

void SafeMsgBox::critical(QString title, QString text, int buttons, int *reply)
{
    QWidget* p = qobject_cast<QWidget*>(parent());
    int ret = QMessageBox::critical(p, title, text, QMessageBox::StandardButton(buttons));
    if(reply)
        *reply=ret;
}



SafeMsgBoxInterface::SafeMsgBoxInterface(SafeMsgBox *target, QObject *parent) :
    QObject(parent)
{
    if(!target)
        throw("In the SafeMsgBoxInterface constructur must be not-null target!");

    connect(this, SIGNAL(info(QString,QString,int,int*)),
            target, SLOT(info(QString,QString,int,int*)),
            Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(question(QString,QString,int,int*)),
            target, SLOT(question(QString,QString,int,int*)),
            Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(warning(QString,QString,int,int*)),
            target, SLOT(warning(QString,QString,int,int*)),
            Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(critical(QString,QString,int,int*)),
            target, SLOT(critical(QString,QString,int,int*)),
            Qt::BlockingQueuedConnection);
}

int SafeMsgBoxInterface::info(QString title, QString text, int buttons)
{
    int reply = QMessageBox::NoButton;
    emit info(title, text, buttons, &reply);
    return reply;
}

int SafeMsgBoxInterface::question(QString title, QString text, int buttons)
{
    int reply = QMessageBox::NoButton;
    emit question(title, text, buttons, &reply);
    return reply;
}

int SafeMsgBoxInterface::warning(QString title, QString text, int buttons)
{
    int reply = QMessageBox::NoButton;
    emit warning(title, text, buttons, &reply);
    return reply;
}

int SafeMsgBoxInterface::critical(QString title, QString text, int buttons)
{
    int reply = QMessageBox::NoButton;
    emit critical(title, text, buttons, &reply);
    return reply;
}
