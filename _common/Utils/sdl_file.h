#ifndef SDL_FILE_H
#define SDL_FILE_H

#include <string>
#include <stdint.h>

struct SdlFile_private;

class SdlFile
{
    SdlFile_private *p = nullptr;
public:
    enum Mode
    {
        Read    = 0x1,
        Write   = 0x2,
        Append  = 0x4,
        Text    = 0x8
    };

    SdlFile();
    SdlFile(const std::string &fileName);
    SdlFile(const SdlFile &f);
    ~SdlFile();
    std::string errorString();
    std::string fileName();
    void setFileName(const std::string &fileName);
    bool open(int mode);
    bool exists();
    bool close();

    int64_t size();
    std::string readAll();
};

#endif // SDL_FILE_H
