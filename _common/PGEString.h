#ifdef PGE_EDITOR
#include <QString>
#include <QStringList>
#include <QList>
typedef QString     PGEString;
typedef QStringList PGEStringList;
#define PGEList     QList
#define StdToPGEString(str)     QString::fromStdString(str)
#define PGEStringToStd(str)     (str).toStdString()
#define PGEStringLit(str)       QStringLiteral(str)
#else
#include <string>
#include <vector>
typedef std::string                 PGEString;
typedef std::vector<std::string>    PGEStringList;
#define PGEList                     std::vector
#define StdToPGEString(str)         (str)
#define PGEStringToStd(str)         (str)
#define PGEStringLit(str)
#endif
