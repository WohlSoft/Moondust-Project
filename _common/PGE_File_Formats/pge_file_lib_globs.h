/*! \file pge_file_lib_globs.h
    \brief Contains internal settings and references for PGE File Library

    All defined here macroses are allows to build PGE File Library for both
    Qt and STL libraries set.
*/

#ifndef _PGE_FILE_LIB_GLOBS_H
#define _PGE_FILE_LIB_GLOBS_H

/*! \def PGE_FILES_QT
    \brief If this macro is defined, Qt version of PGE File Library will be built
*/

/*! \def PGE_ENGINE
    \brief If this macro is defined, this library builds as part of PGE Engine
*/

/*! \def PGE_EDITOR
    \brief If this macro is defined, this library builds as part of PGE Editor
*/

/*! \def PGEChar
    \brief A macro which equal to 'char' if PGE File Library built in the STL mode
           and equal to QChar if PGE File Library built in the Qt mode
*/

/*! \def PGESTRING
    \brief A macro which equal to std::string if PGE File Library built in the STL mode
           and equal to QString if PGE File Library built in the Qt mode
*/

/*! \def PGESTRINGList
    \brief A macro which equal to std::vector<std::string> if PGE File Library built in the STL mode
           and equal to QStringList if PGE File Library built in the Qt mode
*/

/*! \def PGELIST
    \brief A macro which equal to std::vector if PGE File Library built in the STL mode
           and equal to QList if PGE File Library built in the Qt mode
*/

/*! \def PGEVECTOR
    \brief A macro which equal to std::vector if PGE File Library built in the STL mode
           and equal to QVector if PGE File Library built in the Qt mode
*/


#ifdef PGE_FILES_QT
#include <QString>
#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QVector>
#include <QPair>
#include <QMap>
#include <QObject>
#include <tgmath.h>
#if defined(PGE_ENGINE)||defined(PGE_EDITOR)
#include <QSize>
#endif
#define PGE_FILES_INHERED public QObject
#define PGESTRING QString
#define PGESTRINGisEmpty() isEmpty()
#define PGESTR_Simpl(str) str.simplified()
#define PGEGetChar(chr) chr.toLatin1()
#define PGEChar QChar
#define PGESTRINGList QStringList
#define PGEVECTOR QVector
#define PGELIST QList
#define PGEPAIR QPair
#define PGEMAP QMap
#define PGEFILE QFile
#define PGE_SPLITSTR(dst, src, sep) dst=src.split(sep);
#define PGE_ReplSTR(src, from, to) src.replace(from, to)
#define PGE_RemSSTR(src, substr) src.remove(substr)
#define PGE_RemSRng(pos, len) remove(pos, len)
inline bool IsNULL(PGESTRING str) { return str.isNull(); }
inline int toInt(PGESTRING str){ return str.toInt(); }
inline float toFloat(PGESTRING str){ return str.toFloat(); }
inline double toDouble(PGESTRING str){ return str.toDouble(); }
inline PGESTRING removeSpaces(PGESTRING src) { return src.remove(' '); }
template<typename T>
PGESTRING fromNum(T num) { return QString::number(num); }
#else
#include <string>
#include <vector>
#include <utility>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <algorithm>
#include <map>
#define PGE_FILES_INGERED
#define PGESTRING std::string
#define PGESTRINGisEmpty() empty()
inline PGESTRING PGESTR_Simpl(PGESTRING str)
    { str.erase( std::remove_if( str.begin(), str.end(), ::isspace ), str.end() );
        return str;}
#define PGEGetChar(chr) chr
#define PGEChar char
#define PGESTRINGList std::vector<std::string >
#define PGEVECTOR std::vector
#define PGELIST std::vector
#define PGEPAIR std::pair
#define PGEMAP std::map
#define PGEFILE std::fstream
namespace PGE_FileFormats_misc
{
    void split(std::vector<std::string>& dest, const std::string& str, std::string separator);
    void replaceAll(std::string& str, const std::string& from, const std::string& to);
    void RemoveSub(std::string& sInput, const std::string& sub);
    bool hasEnding (std::string const &fullString, std::string const &ending);
}
#define PGE_SPLITSTR(dst, src, sep) dst.clear(); PGE_FileFormats_misc::split(dst, src, sep);
inline PGESTRING PGE_ReplSTR(PGESTRING src, PGESTRING from, PGESTRING to) {
    PGE_FileFormats_misc::replaceAll(src, from, to);
    return src;
}

