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


#include "image_list_menu.h"
#include <QListWidget>
#include <QListWidgetItem>

#include <QMenu>

#include <QtDebug>

ImageListMenu::ImageListMenu(QObject *parent) :
        QWidgetAction(parent)
{}

QWidget *ImageListMenu::createWidget(QWidget *parent)
{
    if(m_widget)
    {
        m_widget->setParent(parent);
        return m_widget;
    }

    m_widget = new QListWidget(parent);
    m_widget->setViewMode(QListWidget::IconMode);
    m_widget->setMinimumSize(600, 600);
    m_widget->setIconSize(QSize(100, 75));
    m_widget->setSpacing(2);
    m_widget->setDragEnabled(false);
    m_widget->setUniformItemSizes(true);
    QStringList l;
    l << "lel" << "lol" << "wtf?" << "rofl" << "kek" << "hai" << "Go eat cricket ugly";
    {
        auto *akt = new QListWidgetItem(m_widget);
        QPixmap empty(100, 70);
        empty.fill(QColor(Qt::black));
        akt->setIcon(QIcon(empty));
        akt->setText(tr("[No image]"));
        akt->setData(Qt::UserRole, 0);
        m_widget->addItem(akt);
    }

    int counter = 1;
    for(auto &i : l)
    {
        auto *akt = new QListWidgetItem(m_widget);
        QPixmap empty(100, 70);
        empty.fill(QColor(Qt::blue));
        akt->setIcon(QIcon(empty));
        akt->setText(QString("%1: %2").arg(counter).arg(i));
        akt->setData(Qt::UserRole, counter);
        m_widget->addItem(akt);
        counter++;
    }

    QObject::connect(m_widget, &QListWidget::itemClicked, [this, parent](QListWidgetItem *item)
    {
        auto *menu = qobject_cast<QMenu*>(parent);
        if(item)
            qDebug() << "Clicked image: " << item->data(Qt::UserRole) << item->text();
        else
            qDebug() << "Clicked nothing!";
        emit triggered();
        if(menu)
            menu->close();
    });

    return m_widget;
}

void ImageListMenu::deleteWidget(QWidget *widget)
{
    if(widget == m_widget)
    {
        delete widget;
        m_widget = nullptr;
    }
}
