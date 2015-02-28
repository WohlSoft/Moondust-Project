#ifndef MWDOCK_BASE_H
#define MWDOCK_BASE_H

class MainWindow;
class QObject;

class MWDock_Base
{
public:
    explicit MWDock_Base(QObject *parent = 0);
    virtual ~MWDock_Base();
    MainWindow *mw();

//public slots:
//    virtual void re_translate();

//signals:

protected:
    virtual void construct(MainWindow *ParentMW);

private:
    void setParentMW(MainWindow *ParentMW);
    MainWindow *_mw;
};

#endif // MWDOCK_BASE_H
