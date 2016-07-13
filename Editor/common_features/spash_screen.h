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
    void addAnimation(QPoint p, QPixmap &pixmap, int frames=1, int speed=64);
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
    QString _label_str;
    double  _parts_max;
    double  _parts_val;
    double  _label_val;
    double  _label_max;
    int     _percents;
    QString _label;

    AnimationTimer animator;
    QVector<SplashPiece > animations;
    qreal opacity;
    qreal width_ratio;
    qreal height_ratio;
    QTimer scaler;
    QPixmap buffer;
    QPixmap tmp;
};

#endif // EDITORSPASHSCREEN_H
