#pragma once
#ifndef EDITORSPASHSCREEN_H
#define EDITORSPASHSCREEN_H

#include <QObject>
#include <QSplashScreen>
#include <QTimer>
#include <QPair>
#include <QPoint>
#include <QPainter>
#include <QSharedPointer>

#include "simple_animator.h"

typedef QPair<QPoint, QSharedPointer<SimpleAnimator> > SplashPiece;

class EditorSpashScreen : public QSplashScreen
{
    Q_OBJECT

public:
    EditorSpashScreen();
    EditorSpashScreen(QPixmap &pixmap);
    virtual ~EditorSpashScreen();

    virtual void drawContents(QPainter *painter);
    void addAnimation(QPoint p, QPixmap &pixmap, int frames = 1, int speed = 64);
    void startAnimations();

private slots:
    void opacityUP();

public slots:
    void progressValue(int val);
    void progressMax(int val);
    void progressTitle(QString val);

    void progressPartsMax(int val);
    void progressPartsVal(int val);

protected:
    virtual void keyPressEvent(QKeyEvent *);
    virtual void closeEvent(QCloseEvent *e);

private:
    void construct();
    void rebuildLabel();

    QString m_labelString;
    double  m_partsMax;
    double  m_partsVal;
    double  m_labelVal;
    double  m_labelMax;
    int     m_percents;
    QString m_label;

    AnimationTimer m_animator;
    QVector<SplashPiece > m_animations;
    qreal m_opacity;
    qreal m_width_ratio;
    qreal m_height_ratio;
    QTimer m_scaler;
    QPixmap m_buffer;
    QPixmap m_tmpImage;
};

#endif // EDITORSPASHSCREEN_H
