#include "spash_screen.h"
#include <QApplication>
#include <QImage>
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
    setPixmap(pixmap);
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
    for(int i=0; i<animations.size(); i++)
    {
        QRect x;
        x.setX(animations[i].first.x() RatioWidth);
        x.setY(animations[i].first.y() RatioHeight);
        x.setWidth(animations[i].second.image().width() RatioWidth);
        x.setHeight(animations[i].second.image().height() RatioHeight);
        painter->drawPixmap(x, animations[i].second.image());
    }
}

void EditorSpashScreen::addAnimation(QPoint p, QPixmap &pixmap, int frames, int speed)
{
    if(pixmap.isNull()) return;

    SplashPiece ani;
    ani.first = p;
//    #ifdef Q_OS_ANDROID
//    ani.first.setX(p.x());
//    ani.first.setY(p.y()*height_ratio);
//    QSize oldSize = pixmap.size();
//    QPixmap newImg= pixmap.scaled(oldSize.width()*width_ratio, oldSize.height()*height_ratio, Qt::KeepAspectRatio);
//    pixmap=newImg;
//    #endif
    ani.second.setSettings(pixmap, true, frames, speed);
    animations.push_back(ani);
}

void EditorSpashScreen::startAnimations()
{
    for(int i=0; i<animations.size(); i++)
    {
        animations[i].second.start();
    }
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

void EditorSpashScreen::construct()
{
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

