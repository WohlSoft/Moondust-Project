#ifndef EDITORSPASHSCREEN_H
#define EDITORSPASHSCREEN_H

#include <QObject>
#include <QSplashScreen>
#include <QTimer>
#include <QPair>
#include <QPoint>
#include <QPainter>

#include "simple_animator.h"

typedef QPair<QPoint, SimpleAnimator > SplashPiece;

class EditorSpashScreen : public QSplashScreen
{
    Q_OBJECT

public:
    EditorSpashScreen();
    EditorSpashScreen(QPixmap &pixmap);
    virtual ~EditorSpashScreen();
    virtual void drawContents(QPainter *painter);
    void addAnimation(QPoint p, QPixmap &pixmap, int frames=1, int speed=64);
    void startAnimations();
private slots:
    void opacityUP();

private:
    void construct();
    QVector<SplashPiece > animations;
    qreal opacity;
    qreal width_ratio;
    qreal height_ratio;
    QTimer scaler;
    QPixmap buffer;
    QPixmap tmp;
};

#endif // EDITORSPASHSCREEN_H