inline PGESTRING PGE_RemSSTR(PGESTRING src, PGESTRING substr) { PGE_FileFormats_misc::RemoveSub(src, substr); return src; }
#define PGE_RemSRng(pos, len) erase(pos, len)
inline bool IsNULL(PGESTRING str) { return (str.empty()!=0); }
inline int toInt(PGESTRING str){ return std::atoi(str.c_str()); }
inline float toFloat(PGESTRING str){ return std::atof(str.c_str()); }
inline double toDouble(PGESTRING str){ return std::atof(str.c_str()); }
inline PGESTRING removeSpaces(PGESTRING src) { return PGE_RemSSTR(src, " "); }
template<typename T>
PGESTRING fromNum(T num) { std::ostringstream n; n<<num; return n.str(); }
#endif

/*!
 * Misc I/O classes used by PGE File Library internally
 */
namespace PGE_FileFormats_misc
{
    /*!
     * \brief Provides cross-platform file path calculation for a file names or paths
     */
    class FileInfo
    {
    public:
        /*!
         * \brief Constructor
         */
        FileInfo();
        /*!
         * \brief Constructor with pre-opening of a file
         * \param filepath relative or absolute file path
         */
        FileInfo(PGESTRING filepath);
        /*!
         * \brief Sets file which will be used to calculate file information
         * \param filepath
         */
        void setFile(PGESTRING filepath);
        /*!
         * \brief Returns file extension (last part of filename after last dot)
         * \return file suffix or name extension (last part of filename after last dot)
         */
        PGESTRING suffix();
        /*!
         * \brief Returns full filename without path
         * \return full filename without path
         */
        PGESTRING filename();
        /*!
         * \brief Returns absolute path to file
         * \return absolute path to file
         */
        PGESTRING fullPath();
        /*!
         * \brief Returns base name part (first part of file name before first dot)
         * \return base name part (first part of file name before first dot)
         */
        PGESTRING basename();
        /*!
         * \brief Returns full directory path where actual file is located
         * \return full directory path where actual file is located
         */
        PGESTRING dirpath();
    private:
        /*!
         * \brief Recalculates all internal fields
         */
        void rebuildData();
        /*!
         * \brief Current filename
         */
        PGESTRING filePath;
        /*!
         * \brief Current filename without directory path
         */
        PGESTRING _filename;
        /*!
         * \brief File name suffix (last part of file name after last dot)
         */
        PGESTRING _suffix;
        /*!
         * \brief Base name (first part of file name before first dot)
         */
        PGESTRING _basename;
        /*!
         * \brief Full directory path where file is located
         */
        PGESTRING _dirpath;
    };

    /*!
     * \brief Provides cross-platform text file reading interface
     */
    class TextFileInput
    {
    public:
        /*!
         * \brief Relative positions of carriage
         */
        enum positions{
            //! Relative to current position
            current=0,
            //! Relative to begin of file
            begin,
            //! Relative to end of file
            end
        };
        /*!
         * \brief Checks is requested file exist
         * \param filePath Full or relative path to the file
         * \return true if file exists
         */
        static bool exists(PGESTRING filePath);
        /*!
         * \brief Constructor
         */
        TextFileInput();
        /*!
         * \brief Constructor with pre-opening of the file
         * \param filePath Full or relative path to the file
         * \param utf8 Use UTF-8 encoding or will be used local 8-bin encoding
         */
        TextFileInput(PGESTRING filePath, bool utf8=false);
        /*!
         * \brief Destructor
         */
        ~TextFileInput();
        /*!
         * \brief Opening of the file
         * \param filePath Full or relative path to the file
         * \param utf8 Use UTF-8 encoding or will be used local 8-bin encoding
         */
        bool open(PGESTRING filePath, bool utf8=false);
        /*!
         * \brief Close currently opened file
         */
        void close();
        /*!
         * \brief Reads requested number of characters from a file
         * \param Maximal lenght of characters to read from file
         * \return string contains requested line of characters
         */
        PGESTRING read(long len);
        /*!
         * \brief Reads whole line before line feed character
         * \return string contains gotten line
         */
        PGESTRING readLine();
        /*!
         * \brief Reads all data from a file at current position of carriage
         * \return
         */
        PGESTRING readAll();
        /*!
         * \brief Is carriage position at end of file
         * \return true if carriage position at end of file
         */
        bool eof();
        /*!
         * \brief Returns current position of carriage relative to begin of file
         * \return current position of carriage relative to begin of file
         */
        long long tell();
        /*!
         * \brief Changes position of carriage to specific file position
         * \param pos Target position of carriage
         * \param relativeTo defines relativity of target position of carriage (current position, begin of file or end of file)
         */
        void seek(long long pos, positions relativeTo);
    private:
        #ifdef PGE_FILES_QT
        //! File handler used in Qt version of PGE file Library
        QFile file;
        //! File input stream used in Qt version of PGE file Library
        QTextStream stream;
        #else
        //! File input stream used in STL version of PGE file Library
        std::fstream stream;
        #endif
    };

}

#endif // _PGE_FILE_LIB_GLOBS_H

