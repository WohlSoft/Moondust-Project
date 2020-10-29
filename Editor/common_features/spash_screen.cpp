#include "spash_screen.h"
#include <QApplication>
#include <QImage>
#include <QPainterPath>
#include <QCloseEvent>
#include <Utils/maths.h>
#ifdef Q_OS_ANDROID
#   include <QPixmap>
#   include <QDesktopWidget>
#   define RatioWidth *width_ratio
#   define RatioHeight *height_ratio
#else
#   define RatioWidth
#   define RatioHeight
#endif

EditorSpashScreen::EditorSpashScreen()
{
    m_opacity = 1;
    m_width_ratio = 1.0;
    m_height_ratio = 1.0;
    construct();
}

EditorSpashScreen::EditorSpashScreen(QPixmap &pixmap)
{
    m_opacity = 1;
    QPixmap newx(pixmap.width(), pixmap.height() + 50);
    newx.fill(QColor(Qt::transparent));
    QPainter x(&newx);
    x.drawPixmap(0, 0, pixmap.width(), pixmap.height(), pixmap);
    x.end();
    setPixmap(newx);
    construct();
}

EditorSpashScreen::~EditorSpashScreen()
{}

void EditorSpashScreen::drawContents(QPainter *painter)
{
    bool rtl = (qApp->layoutDirection() == Qt::RightToLeft);
    painter->save();

    painter->setOpacity(1);
    painter->fillRect(rect(), QBrush(Qt::transparent));
    painter->setOpacity(m_opacity);
    painter->drawPixmap(rect(), m_buffer, m_buffer.rect());
    painter->setPen(Qt::black);
    painter->setBrush(Qt::white);
    painter->setRenderHint(QPainter::TextAntialiasing);
    painter->setRenderHint(QPainter::Antialiasing);
    for(int i = 0; i < m_animations.size(); i++)
    {
        QPixmap &frame = m_animations[i].second->wholeImage();
        QRect frameRect = m_animations[i].second->frameRect();
        QRect x;
        x.setX(m_animations[i].first.x() RatioWidth);
        x.setY(m_animations[i].first.y() RatioHeight);
        x.setWidth(frameRect.width() RatioWidth);
        x.setHeight(frameRect.height() RatioHeight);
        painter->drawPixmap(x, frame, frameRect);
    }

    if(rtl)
    {
        painter->save();
        QRect rectToDraw = rect();
        rectToDraw.setTop(rectToDraw.bottom() - 30);
        painter->setBrush(QBrush(Qt::black));
        painter->setPen(QPen(Qt::white, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter->drawRect(rectToDraw);
        painter->restore();
    }

    painter->setBrush(QBrush(Qt::black));
    QPen progressLine_bar(QBrush(Qt::black), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter->setPen(progressLine_bar);
    painter->drawRect(0, rect().height() - 5, rect().width(), 4);

    painter->setBrush(QBrush(Qt::green));
    painter->setPen(Qt::transparent);

    int progressLineLength = Maths::iRound(double(rect().width()) * ((double)m_percents / 100.0));
    painter->drawRect(rtl ? (rect().width() - progressLineLength) : 0,
                      rect().height() - 4,
                      progressLineLength,
                      2);

    painter->setPen(Qt::white);
    painter->setBrush(Qt::black);
    painter->setFont(QFont("Lucida Grande", 8, -1, true));

    QRect rectToDraw = rect();
    rectToDraw.setLeft(rectToDraw.left() + 20);
    rectToDraw.setRight(rectToDraw.right() - 20);
    rectToDraw.setTop(rectToDraw.bottom() - 30);

    if(!rtl)
    {
        QPainterPath path;
        path.addText(rect().x() + 20, rect().bottom() - 20, painter->font(), m_label);
        painter->strokePath(path, QPen(QColor(Qt::black), 4));
    }

    painter->drawText(rectToDraw, Qt::AlignTop | Qt::AlignLeft, m_label);

    painter->restore();
}

void EditorSpashScreen::addAnimation(QPoint p, QPixmap &pixmap, int frames, int speed)
{
    if(pixmap.isNull())
        return;

    SplashPiece ani;
    m_animations.push_back(ani);
    SplashPiece &aniP = m_animations.last();
    aniP.first = p;
    aniP.second = QSharedPointer<SimpleAnimator>(new SimpleAnimator(pixmap, true, frames, speed));
    m_animator.registerAnimation(aniP.second.data());
}

void EditorSpashScreen::startAnimations()
{
    m_animator.start(32);
    m_scaler.start();
}

void EditorSpashScreen::opacityUP()
{
    if(m_opacity >= 1.0)
        m_opacity = 1;
    else
        m_opacity += 0.09;
    repaint();
}

void EditorSpashScreen::progressValue(int val)
{
    m_labelVal = val;
    rebuildLabel();
}

void EditorSpashScreen::progressMax(int val)
{
    m_labelMax = val;
}

void EditorSpashScreen::progressTitle(QString val)
{
    m_labelString = val;
    rebuildLabel();
}

void EditorSpashScreen::progressPartsMax(int val)
{
    m_partsMax = val;
    rebuildLabel();
}

void EditorSpashScreen::progressPartsVal(int val)
{
    m_partsVal = val;
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
    m_labelVal = 0.0;
    m_labelMax = 100.0;
    m_percents = 0;

    m_partsMax = 1.0;
    m_partsVal = 0.0;

    m_buffer = this->pixmap();

    QPixmap t = QPixmap(m_buffer.width(), m_buffer.height());
    t.fill(Qt::transparent);
    this->setPixmap(t);

    m_opacity = 0.0;
    m_scaler.setTimerType(Qt::PreciseTimer);
    m_scaler.setInterval(64);
    QObject::connect(&m_scaler, SIGNAL(timeout()), this, SLOT(opacityUP()));
}

void EditorSpashScreen::rebuildLabel()
{
    double onePice = ((m_partsVal < m_partsMax) ? (m_labelVal / m_labelMax) : 0.0);
    m_percents = (int)round(((m_partsVal + onePice) / m_partsMax) * 100.0);
    m_label = QString("%1% - %2").arg(m_percents).arg(m_labelString);
}

