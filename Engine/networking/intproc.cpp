#include "intproc.h"

EditorPipe * IntProc::editor=NULL;
bool IntProc::enabled=false;

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
    if(editor)
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

bool IntProc::isEnabled()
{
    return enabled;
}
