/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2015 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <QLayout>
#include <QWidgetItem>

#include "util.h"

void util::updateFilter(QLineEdit *searchEdit, QListWidget *itemList, QComboBox *typeBox)
{
    QString toSearch;
    toSearch = searchEdit->text();
    for(int i = 0; i < itemList->count(); i++){
        if(toSearch.isEmpty()){
            itemList->setRowHidden(i,false);
            continue;
        }
        if(typeBox->currentIndex()==0){ //search by text
            if(!itemList->item(i)->text().contains(toSearch, Qt::CaseInsensitive)){
                itemList->setRowHidden(i,true);
            }else{
                itemList->setRowHidden(i,false);
            }
        }else if(typeBox->currentIndex()==1){ //search by id
            bool conv = false;
            int toIdSearch = toSearch.toInt(&conv);
            if(!conv){ //cannot convert
                break;
            }
            if(itemList->item(i)->data(3).toInt()==toIdSearch){
                itemList->setRowHidden(i,false);
            }else{
                itemList->setRowHidden(i,true);
            }
        }else{//else do nothing
            break;
        }
    }
}

void util::memclear(QListWidget *wid)
{
    QList<QListWidgetItem*> items = wid->findItems(QString("*"), Qt::MatchWrap | Qt::MatchWildcard);
    while(!items.isEmpty())
    {
        QListWidgetItem *tmp = items.first();
        items.pop_front();
        delete tmp;
    }
}

void util::memclear(QTableWidget *wid)
{
    QList<QTableWidgetItem*> items = wid->findItems(QString("*"), Qt::MatchWrap | Qt::MatchWildcard);
    while(!items.isEmpty())
    {
        QTableWidgetItem *tmp = items.first();
        items.pop_front();
        delete tmp;
    }
}

void util::clearLayoutItems(QLayout *layout)
{
    QLayoutItem *child;
    while ((child = layout->takeAt(0)) != 0) {
        QWidgetItem* i = dynamic_cast<QWidgetItem*>(child);
        if(i){
            delete i->widget();
        }

        delete child;
    }
}

bool util::contains(const QComboBox *b, const QString &s)
{
    for(int i = 0; i < b->count(); ++i){
        if(b->itemText(i) == s){
            return true;
        }
    }
    return false;
}


QString util::filePath(QString s)
{
    QString t = s;
    t.replace('\\', '_');
    t.replace('/', '_');
    t.replace(':', '_');
    t.replace('*', '_');
    t.replace('?', '_');
    t.replace('\"', '_');
    t.replace('<', '_');
    t.replace('>', '_');
    t.replace('|', '_');
    return t;
}
