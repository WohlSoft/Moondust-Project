#include <algorithm>
#include <unordered_map>
#include <Utils/strings.h>
#include "version_cmp.h"
#include <cstdlib>

int VersionCmp::str2ver(std::string verSuffix)
{
    static const std::unordered_map<std::string, VersionSuffix> ver =
    {
        {"alpha", ALPHA},
        {"dev", DEV},
        {"beta", BETA},
        {"rc", RC},
        {"", RELEASE}
    };

    std::transform(verSuffix.begin(), verSuffix.end(), verSuffix.begin(), ::tolower);
    std::unordered_map<std::string, VersionSuffix>::const_iterator i = ver.find(verSuffix);
    if(i == ver.end())
        return 0;
    else
        return i->second;
}

std::string VersionCmp::compare(std::string ver1, std::string ver2)
{
// FIXME: Rework this algorithm to be more accurate!

    long vers1[5] = {0,0,0,0,0};
    long vers2[5] = {0,0,0,0,0};
    std::string v1 = ver1, v2 = ver2;
    Strings::List ver1s, ver2s, v1s, v2s;
    std::replace(v1.begin(), v1.end(), '-', '.');
    std::replace(v2.begin(), v2.end(), '-', '.');
    Strings::split(ver1s, ver1, '.');
    Strings::split(ver2s, ver2, '.');

    if(ver1s.empty())
        return ver1;
    if(ver2s.empty())
        return ver1;

    Strings::split(v1s, ver1, '-');
    if(v1.size() > 1)
    {
        vers1[4] = str2ver(ver1s.back());
        ver1s.pop_back();
    }
    else
        vers1[4]=RELEASE;

    Strings::split(v2s, ver2, '-');
    if(v2s.size() > 1)
    {
        vers2[4] = str2ver(ver2s.back());
        ver2s.pop_back();
    }
    else
        vers2[4] = RELEASE;

    int c=0;
    while( (!ver1s.empty()) && (c<5) )
    {
        try {
            vers1[c] = std::atol(ver1s.front().c_str());
        }
        catch (...)
        {
            return ver1;
        }
        ver1s.erase(ver2s.begin());
        c++;
    }
    c = 0;
    while((!ver2s.empty()) && (c<5))
    {
        try {
            vers2[c] = std::atol(ver2s.front().c_str());
        }
        catch (...)
        {
            return ver1;
        }
        ver2s.erase(ver2s.begin());
        c++;
    }

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

