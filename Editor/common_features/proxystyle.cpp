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

#include "proxystyle.h"

int PGE_ProxyStyle::styleHint(QStyle::StyleHint hint, const QStyleOption *option, const QWidget *widget, QStyleHintReturn *returnData) const
{

//    Combobox fix by Guilherme Nascimento:
//    http://stackoverflow.com/questions/20554940/qcombobox-pop-up-expanding-and-qtwebkit

    if(hint == QStyle::SH_ComboBox_Popup) {
        const QComboBox *combo = (QComboBox *) widget;//convert qwidget in QComboBox
        const QObjectList a = combo->children();
        const int j = a.count();
        QAbstractItemView *view = 0;
        QString className = "";
        bool hasView = false;

        /*
            at this point I have not used combo->view() because he "crash" the application without explanation
            so I had to make a loop to find the "view"
            */
        for(int i = 0; i < j; ++i) {
            const QObjectList b = a.at(i)->children();
            const int y = b.count();

            for(int x = 0; x < y; ++x) {
                className = b.at(x)->metaObject()->className();
                if(className == "QComboBoxListView") {
                    view = (QAbstractItemView *) b.at(x);
                    hasView = true;
                    break;
                }
            }

            if (hasView == true) {
                break;
            }
        }

        if(hasView == true) {
            const int iconSize = combo->iconSize().width();
            const QFontMetrics fontMetrics1 = view->fontMetrics();
            const QFontMetrics fontMetrics2 = combo->fontMetrics();
            const int j = combo->count();
            int width = combo->width(); //default width

            for(int i=0; i < j; ++i) {
                const int textWidth = qMax(
                            fontMetrics1.width(combo->itemText(i) + "WW"),
                            fontMetrics2.width(combo->itemText(i) + "WW")
                            );
                if(combo->itemIcon(i).isNull()) {
                    width = qMax(width, textWidth);
                } else {
                    width = qMax(width, textWidth + iconSize);
                }
            }

            view->setFixedWidth(width);
        }
    }
//  Combobox fix end

    return QProxyStyle::styleHint(hint, option, widget, returnData);
}
