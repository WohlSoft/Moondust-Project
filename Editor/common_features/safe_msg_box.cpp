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
#include <QMutex>

SafeMsgBox::SafeMsgBox(QObject *parent) :
    QObject(parent)
{}

void SafeMsgBox::info(QString title, QString text, int buttons, QMutex* locker, int *reply)
{
    QWidget* p = qobject_cast<QWidget*>(parent());
    QMessageBox::StandardButton ret = QMessageBox::information(p, title, text, QMessageBox::StandardButton(buttons));
    if(locker && reply)
    {
        *reply=ret;
        locker->unlock();
    }
}

void SafeMsgBox::question(QString title, QString text, int buttons, QMutex *locker, int *reply)
{
    QWidget* p = qobject_cast<QWidget*>(parent());
    int ret = QMessageBox::question(p, title, text, QMessageBox::StandardButton(buttons));
    if(locker && reply)
    {
        *reply=ret;
        locker->unlock();
    }
}

void SafeMsgBox::warning(QString title, QString text, int buttons, QMutex *locker, int *reply)
{
    QWidget* p = qobject_cast<QWidget*>(parent());
    int ret = QMessageBox::warning(p, title, text, QMessageBox::StandardButton(buttons));
    if(locker && reply)
    {
        *reply=ret;
        locker->unlock();
    }
}

void SafeMsgBox::critical(QString title, QString text, int buttons, QMutex *locker, int *reply)
{
    QWidget* p = qobject_cast<QWidget*>(parent());
    int ret = QMessageBox::critical(p, title, text, QMessageBox::StandardButton(buttons));
    if(locker && reply)
    {
        *reply=ret;
        locker->unlock();
    }
}



SafeMsgBoxInterface::SafeMsgBoxInterface(SafeMsgBox *target, QObject *parent) :
    QObject(parent)
{
    if(!target)
        throw("In the SafeMsgBoxInterface constructur must be not-null target!");

    connect(this, SIGNAL(info(QString,QString,int,QMutex*,int*)),     target, SLOT(info(QString,QString,int,QMutex*,int*)));
    connect(this, SIGNAL(question(QString,QString,int,QMutex*,int*)), target, SLOT(question(QString,QString,int,QMutex*,int*)));
    connect(this, SIGNAL(warning(QString,QString,int,QMutex*,int*)),  target, SLOT(warning(QString,QString,int,QMutex*,int*)));
    connect(this, SIGNAL(critical(QString,QString,int,QMutex*,int*)), target, SLOT(critical(QString,QString,int,QMutex*,int*)));
}

int SafeMsgBoxInterface::info(QString title, QString text, int buttons)
{
    int reply = QMessageBox::NoButton;
    QMutex locker;
    locker.lock();
    emit info(title, text, buttons, &locker, &reply);
    locker.lock();
    locker.unlock();
    return reply;
}

int SafeMsgBoxInterface::question(QString title, QString text, int buttons)
{
    int reply = QMessageBox::NoButton;
    QMutex locker;
    locker.lock();
    emit question(title, text, buttons, &locker, &reply);
    locker.lock();
    locker.unlock();
    return reply;
}

int SafeMsgBoxInterface::warning(QString title, QString text, int buttons)
{
    int reply = QMessageBox::NoButton;
    QMutex locker;
    locker.lock();
    emit warning(title, text, buttons, &locker, &reply);
    locker.lock();
    locker.unlock();
    return reply;
}

int SafeMsgBoxInterface::critical(QString title, QString text, int buttons)
{
    int reply = QMessageBox::NoButton;
    QMutex locker;
    locker.lock();
    emit critical(title, text, buttons, &locker, &reply);
    locker.lock();
    locker.unlock();
    return reply;
}
