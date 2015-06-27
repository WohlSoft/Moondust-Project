#include "pge_file_lib_globs.h"

#ifndef PGE_FILES_QT
namespace PGE_FileFormats_misc
{
    void split(std::vector<std::string>& dest, const std::string& str, const char* separator)
    {
        char* pTempStr = strdup( str.c_str() );
        char* pWord = std::strtok(pTempStr, separator);
        while(pWord != NULL)
        {
            dest.push_back(pWord);
            pWord = std::strtok(NULL, separator);
        }
        free(pTempStr);
    }

    void replaceAll(std::string& str, const std::string& from, const std::string& to)
    {
        if(from.empty())
            return;
        size_t start_pos = 0;
        while((start_pos = str.find(from, start_pos)) != std::string::npos) {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
        }
    }

    void RemoveSub(std::string& sInput, const std::string& sub)
    {
        std::string::size_type foundpos = sInput.find(sub);
        if ( foundpos != std::string::npos )
            sInput.erase(sInput.begin() + foundpos, sInput.begin() + foundpos + sub.length());
    }
}
#endif
