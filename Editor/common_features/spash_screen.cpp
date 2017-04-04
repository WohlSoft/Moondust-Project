#include "spash_screen.h"
#include <QApplication>
#include <QImage>
#include <QCloseEvent>
#include <Utils/maths.h>
#ifdef Q_OS_ANDROID
#include <QPixmap>
#include <QDesktopWidget>
#define RatioWidth *width_ratio
#define RatioHeight *height_ratio
#else
#define RatioWidth
#define RatioHeight
#endif

EditorSpashScreen::EditorSpashScreen()
{
    opacity=1;
    width_ratio=1.0;
    height_ratio=1.0;
    construct();
}

EditorSpashScreen::EditorSpashScreen(QPixmap &pixmap)
{
    opacity=1;
    QPixmap newx(pixmap.width(), pixmap.height()+50);
    newx.fill(QColor(Qt::transparent));
    QPainter x(&newx);
    x.drawPixmap(0,0, pixmap.width(), pixmap.height(), pixmap);
    x.end();
    setPixmap(newx);
    construct();
}

EditorSpashScreen::~EditorSpashScreen()
{}

void EditorSpashScreen::drawContents(QPainter *painter)
{
    painter->setOpacity(1);
    painter->fillRect(rect(), QBrush(Qt::transparent));
    painter->setOpacity(opacity);
    painter->drawPixmap(rect(), buffer, buffer.rect());
    painter->setPen(Qt::black);
    painter->setBrush(Qt::white);
    painter->setRenderHint(QPainter::TextAntialiasing);
    painter->setRenderHint(QPainter::Antialiasing);
    for(int i=0; i<animations.size(); i++)
    {
        QPixmap &frame = animations[i].second->wholeImage();
        QRect frameRect = animations[i].second->frameRect();
        QRect x;
        x.setX(animations[i].first.x() RatioWidth);
        x.setY(animations[i].first.y() RatioHeight);
        x.setWidth(frameRect.width() RatioWidth);
        x.setHeight(frameRect.height() RatioHeight);
        painter->drawPixmap(x, frame, frameRect);
    }

    painter->setBrush(QBrush(Qt::black));
    QPen progressLine_bar( QBrush(Qt::black), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter->setPen(progressLine_bar);
    painter->drawRect(0, rect().height()-5, rect().width(), 4);

    painter->setBrush(QBrush(Qt::green));
    painter->setPen(Qt::transparent);
    painter->drawRect(0, rect().height()-4,
                        Maths::iRound(double(rect().width()) *( (double)_percents/100.0)), 2);

    painter->setPen(Qt::white);
    painter->setFont(QFont("Lucida Grande", 8, -1, true));

    QPainterPath path;
    path.addText(rect().x()+20, rect().bottom()-20, painter->font(), _label);
    painter->strokePath(path, QPen(QColor(Qt::black), 4));

    painter->drawText(rect().x()+20, rect().bottom()-20, _label);
}

void EditorSpashScreen::addAnimation(QPoint p, QPixmap &pixmap, int frames, int speed)
{
    if(pixmap.isNull()) return;

    SplashPiece ani;
    animations.push_back(ani);
    SplashPiece &aniP = animations.last();
    aniP.first = p;
    aniP.second = QSharedPointer<SimpleAnimator>(new SimpleAnimator(pixmap, true, frames, speed));
    animator.registerAnimation(aniP.second.data());
}

void EditorSpashScreen::startAnimations()
{
    animator.start(32);
    scaler.start();
}

void EditorSpashScreen::opacityUP()
{
    if(opacity>=1.0)
    {
        opacity=1;
    }
    else
    {
        opacity+=0.09;
    }
    repaint();
}

void EditorSpashScreen::progressValue(int val)
{
    _label_val=val;
    rebuildLabel();
}

void EditorSpashScreen::progressMax(int val)
{
    _label_max=val;
}

void EditorSpashScreen::progressTitle(QString val)
{
    _label_str=val;
    rebuildLabel();
}

void EditorSpashScreen::progressPartsMax(int val)
{
    _parts_max=val;
    rebuildLabel();
}

void EditorSpashScreen::progressPartsVal(int val)
{
    _parts_val=val;
}

void EditorSpashScreen::keyPressEvent(QKeyEvent *)
{
    //Just ignore everything
}

void EditorSpashScreen::closeEvent(QCloseEvent *e)
{
    //No way to close this dialog box!
    e->ignore();
}

void EditorSpashScreen::construct()
{
    _label_val=0.0;
    _label_max=100.0;
    _percents=0;

    _parts_max=1.0;
    _parts_val=0.0;

    buffer=this->pixmap();

    #ifdef Q_OS_ANDROID
    QDesktopWidget* desktopWidget = qApp->desktop();
    QRect screenGeometry = desktopWidget->screenGeometry();
    int screenWidth = screenGeometry.width();
    int screenHeight = screenGeometry.height();
    qreal oldHeight = buffer.height();
    qreal oldWidth = buffer.width();
    buffer = buffer.scaled(screenWidth, screenHeight, Qt::KeepAspectRatio);
    height_ratio = qreal(buffer.height()) / oldHeight;
    width_ratio =  qreal(buffer.width()) / oldWidth;
    #endif

    QPixmap t = QPixmap(buffer.width(), buffer.height());
    t.fill(Qt::transparent);
    this->setPixmap(t);

    opacity=0.0;
    scaler.setTimerType(Qt::PreciseTimer);
    scaler.setInterval(64);
    connect(&scaler, SIGNAL(timeout()), this, SLOT(opacityUP()));
}

void EditorSpashScreen::rebuildLabel()
{
    double onePice= ((_parts_val<_parts_max) ? (_label_val/_label_max) : 0.0);
    _percents = (int)round( ( (_parts_val+onePice)/_parts_max )*100.0 );
    _label = QString("%1% - %2").arg(_percents).arg(_label_str);
}

