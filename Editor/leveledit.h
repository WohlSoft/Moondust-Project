#ifndef LEVELEDIT_H
#define LEVELEDIT_H

#include <QtGui>
#include <QGraphicsView>
#include <QGraphicsScene>
#include "lvl_filedata.h"
#include "lvlscene.h"

namespace Ui {
class leveledit;
}

class leveledit : public QWidget
{
    Q_OBJECT
    
public:
    explicit leveledit(QWidget *parent = 0);
    ~leveledit();

    LevelData LvlData;
    //QGraphicsScene LvlScene;

    void newFile();
    bool loadFile(const QString &fileName, LevelData FileData, dataconfigs &configs);
    bool save();
    bool saveAs();
    bool saveFile(const QString &fileName);
    QString userFriendlyCurrentFile();
    QString currentFile() { return curFile; }

    void setCurrentSection(int scId);

    
protected:
    void closeEvent(QCloseEvent *event);


private:
    void documentWasModified();
    Ui::leveledit *ui;

    LevelData StartLvlData;
    void DrawObjects(QProgressDialog &progress, dataconfigs &configs);

    bool isModyfied;
    bool maybeSave();
    void setCurrentFile(const QString &fileName);
    void setDataBoxes();
    QString strippedName(const QString &fullFileName);
    QString curFile;
    bool isUntitled;
    unsigned int FileType;

    //QGraphicsScene* pScene;
    LvlScene * scene;

};

#endif // LEVELEDIT_H
