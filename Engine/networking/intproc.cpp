#include "intproc.h"

EditorPipe * IntProc::editor=NULL;
bool IntProc::enabled=false;
QString IntProc::state="";

bool IntProc::cmd_accepted=false;
QString IntProc::cmd="";

IntProc::IntProc(QObject *parent) :
    QObject(parent)
{
    editor = NULL;
}


void IntProc::init()
{
    editor = new EditorPipe();
    editor->start();
    enabled=true;
}


void IntProc::quit()
{
    if(editor!=NULL)
    {
        editor->terminate();
        delete editor;
        editor = NULL;
        enabled=false;
    }

}

bool IntProc::isWorking()
{
    return (editor!=NULL);
}

QString IntProc::getCMD()
{
    cmd_accepted=false;
    QString tmp=cmd;
    cmd.clear();
    return tmp;
}

bool IntProc::isEnabled()
{
    return enabled;
}
