/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#ifndef LEVELEDIT_H
#define LEVELEDIT_H

#include <QtGui>
#include <QGraphicsView>
#include <QGraphicsScene>
#include "lvl_filedata.h"
#include "lvlscene.h"

namespace Ui {
class leveledit;
}

class leveledit : public QWidget
{
    Q_OBJECT
    
public:
    explicit leveledit(QWidget *parent = 0);
    ~leveledit();

    LevelData LvlData;
    //QGraphicsScene LvlScene;

    void newFile();
    bool loadFile(const QString &fileName, LevelData FileData, dataconfigs &configs);
    bool save();
    bool saveAs();
    bool saveFile(const QString &fileName);
    QString userFriendlyCurrentFile();
    QString currentFile() { return curFile; }

    void setCurrentSection(int scId);

    void ExportToImage();

    LvlScene * scene;
    
protected:
    void closeEvent(QCloseEvent *event);


private:
    void documentWasModified();
    Ui::leveledit *ui;

    LevelData StartLvlData;
    void DrawObjects(QProgressDialog &progress, dataconfigs &configs);

    bool isModyfied;
    bool maybeSave();
    void setCurrentFile(const QString &fileName);
    void setDataBoxes();
    QString strippedName(const QString &fullFileName);
    QString curFile;
    QString latest_export;
    bool isUntitled;
    unsigned int FileType;

    //QGraphicsScene* pScene;
};

#endif // LEVELEDIT_H
