#include "intproc.h"

EditorPipe * IntProc::editor=NULL;
bool IntProc::enabled=false;
QString IntProc::state="";

bool IntProc::cmd_accepted=false;
IntProc::ExternalCommands IntProc::command=IntProc::MessageBox;

QString IntProc::cmd="";

IntProc::IntProc(QObject *parent) :
    QObject(parent)
{
    editor = NULL;
}


void IntProc::init()
{
    qDebug()<<"IntProc constructing...";
    editor = new EditorPipe();
    qDebug()<<"IntProc starting...";
    editor->start();
    qDebug()<<"IntProc started!";
    enabled=true;
}


void IntProc::quit()
{
    if(editor!=NULL)
    {
        editor->isWorking=false;
        if(!editor->wait(4000))
        {
            editor->terminate();
            editor->wait();
        }
        delete editor;
        editor = NULL;
        enabled=false;
    }
}

bool IntProc::isWorking()
{
    return (editor!=NULL);
}

bool IntProc::hasCommand()
{
    return cmd_accepted;
}

IntProc::ExternalCommands IntProc::commandType()
{
    return command;
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
