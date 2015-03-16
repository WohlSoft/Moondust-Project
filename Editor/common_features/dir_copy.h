#ifndef DIRCOPIER_H
#define DIRCOPIER_H

#include <QString>

class DirCopy
{
public:
    static void copy(const QString &sourceFolder, const QString &destFolder);
};

#endif // DIRCOPIER_H
