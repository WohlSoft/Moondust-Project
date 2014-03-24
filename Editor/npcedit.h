#ifndef NPCEDIT_H
#define NPCEDIT_H

#include <QWidget>
#include <QFile>
#include "npc_filedata.h"

namespace Ui {
class npcedit;
}

class npcedit : public QWidget
{
    Q_OBJECT
    
public:
    explicit npcedit(QWidget *parent = 0);
    ~npcedit();

    void newFile();
    bool loadFile(const QString &fileName, NPCConfigFile FileData);
    NPCConfigFile NpcData;

    bool save();
    bool saveAs();
    bool saveFile(const QString &fileName);
    QString userFriendlyCurrentFile();
    QString currentFile() { return curFile; }

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void documentWasModified();

    void on_en_GFXOffsetX_clicked();

    void on_en_GFXOffsetY_clicked();

    void on_En_GFXw_clicked();

    void on_En_GFXh_clicked();

    void on_En_Frames_clicked();

    void on_En_Framespeed_clicked();

    void on_En_Framestyle_clicked();

    void on_En_IsForeground_clicked();

    void on_En_GrabSide_clicked();

    void on_En_GrabTop_clicked();

    void on_En_JumpHurt_clicked();

    void on_En_DontHurt_clicked();

    void on_En_Score_clicked();

    void on_En_NoEat_clicked();

    void on_En_NoFireball_clicked();

    void on_En_NoIceball_clicked();

    void on_En_Width_clicked();

    void on_En_Height_clicked();

    void on_En_Speed_clicked();

private:
    Ui::npcedit *ui;

    bool maybeSave();
    void setCurrentFile(const QString &fileName);
    void setDataBoxes();
    QString strippedName(const QString &fullFileName);
    QString curFile;
    bool isUntitled;
    unsigned int FileType;
};

#endif // NPCEDIT_H
