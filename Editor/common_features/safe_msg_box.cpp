/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2019 Vitaly Novichkov <admin@wohlnet.ru>
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
#include <QApplication>
#include <QMessageBox>
#include <QMutexLocker>
#include <QDesktopWidget>
#include <QRect>
#include <common_features/util.h>

#define ToBTNS(btn) QMessageBox::StandardButtons(QMessageBox::StandardButton(btn))

void SafeMsgBox::initInterface(SafeMsgBoxInterface *source)
{
    QMutexLocker lock(&m_source_mtx); Q_UNUSED(lock);

    if(m_source.find(source) != m_source.end())
        return;
    QObject::connect(source, SIGNAL(info(QString, QString, unsigned long, int *)),
                     this, SLOT(info(QString, QString, unsigned long, int *)),
                     Qt::BlockingQueuedConnection);
    QObject::connect(source, SIGNAL(question(QString, QString, unsigned long, int *)),
                     this, SLOT(question(QString, QString, unsigned long, int *)),
                     Qt::BlockingQueuedConnection);
    QObject::connect(source, SIGNAL(warning(QString, QString, unsigned long, int *)),
                     this, SLOT(warning(QString, QString, unsigned long, int *)),
                     Qt::BlockingQueuedConnection);
    QObject::connect(source, SIGNAL(critical(QString, QString, unsigned long, int *)),
                     this, SLOT(critical(QString, QString, unsigned long, int *)),
                     Qt::BlockingQueuedConnection);
    QObject::connect(source, SIGNAL(richBox(QString, QString, unsigned long, int, int *)),
                     this, SLOT(richBox(QString, QString, unsigned long, int, int *)),
                     Qt::BlockingQueuedConnection);
    m_source.insert(source);
}

void SafeMsgBox::disconnectOne(SafeMsgBoxInterface *source)
{
    QMutexLocker lock(&m_source_mtx); Q_UNUSED(lock);

    if(m_source.find(source) == m_source.end())
        return;
    QObject::disconnect(source, SIGNAL(info(QString, QString, unsigned long, int *)),
                        this, SLOT(info(QString, QString, unsigned long, int *)));
    QObject::disconnect(source, SIGNAL(question(QString, QString, unsigned long, int *)),
                        this, SLOT(question(QString, QString, unsigned long, int *)));
    QObject::disconnect(source, SIGNAL(warning(QString, QString, unsigned long, int *)),
                        this, SLOT(warning(QString, QString, unsigned long, int *)));
    QObject::disconnect(source, SIGNAL(critical(QString, QString, unsigned long, int *)),
                        this, SLOT(critical(QString, QString, unsigned long, int *)));
    QObject::disconnect(source, SIGNAL(richBox(QString, QString, unsigned long, int, int *)),
                        this, SLOT(richBox(QString, QString, unsigned long, int, int *)));
    m_source.remove(source);
}

SafeMsgBox::SafeMsgBox(QObject *parent) :
    QObject(parent)
{}

void SafeMsgBox::disconnectAll()
{
    QMutexLocker lock(&m_source_mtx); Q_UNUSED(lock);

    if(m_source.isEmpty())
        return;

    for(auto *source : m_source)
    {
        QObject::disconnect(source, SIGNAL(info(QString, QString, unsigned long, int *)),
                            this, SLOT(info(QString, QString, unsigned long, int *)));
        QObject::disconnect(source, SIGNAL(question(QString, QString, unsigned long, int *)),
                            this, SLOT(question(QString, QString, unsigned long, int *)));
        QObject::disconnect(source, SIGNAL(warning(QString, QString, unsigned long, int *)),
                            this, SLOT(warning(QString, QString, unsigned long, int *)));
        QObject::disconnect(source, SIGNAL(critical(QString, QString, unsigned long, int *)),
                            this, SLOT(critical(QString, QString, unsigned long, int *)));
        QObject::disconnect(source, SIGNAL(richBox(QString, QString, unsigned long, int, int *)),
                            this, SLOT(richBox(QString, QString, unsigned long, int, int *)));
    }
    m_source.clear();
}

