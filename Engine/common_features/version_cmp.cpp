#include <algorithm>
#include <unordered_map>
#include <Utils/strings.h>
#include "version_cmp.h"
#include <cstdlib>
#include <sstream>

static long str2ver(std::string verSuffix)
{
    static const std::unordered_map<std::string, VersionCmp::VersionSuffix> ver =
    {
        {"alpha", VersionCmp::ALPHA},
        {"dev", VersionCmp::DEV},
        {"beta", VersionCmp::BETA},
        {"rc", VersionCmp::RC},
        {"", VersionCmp::RELEASE}
    };

    std::transform(verSuffix.begin(), verSuffix.end(), verSuffix.begin(), ::tolower);
    std::unordered_map<std::string, VersionCmp::VersionSuffix>::const_iterator i = ver.find(verSuffix);
    if(i == ver.end())
        return 0;
    else
        return i->second;
}

static bool ver2arr(long *dest, std::string ver)
{
    std::string::size_type rev = ver.find('-');
    if(rev != std::string::npos)
    {
        dest[4] = str2ver(ver.substr(rev + 1));
        ver = ver.substr(0, rev);
    }
    std::string::size_type beg = 0;
    std::string::size_type end = 0;
    int iterator = 0;
    bool quit = false;
    do
    {
        if(iterator == 4)
            return false;
        beg = end;
        end = ver.find('.', beg);
        if(end == std::string::npos)
        {
            quit = true;
            end = ver.size();
        }

        if((end - beg) == 0)
        {
            return (iterator > 0);
        }

        try
        {
            std::string cell = ver.substr(beg, (end - beg));
            dest[iterator++] = std::atol(cell.c_str());
            end++;//skip dot
        }
        catch (...)
        {
            return false;
        }
    }
    while(!quit);

    return true;
}

std::string VersionCmp::compare(std::string ver1, std::string ver2)
{
    long vers1[5] = {0, 0, 0, 0, 0};
    long vers2[5] = {0, 0, 0, 0, 0};

    if(ver1.empty())
        return ver2;
    if(ver2.empty())
        return ver1;

    if(!ver2arr(vers1, ver1))
        return ver2;
    if(!ver2arr(vers2, ver2))
        return ver1;

    double mult  = 0.000001;
    double ver1i = 0.0;
    double ver2i = 0.0;

    for(int i = 4; i >= 0; i--)
    {
        ver1i += vers1[i] * mult;
        ver2i += vers2[i] * mult;
        mult *= 1000.0;
    }

    if(ver1i < ver2i)
        return ver2;
    else
        return ver1;
}
