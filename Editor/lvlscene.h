#ifndef LVLSCENE_H
#define LVLSCENE_H

#include <QGraphicsScene>
#include <QProgressDialog>
#include "lvl_filedata.h"

class LvlScene : public QGraphicsScene
{
    Q_OBJECT
public:
    LvlScene(QObject *parent = 0);
    void setBlocks(LevelData FileData, QProgressDialog &progress);
    void setBGO(LevelData FileData, QProgressDialog &progress);
    void setNPC(LevelData FileData, QProgressDialog &progress);
    void makeSectionBG(int x, int y, int h, int w);

private:
    QGraphicsItem * itemCollidesWith(QGraphicsItem * item);
    void placeBox(float x, float y);
    void placeBlock(float x, float y, QPixmap &img);

};

#endif // LVLSCENE_H