void SafeMsgBox::info(QString title, QString text, unsigned long buttons, int *reply)
{
    QWidget *p = qobject_cast<QWidget *>(parent());
    QMessageBox::StandardButton ret = QMessageBox::information(p, title, text, ToBTNS(buttons));

    if(reply)
        *reply = static_cast<int>(ret);
}

void SafeMsgBox::question(QString title, QString text, unsigned long buttons, int *reply)
{
    QWidget *p = qobject_cast<QWidget *>(parent());
    QMessageBox::StandardButton ret = QMessageBox::question(p, title, text, ToBTNS(buttons));

    if(reply)
        *reply = static_cast<int>(ret);
}

void SafeMsgBox::warning(QString title, QString text, unsigned long buttons, int *reply)
{
    QWidget *p = qobject_cast<QWidget *>(parent());
    QMessageBox::StandardButton ret = QMessageBox::warning(p, title, text, ToBTNS(buttons));

    if(reply)
        *reply = static_cast<int>(ret);
}

void SafeMsgBox::critical(QString title, QString text, unsigned long buttons, int *reply)
{
    QWidget *p = qobject_cast<QWidget *>(parent());
    QMessageBox::StandardButton ret = QMessageBox::critical(p, title, text, ToBTNS(buttons));

    if(reply)
        *reply = static_cast<int>(ret);
}

void SafeMsgBox::richBox(QString title, QString text, unsigned long buttons, int msgType, int *reply)
{
    QWidget *p = qobject_cast<QWidget *>(parent());
    QMessageBox msgBox(p);
    msgBox.setWindowTitle(title);
    msgBox.setWindowModality(Qt::WindowModal);
    msgBox.setTextFormat(Qt::RichText); //this is what makes the links clickable
#if QT_VERSION >= QT_VERSION_CHECK(5, 1, 0)
    msgBox.setTextInteractionFlags(Qt::TextBrowserInteraction);
#endif
    msgBox.setText(text);
    QSize mSize = msgBox.sizeHint();
    QRect screenRect = util::getScreenGeometry();
    msgBox.move(QPoint(screenRect.width() / 2 - mSize.width() / 2,
                       screenRect.height() / 2 - mSize.height() / 2));
    msgBox.setIcon(QMessageBox::Icon(msgType));
    msgBox.setStandardButtons(ToBTNS(buttons));
    int ret = msgBox.exec();

    if(reply)
        *reply = ret;
}



SafeMsgBoxInterface::SafeMsgBoxInterface(SafeMsgBox *target, QObject *parent):
    QObject(parent)
{
    Q_ASSERT(target);
    target->initInterface(this);
    m_target = target;
}

SafeMsgBoxInterface::~SafeMsgBoxInterface()
{
    m_target->disconnectOne(this);
}

int SafeMsgBoxInterface::info(QString title, QString text, unsigned long buttons)
{
    int reply = QMessageBox::NoButton;
    emit info(title, text, buttons, &reply);
    return reply;
}

int SafeMsgBoxInterface::question(QString title, QString text, unsigned long buttons)
{
    int reply = QMessageBox::NoButton;
    emit question(title, text, buttons, &reply);
    return reply;
}

int SafeMsgBoxInterface::warning(QString title, QString text, unsigned long buttons)
{
    int reply = QMessageBox::NoButton;
    emit warning(title, text, buttons, &reply);
    return reply;
}

int SafeMsgBoxInterface::critical(QString title, QString text, unsigned long buttons)
{
    int reply = QMessageBox::NoButton;
    emit critical(title, text, buttons, &reply);
    return reply;
}

int SafeMsgBoxInterface::richBox(QString title, QString text, unsigned long buttons, int msgType)
{
    int reply = QMessageBox::NoButton;
    emit richBox(title, text, buttons, msgType, &reply);
    return reply;
}
