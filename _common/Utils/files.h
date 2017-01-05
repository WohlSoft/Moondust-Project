#ifndef FILES_H
#define FILES_H

#include <string>

namespace Files
{
    bool fileExists(const std::string &path);
    bool deleteFile(const std::string &path);
    std::string basename(std::string path);
    std::string dirname(std::string path);
    std::string changeSuffix(std::string path, const std::string& suffix);
    bool hasSuffix(const std::string &path, const std::string &suffix);
}

#endif // FILES_H
