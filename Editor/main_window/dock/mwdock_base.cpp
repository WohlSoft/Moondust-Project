#include "mwdock_base.h"
#include <mainwindow.h>

MWDock_Base::MWDock_Base(QObject *parent)//: QObject(parent)
{
    if(parent)
    {
        if(strcmp(parent->metaObject()->className(), "MainWindow")==0)
            _mw = qobject_cast<MainWindow *>(parent);
        else
            _mw = NULL;
    }
}

MWDock_Base::~MWDock_Base()
{}

MainWindow *MWDock_Base::mw()
{
    return _mw;
}

//void MWDock_Base::re_translate()
//{}

void MWDock_Base::construct(MainWindow *ParentMW)
{
    setParentMW(ParentMW);
}

void MWDock_Base::setParentMW(MainWindow *ParentMW)
{
    _mw = ParentMW;
}
