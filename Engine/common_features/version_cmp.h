#ifndef VERSIONCMP_H
#define VERSIONCMP_H

#include <QString>
#include <string>

class VersionCmp
{
public:
    enum VersionSuffix
    {
        DEV     =0,
        ALPHA   =0,
        BETA    =1,
        RC      =2,
        RELEASE =3
    };

    static int str2ver(QString verSuffix);
    static QString compare(QString ver1, QString ver2);

    static int str2ver(std::string verSuffix);
    static std::string compare(std::string ver1, std::string ver2);
};

#endif // VERSIONCMP_H
