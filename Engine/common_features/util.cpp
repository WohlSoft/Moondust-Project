/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2016 Vitaly Novichkov <admin@wohlnet.ru>
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


#include "util.h"
#include <QLayout>
#include <QWidgetItem>
#include <QFileInfo>

#ifdef _WIN32
#include <windows.h>
#endif

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

bool util::strempty(const char *str)
{
    if(str){
        if(strlen(str))
            return true;
    }
    return false;
}

QString util::resolveRelativeOrAbsolute(const QString& path, const QStringList &relativeLookup)
{
    if(QFileInfo(path).isAbsolute()){
        if(QFileInfo(path).exists())
            return path;
    }else{
        foreach(QString nextpath, relativeLookup){
            QString newCompletePath = nextpath + "/" + path;
            if(QFileInfo(newCompletePath).exists())
                return newCompletePath;
        }
    }
    return "";
}

#define CSV2IntArr_CODE(source, dest, func, def)\
    \
    if(!source.isEmpty())\
    {\
        bool ok;\
        QStringList tmlL = source.split(',', QString::SkipEmptyParts);\
        foreach(QString fr, tmlL)\
        {\
            dest.push_back(fr.func(&ok));\
            if(!ok) dest.pop_back();\
        }\
        if(dest.isEmpty()) dest.push_back(def);\
    }\
    else\
    {\
        dest.push_back(def);\
    }

void util::CSV2IntArr(QString source, QList<int> &dest)
{
    CSV2IntArr_CODE(source, dest, toInt, 0)
}

void util::CSV2IntArr(QString source, QVector<int> &dest)
{
    CSV2IntArr_CODE(source, dest, toInt, 0)
}

void util::CSV2DoubleArr(QString source, QList<double> &dest)
{
    CSV2IntArr_CODE(source, dest, toDouble, 0.0)
}

void util::CSV2DoubleArr(QString source, QVector<double> &dest)
{
    CSV2IntArr_CODE(source, dest, toDouble, 0.0)
}

size_t charsets_utils::utf8len(const char *s)
{
    size_t len = 0;
    while(*s)
        len += (*(s++)&0xC0)!=0x80;
    return len;
}


int charsets_utils::UTF8Str_To_WStr(std::wstring &dest, const std::string &source)
{
    #ifdef _WIN32
    dest.resize(utf8len(source.c_str()));
    return MultiByteToWideChar(CP_UTF8, 0, source.c_str(), source.length(), (wchar_t*)dest.c_str(), source.length());
    #else
    (void)dest; (void)source;
    return utf8len(source.c_str());
    #endif
}

int charsets_utils::WStr_To_UTF8Str(std::string &dest, const std::wstring &source)
{
    #ifdef _WIN32
    int dest_len = WideCharToMultiByte(CP_UTF8, 0, source.c_str(), source.length(), (LPSTR)dest.c_str(), 0, NULL, NULL);
    dest.resize(dest_len);
    WideCharToMultiByte(CP_UTF8, 0, source.c_str(), source.length(), (LPSTR)dest.c_str(), dest_len, NULL, NULL);
    return dest_len;
    #else
    (void)dest; (void)source;
    return source.size();
    #endif
}
