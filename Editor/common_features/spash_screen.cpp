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
    for(int i=0; i<animations.size(); i++)
    {
        QRect x;
        x.setX(animations[i].first.x() RatioWidth);
        x.setY(animations[i].first.y() RatioHeight);
        x.setWidth(animations[i].second.image().width() RatioWidth);
        x.setHeight(animations[i].second.image().height() RatioHeight);
        painter->drawPixmap(x, animations[i].second.image());
    }
    QPainterPath path;
    painter->setPen(Qt::white);
    path.addText(rect().x()+20, rect().bottom()-20, QFont("Times", 8, -1, true), _label);
    painter->strokePath(path, QPen(QColor(Qt::black), 4));
    painter->fillPath(path, QBrush(Qt::white));
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

void EditorSpashScreen::progressValue(int val)
{
    _label_val=val;
    rebuildLabel();
}

void EditorSpashScreen::progressMax(int val)
{
    _label_max=val;
    rebuildLabel();
}

void EditorSpashScreen::progressTitle(QString val)
{
    _label_str=val;
    rebuildLabel();
}

void EditorSpashScreen::construct()
{
    _label_val=0.0;
    _label_max=100.0;
    _percents=0;
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
    _percents = (int)round((_label_val/_label_max)*100.0);
    _label = QString("%1% - %2").arg(_percents).arg(_label_str);
}

