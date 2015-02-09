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

#ifndef PGE_X_H
#define PGE_X_H

#include <QPair>
#include <QString>
#include <QStringList>
#include <QObject>

typedef QPair<QString, QStringList> PGEXSct;

class PGEFile: public QObject
{
    Q_OBJECT

public:
    enum PGEX_Item_type
    {
        PGEX_Struct=0,
        PGEX_PlainText
    };


    struct PGEX_Val
    {
        QString marker;
        QString value;
    };

    struct PGEX_Item
    {
        PGEX_Item_type type;
        QList<PGEX_Val > values;
    };

    struct PGEX_Entry
    {
        QString name;
        PGEX_Item_type type;
        QList<PGEX_Item > data;
        QList<PGEX_Entry > subTree;
    };

    PGEFile(QObject *parent=NULL);
    PGEFile(PGEFile &pgeFile, QObject *parent=NULL);
    PGEFile(QString _rawData);
    void setRawData(QString _rawData);
    bool buildTreeFromRaw();
    QString lastError();

    //data tree
    QList<PGEX_Entry > dataTree;

private:
    QString _lastError;
    QString rawData;
    QList<PGEXSct > rawDataTree;

    //Static functions
public:
    // /////////////Validators///////////////
    static bool IsSectionTitle(QString in);//Section Title

    static PGEX_Entry buildTree(QStringList &src_data, bool *_valid = 0);

    //returns FALSE on valid data
    static bool IsQStr(QString in);// QUOTED STRING
    static bool IsHex(QString in);// Hex Encoded String
    static bool IsIntU(QString in);// UNSIGNED INT
    static bool IsIntS(QString in);// SIGNED INT
    static bool IsFloat(QString in);// FLOAT
    static bool IsBool(QString in);//BOOL
    static bool IsBoolArray(QString in);//Boolean array
    static bool IsIntArray(QString in);//Integer array
    static bool IsStringArray(QString in);//String array

    //Split string into data values
    static QList<QStringList> splitDataLine(QString src_data, bool *valid = 0);

    //PGE Extended File parameter string generators
    static QString IntS(long input);
    static QString BoolS(bool input);
    static QString FloatS(double input);
    static QString qStrS(QString input);
    static QString hStrS(QString input);
    static QString strArrayS(QStringList input);
    static QString intArrayS(QList<int > input);
    static QString BoolArrayS(QList<bool > input);

    static QString X2STR(QString input);
    static QStringList X2STRArr(QString src);
    static QList<bool> X2BollArr(QString src);

    static QString escapeStr(QString input);
    static QString restoreStr(QString input);
    static QString encodeEscape(QString input);
    static QString decodeEscape(QString input);

    static QString value(QString marker, QString data);
    static QString removeQuotes(QString str);
};


#endif // PGE_X_H

