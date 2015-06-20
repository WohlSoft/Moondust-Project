#include <QStringList>
#include "version_cmp.h"

int VersionCmp::str2ver(QString verSuffix)
{
    if(verSuffix.toLower()=="alpha")
    {
        return ALPHA;
    }
    else
    if(verSuffix.toLower()=="dev")
    {
        return DEV;
    }
    else
    if(verSuffix.toLower()=="beta")
    {
        return BETA;
    }
    else
    if(verSuffix.toLower()=="rc")
    {
        return RC;
    }
    else
    if(verSuffix.toLower()=="")
    {
        return RELEASE;
    }
    else
        return 0;
}

QString VersionCmp::compare(QString ver1, QString ver2)
{
    long vers1[5] = {0,0,0,0,0};
    long vers2[5] = {0,0,0,0,0};

    QStringList ver1s = QString(ver1).replace('-', '.').split('.');
    QStringList ver2s = QString(ver2).replace('-', '.').split('.');
    bool ok=true;

    if(ver1s.isEmpty()) return ver1;
    if(ver2s.isEmpty()) return ver1;

    if(ver1.split('-').size()>1)
    {
        vers1[4] = str2ver(ver1s.last());
        ver1s.pop_back();
    }
    else
        vers1[4]=RELEASE;

    if(ver2.split('-').size()>1)
    {
        vers2[4] = str2ver(ver2s.last());
        ver2s.pop_back();
    }
    else
        vers2[4]=RELEASE;

    int c=0;
    while((!ver1s.isEmpty())&&(c<5))
    {
        vers1[c]=ver1s.first().toLong(&ok);
        if(!ok) return ver1;
        ver1s.pop_front();
        c++;
    }
    c=0;
    while((!ver2s.isEmpty())&&(c<5))
    {
        vers2[c]=ver2s.first().toLong(&ok);
        if(!ok) return ver1;
        ver2s.pop_front();
        c++;
    }

    qreal mult=0.000001;
    qreal ver1i=0.0;
    qreal ver2i=0.0;

    for(int i=4; i>=0;i--)
    {
        ver1i += vers1[i]*mult;
        ver2i += vers2[i]*mult;
        mult*=1000.0;
    }

    if(ver1i<ver2i)
        return ver2;
    else
        return ver1;
}
