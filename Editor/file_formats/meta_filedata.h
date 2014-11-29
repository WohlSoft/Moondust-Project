#ifndef META_FILEDATA_H
#define META_FILEDATA_H

#include <QtCore>

enum ScriptUsage{
    SCRIPT_NOTHING = 0,
    SCRIPT_LUNADLL_AUTOCODE = 1 << 0,
    SCRIPT_LUNADLL_LUNALUA = 1 << 1,
    SCRIPT_LUNADLL_PGELUA = 1 << 2
};


struct Bookmark{
    QString bookmarkName;
    qreal x;
    qreal y;
};


struct MetaData{
    QVector<Bookmark> bookmarks;

    ScriptUsage usageOfScript;
};

#endif // META_FILEDATA_H
