#ifndef KEYDROPPER_H
#define KEYDROPPER_H

#include <QObject>
#include <QDockWidget>

class KeyDropper : public QDockWidget
{
    Q_OBJECT
public:
    explicit KeyDropper(QWidget* parent = 0);
    ~KeyDropper() {}
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
};

#endif // KEYDROPPER_H
