#ifndef MDICHILD_H
#define MDICHILD_H

#include <QTextEdit>
#include <QWidget>


//////////////////////////////////////////////////////////////////////////////////////////
class MdiChild : public QTextEdit
{
    Q_OBJECT

public:
    MdiChild();

    void newFile(int FileType);
    bool loadFile(const QString &fileName);
    bool save();
    bool saveAs();
    bool saveFile(const QString &fileName);
    QString userFriendlyCurrentFile();
    QString currentFile() { return curFile; }

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void documentWasModified();

private:
    bool maybeSave();
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);

    QString curFile;
    bool isUntitled;
    unsigned int FileType;
};



//////////////////////////////////////////////////////////////////////////////////////////

#endif
