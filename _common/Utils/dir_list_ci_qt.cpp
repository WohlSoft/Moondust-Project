#include "dir_list_ci_qt.h"

#include "../DirManager/dirman.h"
#include "strings.h"


DirListCIQt::DirListCIQt(const QString &curDir)
    : m_curDir(curDir)
{
    rescan();
}

void DirListCIQt::setCurDir(const QString &path)
{
    m_curDir = path;
    rescan();
}

static void replaceSlashes(QString &str, const QString &from)
{
    str.clear();
    if(from.isEmpty())
        return;

    str.reserve(from.size());

    QChar prevC = '\0';

    for(QChar c : from)
    {
        if(c == '\\')
            c = '/'; // Replace backslashes
        if(c == '/' && prevC == '/')
            continue; // skip duplicated slashes
        prevC = c;
        str.push_back(c);
    }
}

QString DirListCIQt::resolveFileCase(const QString &in_name)
{
#ifdef _WIN32
    QString name;
    replaceSlashes(name, in_name);
    return name;
#else
    if(in_name.isEmpty())
        return in_name;

    QString name;
    replaceSlashes(name, in_name);

    // For sub-directory path, look deeply
    auto subDir = name.indexOf('/');
    if(subDir >= 0)
    {
        auto sdName = resolveDirCase(name.mid(0, subDir));
        DirListCIQt sd(m_curDir + "/" + sdName);
        return sdName + "/" + sd.resolveFileCase(name.mid(subDir + 1));
    }

    // keep MixerX path arguments untouched
    auto pathArgs = name.indexOf('|');
    if(pathArgs >= 0)
    {
        auto n = name.mid(0, pathArgs);
        for(auto &c : m_fileList)
        {
            auto cs = QString::fromStdString(c);
            if(cs.compare(n, Qt::CaseInsensitive) == 0)
                return cs + name.mid(pathArgs);
        }
    }
    else
    for(auto &c : m_fileList)
    {
        auto cs = QString::fromStdString(c);
        if(cs.compare(name, Qt::CaseInsensitive) == 0)
            return cs;
    }

    return name;
#endif
}

QString DirListCIQt::resolveDirCase(const QString &name)
{
#ifdef _WIN32
    return name; // no need on Windows
#else
    if(name.isEmpty())
        return name;

    for(auto &c : m_dirList)
    {
        auto cs = QString::fromStdString(c);
        if(cs.compare(name, Qt::CaseInsensitive) == 0)
            return cs;
    }

    return name;
#endif
}

void DirListCIQt::rescan()
{
    m_fileList.clear();
    m_dirList.clear();
    if(m_curDir.isEmpty())
        return;

    DirMan d(m_curDir.toStdString());
    d.getListOfFiles(m_fileList);
    d.getListOfFolders(m_dirList);
}
