/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2021 Vitaly Novichkov <admin@wohlnet.ru>
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
#ifndef LEVELEDIT_H
#define LEVELEDIT_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QAtomicInteger>

#include <data_configs/data_configs.h>
#include <PGE_File_Formats/lvl_filedata.h>
#include <editing/_scenes/level/lvl_scene.h>

#include "../edit_base.h"

#define LEVEL_EDIT_CLASS "LevelEdit"

namespace Ui
{
    class LevelEdit;
}

class DataConfig;

class LevelEdit : public EditBase
{
    Q_OBJECT
    friend class LvlScene;
    friend class MainWindow;
public:
    explicit LevelEdit(MainWindow *mw, QWidget *parent = nullptr);
    ~LevelEdit();

public slots:
    void reTranslate();

public:
    LevelData LvlData;
    DataConfig *m_configPack = nullptr;
    //QGraphicsScene LvlScene;

    void prepareLevelFile(LevelData &data);

    bool newFile(DataConfig &configs, EditingSettings options);
    bool loadFile(const QString &fileName, LevelData &FileData, DataConfig &configs, EditingSettings options);
    void showCustomStuffWarnings();
    bool save(bool savOptionsDialog = false);
    bool saveAs(bool savOptionsDialog = false);
    bool saveFile(const QString &fileName, const bool addToRecent = true, bool *out_WarningIsAborted = nullptr);
    bool savePGEXLVL(QString fileName, bool silent = false); //!< Saves a PGE Extended Level file format
    bool saveSMBX64LVL(QString fileName, bool silent = false, bool *out_WarningIsAborted = nullptr); //!< Saves a SMBX Level file format
    bool saveSMBX38aLVL(QString fileName, bool silent = false); //!< Saves SMBX38A Level file format
    QString userFriendlyCurrentFile();
    QString currentFile()
    {
        return curFile;
    }

    bool trySave();

    bool isUntitled();
    bool isModified();

    void markForForceClose();

    int  findNearestSection(long x, long y);

    void setCurrentSection(int scId);
    int  getCurrentSection();

    void resetPosition();
    void goTo(long x, long y, bool SwitchToSection = false, QPoint offset = QPoint(0, 0), bool center = false);

    void zoomReset();
    void zoomIn();
    void zoomOut();

    void setZoom(int percent);
    int  getZoom();

    QGraphicsView *getGraphicsView();

    enum CusrorMode
    {
        MODE_HandDrag = -1,
        MODE_Selecting = 0,
        MODE_Erasing,
        MODE_PlaceItem,
        MODE_DrawSquares,
        MODE_Pasting,
        MODE_Resizing
    };

    void changeCursor(int mode);

    void ExportToImage_fn();
    void ExportToImage_fn_piece();

    LvlScene *scene = nullptr;

    QAtomicInteger<bool> sceneCreated;
    bool m_isUntitled = false;

    QString curFile;

    QTimer *updateTimer = nullptr;
    void setAutoUpdateTimer(int ms);
    void stopAutoUpdateTimer();

signals:
    void forceReload();


protected:
    void closeEvent(QCloseEvent *event);

public slots:
    void doSave();

private slots:
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void leaveEvent(QEvent *leaveEvent);
    virtual void focusInEvent(QFocusEvent *event);
    virtual void dragEnterEvent(QDragEnterEvent *e);
    virtual void dropEvent(QDropEvent *e);

    void updateScene();

    void ExportingReady();

private:
    void documentWasModified();
    void updateTitle();
    Ui::LevelEdit *ui = nullptr;

    LevelData StartLvlData;
    bool DrawObjects(QProgressDialog &progress);

    bool maybeSave();
    void setCurrentFile(const QString &fileName);
    void setDataBoxes();
    QString strippedName(const QString &fullFileName);
    QString m_recentExportPath;
    unsigned int m_fileType = 0;
};

#endif // LEVELEDIT_H
