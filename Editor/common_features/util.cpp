/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
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
#include <QDialog>
#include <QApplication>
#include <QDesktopWidget>
#include <QScreen>
#include <QStyle>
#include <QLineEdit>
#include <QListWidget>
#include <QTableWidget>
#include <QComboBox>
#include <QTabBar>

#include "util.h"

void util::updateFilter(QLineEdit *searchEdit, QListWidget *itemList, int searchType)
{
    QString toSearch;
    toSearch = searchEdit->text();
    for(int i = 0; i < itemList->count(); i++)
    {
        QListWidgetItem *item = itemList->item(i);
        if(toSearch.isEmpty())
        {
            item->setHidden(false);
            continue;
        }

        switch(searchType)
        {
        case 0:
        {
            //search by text
            item->setHidden(!item->text().contains(toSearch, Qt::CaseInsensitive));
            break;
        }
        case 1:
        {
            //search by id
            bool conv = false;
            int toIdSearch = toSearch.toInt(&conv);
            if(!conv)
                break;//cannot convert
            item->setHidden(item->data(Qt::UserRole).toInt() != toIdSearch);
            break;
        }
        case 2:
        {
            //search by ID (contents)
            item->setHidden(!item->data(Qt::UserRole).toString().contains(toSearch, Qt::CaseInsensitive));
            break;
        }
        default:
        {
            //else do nothing
            break;
        }
        }
    }
    itemList->update();
}

void util::memclear(QListWidget *wid)
{
    QList<QListWidgetItem *> items = wid->findItems(QString("*"), Qt::MatchWrap | Qt::MatchWildcard);
    while(!items.isEmpty())
    {
        QListWidgetItem *tmp = items.first();
        items.pop_front();
        delete tmp;
    }
}

void util::memclear(QTableWidget *wid)
{
    QList<QTableWidgetItem *> items = wid->findItems(QString("*"), Qt::MatchWrap | Qt::MatchWildcard);
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
    while((child = layout->takeAt(0)) != nullptr)
    {
        QWidgetItem *i = dynamic_cast<QWidgetItem *>(child);
        if(i)
            delete i->widget();

        delete child;
    }
}

bool util::contains(const QComboBox *b, const QString &s)
{
    for(int i = 0; i < b->count(); ++i)
    {
        if(b->itemText(i) == s)
            return true;
    }
    return false;
}

bool util::contains(const QTabBar *b, const QString &s)
{
    for(int i = 0; i < b->count(); ++i)
    {
        if(b->tabText(i) == s)
            return true;
    }
    return false;
}

/*!
 * \brief Clears string from invalid file path characters
 * \param s source file path string
 * \return String clean from ivalid characters in the file paths
 */
QString util::filePath(QString s)
{
    QString t = s;
    t.replace('\\', '_');
    t.replace('/', '_');
    t.replace(':', '_');
    t.replace(';', '_');
    t.replace('*', '_');
    t.replace('?', '_');
    t.replace('\"', '_');
    t.replace('<', '_');
    t.replace('>', '_');
    t.replace('|', '_');
    return t;
}


void util::DialogToCenter(QDialog *dialog, bool CloseButtonOnly)
{
    if(CloseButtonOnly)
        dialog->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    dialog->setGeometry(alignToScreenCenter(dialog->size()));
}

QString util::getBaseFilename(QString str)
{
    //why >0 and not >=0? Because >=0 means that possible to have empty basename (with zero length, like .htaccess)
    for(int i = str.size() - 1; i > 0; i--)
    {
        if(str[i] == '.')
        {
            str.resize(i);
            break;
        }
    }
    return str;
}

template<class TList>
inline void CSV2IntArr_CODE(const QString &source, TList &dest, const typename TList::value_type &def)
{
    typedef typename TList::value_type T;
    if(!source.isEmpty())
    {
        bool ok;
        QStringList tmlL = source.split(',', QString::SkipEmptyParts);
        for(QString &fr : tmlL)
        {
            if(std::is_same<T, int>::value)
                dest.push_back(fr.toInt(&ok));
            else
                dest.push_back(fr.toDouble(&ok));
            if(!ok) dest.pop_back();
        }
        if(dest.isEmpty()) dest.push_back(def);
    }
    else
        dest.push_back(def);
}

void util::CSV2IntArr(QString source, QList<int> &dest)
{
    CSV2IntArr_CODE(source, dest, 0);
}

void util::CSV2IntArr(QString source, QVector<int> &dest)
{
    CSV2IntArr_CODE(source, dest, 0);
}

void util::CSV2DoubleArr(QString source, QList<double> &dest)
{
    CSV2IntArr_CODE(source, dest, 0.0);
}

void util::CSV2DoubleArr(QString source, QVector<double> &dest)
{
    CSV2IntArr_CODE(source, dest, 0.0);
}

QRect util::getScreenGeometry(int screenIndex)
{
    QScreen *screen = QGuiApplication::primaryScreen();
    if(screenIndex < 0)
        return screen ? screen->geometry() : qApp->desktop()->geometry();
    else
    {
        QList<QScreen*> screens = QGuiApplication::screens();
        if(screenIndex >= screens.size())
            return qApp->desktop()->geometry();
        else
            return screens[screenIndex]->geometry();
    }
}

QRect util::alignToScreenCenter(const QSize size)
{
    return QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size, getScreenGeometry());
}
