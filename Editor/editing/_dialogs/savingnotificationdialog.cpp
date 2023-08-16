/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2023 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "savingnotificationdialog.h"
#include <ui_savingnotificationdialog.h>
#ifdef Q_OS_WIN
#include <QtWin>
#include <QSysInfo>
#endif

SavingNotificationDialog::SavingNotificationDialog(bool showDiscardButton, DialogType dType, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SavingNotificationDialog)
{
    ui->setupUi(this);

    if(parent)
    {
        this->setWindowIcon(parent->windowIcon());
    }
    else
    {
#ifdef Q_OS_MAC
        this->setWindowIcon(QIcon(":/appicon/app.icns"));
#endif
#ifdef Q_OS_WIN
        this->setWindowIcon(QIcon(":/appicon/app.ico"));
#endif
    }

#ifdef Q_OS_WIN
    if(QSysInfo::WindowsVersion >= QSysInfo::WV_VISTA &&
       QSysInfo::WindowsVersion <= QSysInfo::WV_WINDOWS7)
    {
        if(QtWin::isCompositionEnabled())
        {
            this->setAttribute(Qt::WA_TranslucentBackground, true);
            QtWin::extendFrameIntoClientArea(this, -1,-1,-1,-1);
            QtWin::enableBlurBehindWindow(this);
        }
        else
        {
            QtWin::resetExtendedFrame(this);
            setAttribute(Qt::WA_TranslucentBackground, false);
        }
    }

    switch(dType)
    {
        case D_WARN:       PlaySoundW(L"SystemAsterisk", NULL, SND_ASYNC|SND_NODEFAULT); break;
        case D_QUESTION:   PlaySoundW(L"SystemQuestion", NULL, SND_ASYNC|SND_NODEFAULT); break;
    }
#endif
    switch(dType)
    {
        case D_WARN:       ui->icon->setPixmap(QPixmap(":/dialogues/question.png"));break;
        case D_QUESTION:   ui->icon->setPixmap(QPixmap(":/dialogues/warn.png"));break;
    }


    ui->button_Discard->setVisible(showDiscardButton);
}

SavingNotificationDialog::~SavingNotificationDialog()
{
    delete ui;
}

void SavingNotificationDialog::setSavingTitle(const QString &title)
{
    ui->label_Title->setText(title);
}

void SavingNotificationDialog::fixSize()
{
    setFixedSize(sizeHint());
}

void SavingNotificationDialog::addUserItem(const QString &desc, QWidget *wid)
{
    ui->userItems->addWidget(new QLabel(desc), ui->userItems->count()/2, 1);
    ui->userItems->addWidget(wid, ui->userItems->count()/2, 2);
}

void SavingNotificationDialog::on_button_Save_clicked()
{
    savemode = SAVE_SAVE;
    accept();
}

void SavingNotificationDialog::on_button_Discard_clicked()
{
    savemode = SAVE_DISCARD;
    accept();
}

void SavingNotificationDialog::on_button_Cancle_clicked()
{
    savemode = SAVE_CANCLE;
    accept();
}
