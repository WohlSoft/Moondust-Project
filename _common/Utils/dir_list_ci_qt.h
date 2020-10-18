#ifndef DIRLISTCI_QT_H
#define DIRLISTCI_QT_H

#include <QString>
#include <QVector>

/**
 * @brief Case-Insensitive directory list
 */
class DirListCIQt
{
    QString m_curDir;
    std::vector<std::string> m_fileList;
    std::vector<std::string> m_dirList;
public:
    DirListCIQt(const QString &curDir = QString());
    void setCurDir(const QString &path);

    QString resolveFileCase(const QString &name);
    QString resolveDirCase(const QString &name);

    void rescan();
};

#endif // DIRLISTCI_QT_H
