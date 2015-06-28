#ifndef _PGE_FILE_LIB_GLOBS_H
#define _PGE_FILE_LIB_GLOBS_H

#ifdef PGE_FILES_QT
#include <QString>
#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QVector>
#include <QPair>
#include <QObject>
#if defined(PGE_ENGINE)||defined(PGE_EDITOR)
#include <QSize>
#endif
#define PGE_FILES_INHERED public QObject
#define PGESTRING QString
#define PGESTRINGisEmpty() isEmpty()
#define PGEChar QChar
#define PGESTRINGList QStringList
#define PGEVECTOR QVector
#define PGELIST QList
#define PGEPAIR QPair
#define PGEFILE QFile
#define PGE_SPLITSTR(dst, src, sep) dst=src.split(sep);
inline bool IsNULL(PGESTRING str) { return str.isNull(); }
inline int toInt(PGESTRING str){ return str.toInt(); }
inline float toFloat(PGESTRING str){ return str.toFloat(); }
inline double toDouble(PGESTRING str){ return str.toDouble(); }
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
#define PGE_FILES_INGERED
#define PGESTRING std::string
#define PGESTRINGisEmpty() empty()
#define PGEChar char
#define PGESTRINGList std::vector<std::string >
#define PGEVECTOR std::vector
#define PGELIST std::vector
#define PGEPAIR std::pair
#define PGEFILE std::fstream
namespace PGE_FileFormats_misc
{
    void split(std::vector<std::string>& dest, const std::string& str, const char* separator);
    void replaceAll(std::string& str, const std::string& from, const std::string& to);
    void RemoveSub(std::string& sInput, const std::string& sub);
}
#define PGE_SPLITSTR(dst, src, sep) dst.clear(); PGE_FileFormats_misc::split(dst, src, sep);
inline bool IsNULL(PGESTRING str) { return (str.empty()==0); }
inline int toInt(PGESTRING str){ return std::atoi(str.c_str()); }
inline float toFloat(PGESTRING str){ return std::atof(str.c_str()); }
inline double toDouble(PGESTRING str){ return std::atof(str.c_str()); }
template<typename T>
PGESTRING fromNum(T num) { std::ostringstream n; n<<num; return n.str(); }
#endif

#endif // _PGE_FILE_LIB_GLOBS_H

