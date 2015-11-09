#include "pge_file_lib_globs.h"
#ifdef _WIN32
#include "windows.h"
#endif
#ifndef PGE_FILES_QT
#include <limits.h> /* PATH_MAX */
#else
#include <QFileInfo>
#endif

namespace PGE_FileFormats_misc
{
#ifndef PGE_FILES_QT
    void split(std::vector<std::string>& dest, const std::string& str, std::string separator)
    {
        #ifdef _MSC_VER
        char* pTempStr = _strdup( str.c_str() );//Microsoft Visual Studio on Windows
        #else
        char* pTempStr = strdup( str.c_str() );//GCC, CLang on Linux and on Mac OS X, or MinGW on Windows
        #endif
        char* pWord = std::strtok(pTempStr, separator.c_str());
        while(pWord != NULL)
        {
            dest.push_back(pWord);
            pWord = std::strtok(NULL, separator.c_str());
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

    bool hasEnding (std::string const &fullString, std::string const &ending) {
        if (fullString.length() >= ending.length()) {
            return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
        } else {
            return false;
        }
    }
#endif

    bool TextFileInput::exists(PGESTRING filePath)
    {
        #ifdef PGE_FILES_QT
        return QFile::exists(filePath);
        #else
        FILE *x=fopen(filePath.c_str(), "rb");
        if(x)
        {
            fclose(x); return true;
        }
        return false;
        #endif
    }

    TextFileInput::TextFileInput()
    {}

    TextFileInput::TextFileInput(PGESTRING filePath, bool utf8)
    {
        open(filePath, utf8);
    }

    TextFileInput::~TextFileInput()
    {
        #ifdef PGE_FILES_QT
        file.close();
        #else
        if(stream)
            stream.close();
        #endif
    }

    bool TextFileInput::open(PGESTRING filePath, bool utf8)
    {
        #ifdef PGE_FILES_QT
        bool state=false;
        file.setFileName(filePath);
        state=file.open(QIODevice::ReadOnly|QIODevice::Text);
        if(!state) return false;
        stream.setDevice(&file);
        if(utf8) {
            stream.setCodec("UTF-8");
        } else {
            stream.setAutoDetectUnicode(true);
            stream.setLocale(QLocale::system());
            stream.setCodec(QTextCodec::codecForLocale());
        }
        return true;
        #else
        (void)utf8;
        stream.open(filePath, std::ios::in);
        return (bool)stream;
        #endif
    }

    void TextFileInput::close()
    {
        #ifdef PGE_FILES_QT
        file.close();
        #else
        stream.close();
        #endif
    }

    PGESTRING TextFileInput::read(long len)
    {
        #ifdef PGE_FILES_QT
        if(!file.isOpen()) return "";
        return stream.read(len);
        #else
        if(!stream) return "";
        std::string buf(len + 1, '\0');
        stream.read(&buf[0], len);
        return buf;
        #endif
    }

    PGESTRING TextFileInput::readLine()
    {
        #ifdef PGE_FILES_QT
        if(!file.isOpen()) return "";
        return stream.readLine();
        #else
        if(!stream) return "";
        std::string out;
        std::getline(stream, out);
        if(out.size()==0) return "";
        if(out[out.size()-1]=='\r')
            out.erase(out.size()-1, 1);
        return out;
        #endif
    }

    PGESTRING TextFileInput::readAll()
    {
        #ifdef PGE_FILES_QT
        return stream.readAll();
        #else
        if(!stream) return "";
        std::string out;
        stream.seekg(0, std::ios::beg);
        char x;
        while(!stream.eof())
        {
            x=stream.get();
            if(x==-1) break;
            if(x!='\r') out.push_back(x);
        }
        return out;
        #endif
    }

    bool TextFileInput::eof()
    {
        #ifdef PGE_FILES_QT
        return stream.atEnd();
        #else
        return stream.eof();
        #endif
    }

    long long TextFileInput::tell()
    {
        #ifdef PGE_FILES_QT
        return stream.pos();
        #else
        return stream.tellg();
        #endif
    }

    void TextFileInput::seek(long long pos, TextFileInput::positions relativeTo)
    {
        #ifdef PGE_FILES_QT
        (void)relativeTo;
        stream.seek(pos);
        #else
        std::ios_base::seekdir s;
        switch(relativeTo)
        {
            case current: s=std::ios_base::cur; break;
            case begin: s=std::ios_base::beg; break;
            case end: s=std::ios_base::end; break;
            default: s=std::ios_base::beg; break;
        }
        stream.seekg(pos, s);
#endif
    }

    FileInfo::FileInfo()
    {}

    FileInfo::FileInfo(PGESTRING filepath)
    {
        setFile(filepath);
    }

    void FileInfo::setFile(PGESTRING filepath)
    {
        filePath=filepath;
        rebuildData();
    }

    PGESTRING FileInfo::suffix()
    {
        return _suffix;
    }

    PGESTRING FileInfo::filename()
    {
        return _filename;
    }

    PGESTRING FileInfo::fullPath()
    {
        return filePath;
    }

    PGESTRING FileInfo::basename()
    {
        return _basename;
    }

    PGESTRING FileInfo::dirpath()
    {
        return _dirpath;
    }

    void FileInfo::rebuildData()
    {
        #ifdef _MSC_VER
        #define PATH_MAXLEN MAX_PATH
        #else
        #define PATH_MAXLEN PATH_MAX
        #endif
        int i;
        _suffix.clear();
        _filename.clear();
        _dirpath.clear();
        _basename.clear();

        //Take full path
        #ifdef PGE_FILES_QT
        filePath=QFileInfo(filePath).absoluteFilePath();
        #else
        char buf[PATH_MAXLEN + 1];
        char *rez=NULL;
        #ifndef _WIN32
        rez=realpath(filePath.c_str(), buf);
        if(rez) filePath=buf;
        #else
        int ret=0;
        #ifdef UNICODE
        ret=GetFullPathNameA(filePath.c_str(), PATH_MAXLEN, buf, &rez);
        #else
        ret=GetFullPathName(filePath.c_str(), PATH_MAXLEN, buf, &rez);
        #endif
        if(ret!=0) filePath=buf;
        replaceAll(filePath, "\\", "/");
        #endif
        #endif

        //Read directory path
        i=filePath.size()-1;
        for(; i>=0; i--)
        {
            if((filePath[i]=='/')||(filePath[i]=='\\')) break;
        }
        if(i>=0)
        {
           #ifndef PGE_FILES_QT
           _dirpath=filePath.substr(0, i);
           #else
           _dirpath=filePath.left(i);
           #endif
        }

        //Take file suffix
        i=filePath.size()-1;
        for(; i>0; i--)
        {
            if(filePath[i]=='.') break;
        } if(i>0) i++;
        if(i==((signed)filePath.size()-1)) goto skipSuffix;
        for(;i<(signed)filePath.size();i++)
            _suffix.push_back(tolower(PGEGetChar(filePath[i])));
      skipSuffix:

        //Take file name without path
        i=filePath.size()-1;
        for(; i>=0; i--)
        {
            if((filePath[i]=='/')||(filePath[i]=='\\')) break;
        } if(i>0) i++;
        if(i>=((signed)filePath.size()-1)) goto skipFilename;
        for(;i<(signed)filePath.size();i++)
            _filename.push_back(filePath[i]);
      skipFilename:

       //Take base file name
        for(i=0;i<(signed)_filename.size();i++)
        {
            if(_filename[i]=='.') break;
            _basename.push_back(_filename[i]);
        }

    }
}

