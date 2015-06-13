#include "AnimationScene.h"
#include "../main/globals.h"
#include "../main/mw.h"

AnimationScene::AnimationScene(QObject *parent) : QGraphicsScene(parent)
{
    //ImageFrame = new QGraphicsPixmapItem;

    x=0; y=0, dir=1;
    FullFrames = AniFrames;
    framesTable = framesX;
    mCurFrN = 0;
    mPos = QPoint(0,0);
    mSpriteImage = MW::p->x_imageSprite;
    NoAnimate = QPixmap(":/images/NoAni.png");
    draw();
    currentImage = NoAnimate;
    ImageFrame.setPixmap( currentImage );
    addItem(&ImageFrame);

    ImageFrame.setPos(100, 200);
    ImageFrame.setZValue(1.0);

    GRound.setRect(1,1,200,20);
    GRound.setBrush(QBrush(Qt::lightGray));
    GRound.setPen( QPen(QBrush(Qt::gray),2,Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin ) );
    GRound.setZValue(-5.0);
    addItem(&GRound);

    timer.setInterval(128);
    connect(
                    &timer, SIGNAL(timeout()),
                    this,
                    SLOT( nextFrame() ) );
}

void AnimationScene::setAnimation(QVector<AniFrame > frameS)
{
    timer.stop();
    currentFrameSet = frameS;
    mCurFrN = 0;
    setFrame(0);
    timer.start();
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
    int w,h,x,y;

    if((currentFrameSet.size()==0) || (frame >= currentFrameSet.size()))
    {
        ImageFrame.setPos(this->sceneRect().width()/2-50.0, this->sceneRect().height()/2-50.0);
        currentImage=NoAnimate;
        ImageFrame.setPixmap(currentImage);
        GRound.hide();
        return;
    }

    //following variable keeps track which
    //frame to show from sprite sheet
    mCurrentFrameX = 100 * currentFrameSet[frame].x;
        mPos.setX( mCurrentFrameX );

    mCurrentFrameY = 100 * currentFrameSet[frame].y;
        mPos.setY( mCurrentFrameY );

    w = frameWidth;
    h = framesTable[currentFrameSet[frame].x][currentFrameSet[frame].y].isDuck?frameHeightDuck:frameHeight;
    x = framesTable[currentFrameSet[frame].x][currentFrameSet[frame].y].offsetX;
    y = framesTable[currentFrameSet[frame].x][currentFrameSet[frame].y].offsetY;

    draw();

    ImageFrame.setPos(this->sceneRect().width()/2-w/2, this->sceneRect().height()/2 + (100.0-h));
    GRound.show();
    GRound.setPos(this->sceneRect().width()/2-GRound.rect().width()/2,
                  this->sceneRect().height()/2+100);

    ImageFrame.setOffset( -x, -y );
    ImageFrame.setPixmap(currentImage);
}

void AnimationScene::nextFrame()
{
    mCurFrN++;
    if(mCurFrN >= currentFrameSet.size())
        mCurFrN=0;
    setFrame(mCurFrN);
}
