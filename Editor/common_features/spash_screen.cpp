#include "spash_screen.h"

EditorSpashScreen::EditorSpashScreen()
{
    opacity=1;
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
        x.setX(animations[i].first.x());
        x.setY(animations[i].first.y());
        x.setSize(animations[i].second.image().size());
        painter->drawPixmap(x, animations[i].second.image());
    }
}

void EditorSpashScreen::addAnimation(QPoint p, QPixmap &pixmap, int frames, int speed)
{
    if(pixmap.isNull()) return;

    SplashPiece ani;
    ani.first = p;
    ani.second.setSettings(pixmap, true, frames, speed);
    animations.push_back(ani);
}

void EditorSpashScreen::startAnimations()
{
    for(int i=0; i<animations.size(); i++)
    {
        animations[i].second.start();
    }

}

void EditorSpashScreen::opacityUP()
{
    if(opacity>=1.0)
    {
        opacity=1;
    }
    else
    {
        opacity+=0.025;
    }
    repaint();
}

void EditorSpashScreen::construct()
{
    buffer=this->pixmap();

    QPixmap t = QPixmap(buffer.width(), buffer.height());
    t.fill(Qt::transparent);
    this->setPixmap(t);

    opacity=0.0;
    scaler.setInterval(10);
    connect(&scaler, SIGNAL(timeout()), this, SLOT(opacityUP()));
    scaler.start(10);
}

