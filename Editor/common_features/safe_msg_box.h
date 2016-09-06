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

#pragma once
#ifndef SAFEMSGBOX_H
#define SAFEMSGBOX_H

#include <QObject>

/**
 * @brief Provides thread-save way to show message boxes
 */
class SafeMsgBox : public QObject
{
    Q_OBJECT
public:
    explicit SafeMsgBox(QObject *parent = 0);

signals:

public slots:
    void info(QString title,        QString text,   unsigned long buttons, int *reply=nullptr);
    void question(QString title,     QString text,  unsigned long buttons, int *reply=nullptr);
    void warning(QString title,     QString text,   unsigned long buttons, int *reply=nullptr);
    void critical(QString title,    QString text,   unsigned long buttons, int *reply=nullptr);
    void richBox(QString title,
                 QString text,
                 unsigned long buttons,
                 int msgType,
                 int *reply=nullptr);
};

/**
 * @brief Interface class, declare it to feed a thread
 */
class SafeMsgBoxInterface : public QObject
{
    Q_OBJECT
public:
    explicit SafeMsgBoxInterface(SafeMsgBox* target, QObject*parent = 0);
    int info(QString title,        QString text,   unsigned long buttons);
    int question(QString title,    QString text,   unsigned long buttons);
    int warning(QString title,     QString text,   unsigned long buttons);
    int critical(QString title,    QString text,   unsigned long buttons);
    int richBox(QString title,     QString text,   unsigned long buttons, int msgType);

signals:
    void info(QString title,        QString text,   unsigned long buttons, int *reply);
    void question(QString title,    QString text,   unsigned long buttons, int *reply);
    void warning(QString title,     QString text,   unsigned long buttons, int *reply);
    void critical(QString title,    QString text,   unsigned long buttons, int *reply);
    void richBox(QString title,     QString text,   unsigned long buttons, int msgType, int *reply);
};

#endif // SAFEMSGBOX_H
