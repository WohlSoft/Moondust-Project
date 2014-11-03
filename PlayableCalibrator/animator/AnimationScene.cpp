#include "AnimationScene.h"
#include "../main/globals.h"
#include "../main/mw.h"

AnimationScene::AnimationScene(QObject *parent) : QGraphicsScene(parent)
{
    ImageFrame = new QGraphicsPixmapItem;
    x=0; y=0, dir=1;
    FullFrames = AniFrames;
    framesTable = framesX;
    mCurFrN = 0;
    mPos = QPoint(0,0);
    mSpriteImage = MW::p->x_imageSprite;
    NoAnimate = QPixmap(":/images/NoAni.png");
    draw();
    currentImage=NoAnimate;
    ImageFrame->setPixmap(QPixmap(currentImage));
    addItem(ImageFrame);
    ImageFrame->setPos(100, 200);

    timer = new QTimer(this);
    connect(
                    timer, SIGNAL(timeout()),
                    this,
                    SLOT( nextFrame() ) );
}

void AnimationScene::setAnimation(QVector<AniFrame > frameS)
{
    timer->stop();
    currentFrameSet = frameS;
    mCurFrN = 0;
    setFrame(0);
    timer->start(128);
}



void AnimationScene::draw()
{
    currentImage =  mSpriteImage.copy(QRect(mPos.x(), mPos.y(), 100, 100 ));
            // mPos.x(),mPos.y(), *mSpriteImage, mCurrentFrame, 0, 100,100 );
}

QPoint AnimationScene::pos() const
{
    return mPos;
}

void AnimationScene::setFrame(int frame)
{
    int /*w,*/h,x,y, posX, posY;

    if((currentFrameSet.size()==0) || (frame >= currentFrameSet.size()))
    {
        currentImage=NoAnimate;
        ImageFrame->setPixmap(QPixmap(currentImage));
        return;
    }

    //following variable keeps track which
    //frame to show from sprite sheet
    mCurrentFrameX = 100 * currentFrameSet[frame].x;
        mPos.setX( mCurrentFrameX );

    mCurrentFrameY = 100 * currentFrameSet[frame].y;
        mPos.setY( mCurrentFrameY );


    //w = framesTable[currentFrameSet[frame].x][currentFrameSet[frame].y].W;
    h = framesTable[currentFrameSet[frame].x][currentFrameSet[frame].y].H;
    x = framesTable[currentFrameSet[frame].x][currentFrameSet[frame].y].offsetX;
    y = framesTable[currentFrameSet[frame].x][currentFrameSet[frame].y].offsetY;

    posX = 100 - x ;
    posY = 200 - h - y;

    draw();
    ImageFrame->setPos( posX, posY );
    ImageFrame->setPixmap(QPixmap(currentImage));
}

void AnimationScene::nextFrame()
{
    mCurFrN++;
    if(mCurFrN >= currentFrameSet.size())
        mCurFrN=0;
    setFrame(mCurFrN);
}
