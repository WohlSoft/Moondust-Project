#ifndef LVLSCENE_H
#define LVLSCENE_H

#include <QGraphicsScene>
#include <QProgressDialog>
#include "lvl_filedata.h"
#include "dataconfigs.h"

/*
class Sprite
{
public:

    Sprite(QPixmap *image, int frames);
    void draw( QPainter* painter);
    QPoint pos() const;
    void nextFrame();
    int frm;

private:

    QPixmap* mSpriteImage;
    int mCurrentFrame;
    QPoint mPos;
    int mXDir;
};
*/

struct UserBGOs
{
    QPixmap image;
    QBitmap mask;
    unsigned long id;
};

class LvlScene : public QGraphicsScene
{
    Q_OBJECT
public:
    LvlScene(QObject *parent = 0);
    void setBlocks(LevelData FileData, QProgressDialog &progress);
    void setBGO(LevelData FileData, QProgressDialog &progress, dataconfigs &configs);
    void setNPC(LevelData FileData, QProgressDialog &progress);
    void makeSectionBG(int x, int y, int h, int w);

private:
    QGraphicsItem * itemCollidesWith(QGraphicsItem * item);
    void placeBox(float x, float y);
    void placeBlock(float x, float y, QPixmap &img);

};

#endif // LVLSCENE_H
