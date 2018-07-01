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
#ifndef UTIL_H
#define UTIL_H

#include <QString>
class QDialog;
class QLayout;
class QLineEdit;
class QListWidget;
class QTableWidget;
class QComboBox;
class QTabBar;

class util
{
public:
    /*!
     * \brief Updates vizibility of QListWidget elements in comparison to specified criterias
     * \param searchEdit Text contained in the element name to find
     * \param itemList Pointer to QListWidget with elements where find
     * \param typeBox ComboBox contains type of search (by contained string in a name or ID number of the element)
     */
    static void updateFilter(QLineEdit* searchEdit, QListWidget* itemList, int searchType);
    /*!
     * \brief Removes all entries of QListWidget (with deletion!)
     * \param wid Pointer to QListWidget
     */
    static void memclear(QListWidget* wid);
    /*!
     * \brief Removes all entries of QTableWidget (with deletion!)
     * \param wid Pointer to QTableWidget
     */
    static void memclear(QTableWidget* wid);
    /*!
     * \brief Removes everything (with deletion) from a requested QLayoud
     * \param layout Pointer to QLayout object
     */
    static void clearLayoutItems(QLayout* layout);
    /*!
     * \brief Check if QComboBox has specific item
     * \param b Pointer to ComboBox to check
     * \param s String of the label to check if contains in the combobox
     * \return true if requested string has been found, false if not found
     */
    static bool contains(const QComboBox* b, const QString &s);
    /*!
     * \brief Check if QTabBar has specific item
     * \param b Pointer to ComboBox to check
     * \param s String of the label to check if contains in the combobox
     * \return true if requested string has been found, false if not found
     */
    static bool contains(const QTabBar* b, const QString &s);
    /*!
     * \brief Clears string from invalid file path characters
     * \param s source file path string
     * \return String clear from ivalid characters in the file paths
     */
    static QString filePath(QString s);
    /*!
     * \brief Align opened QDialog to center of the screen
     * \param dialog Pointer to dialog
     * \param CloseButtonOnly Hide all other window buttons except close button
     */
    static void DialogToCenter(QDialog *dialog, bool CloseButtonOnly=false);
    /*!
     * \brief Returns a base name of filename (without extension, but incluses everything after)
     * \param str String with a full filename
     * \return base filename (removed part of string after last dot sign)
     */
    static QString getBaseFilename(QString str);
    /*!
     * \brief Convers number sequence separated with comma into array of integers
     * \param source Source string with number sequence separated with a comma
     * \param dest Target array
     */
    static void CSV2IntArr(QString source, QList<int> &dest);
    /*!
     * \brief Convers number sequence separated with comma into array of integers
     * \param source Source string with number sequence separated with a comma
     * \param dest Target array
     */
    static void CSV2IntArr(QString source, QVector<int> &dest);
    /*!
     * \brief Convers number sequence separated with comma into array of integers
     * \param source Source string with number sequence separated with a comma
     * \param dest Target array
     */
    static void CSV2DoubleArr(QString source, QList<double> &dest);
    /*!
     * \brief Convers number sequence separated with comma into array of integers
     * \param source Source string with number sequence separated with a comma
     * \param dest Target array
     */
    static void CSV2DoubleArr(QString source, QVector<double> &dest);
};

#endif // UTIL_H
