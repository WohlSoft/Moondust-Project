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

#ifndef WORLD_EDIT_H
#define WORLD_EDIT_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>

#include <data_configs/data_configs.h>
#include <main_window/global_settings.h>
#include <PGE_File_Formats/wld_filedata.h>
#include <editing/_scenes/world/wld_scene.h>

#define WORLD_EDIT_CLASS "WorldEdit"

namespace Ui {
class WorldEdit;
}

class WorldEdit : public QWidget
{
    Q_OBJECT

    friend class MainWindow;
public:
    explicit WorldEdit(QWidget *parent = 0);
    ~WorldEdit();

    WorldData WldData;
    //QGraphicsScene LvlScene;

    long currentMusic;

    bool newFile(dataconfigs &configs, LevelEditingSettings options);
    bool loadFile(const QString &fileName, WorldData FileData, dataconfigs &configs, LevelEditingSettings options);
    bool save(bool savOptionsDialog = false);
    bool saveAs(bool savOptionsDialog = false);
    bool saveFile(const QString &fileName, const bool addToRecent = true);
    QString userFriendlyCurrentFile();
    QString currentFile() { return curFile; }
    void makeCrashState();

    void setCurrentSection(int scId);
    void ResetPosition();
    void goTo(long x, long y, bool SwitchToSection=false, QPoint offset=QPoint(0,0), bool center=false);

    void ResetZoom();
    void zoomIn();
    void zoomOut();
    void setZoom(int percent);
    int  getZoom();

    QGraphicsView* getGraphicsView();

    enum CusrorMode
    {
        MODE_HandDrag=-1,
        MODE_Selecting=0,
        MODE_Erasing,
        MODE_PlaceItem,
        MODE_DrawSquares,
        MODE_Pasting,
        MODE_Resizing
    };

    void changeCursor(int mode);

    void ExportToImage_fn();

    WldScene * scene;

    bool sceneCreated;
    bool isUntitled;

    QString curFile;

    QTimer *updateTimer;
    void setAutoUpdateTimer(int ms);
    void stopAutoUpdateTimer();
signals:
    void forceReload();

protected:
    virtual void focusInEvent(QFocusEvent *event);
    void closeEvent(QCloseEvent *event);
    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent* e);

private slots:
    virtual void mouseReleaseEvent( QMouseEvent * event );
    virtual void leaveEvent(QEvent * leaveEvent);
    void updateScene();

    void ExportingReady();

private:
    void documentWasModified();

    WorldData StartWldData;
    bool DrawObjects(QProgressDialog &progress);

    bool maybeSave();
    void setCurrentFile(const QString &fileName);
    void setDataBoxes();
    QString strippedName(const QString &fullFileName);
    QString latest_export_path;
    unsigned int FileType;

    //QGraphicsScene* pScene;
    void moveH(int step);
    void moveV(int step);

    Ui::WorldEdit *ui;
};

#endif // WORLD_EDIT_H
