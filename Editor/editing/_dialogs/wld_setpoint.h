#ifndef WLD_SETPOINT_H
#define WLD_SETPOINT_H

#include <QDialog>

#include <PGE_File_Formats/wld_filedata.h>
#include <common_features/logger.h>
#include <common_features/mainwinconnect.h>
#include <editing/_scenes/world/wld_scene.h>

namespace Ui {
class WLD_SetPoint;
}

class WLD_SetPoint : public QDialog
{
    Q_OBJECT

public:
    explicit WLD_SetPoint(QWidget *parent = 0);
    ~WLD_SetPoint();
    QPoint mapPoint;
    bool mapPointIsNull;


    WorldData WldData;
    //QGraphicsScene LvlScene;

    long currentMusic;

    bool loadFile(const QString &fileName, WorldData FileData, dataconfigs &configs, LevelEditingSettings options);

    QString userFriendlyCurrentFile();
    QString currentFile() { return curFile; }

    void ResetPosition();
    void goTo(long x, long y, bool SwitchToSection=false, QPoint offset=QPoint(0,0));


    WldScene * scene;

    bool sceneCreated;
    bool isUntitled;

    QString curFile;

    QTimer *updateTimer;
    void setAutoUpdateTimer(int ms);
    void stopAutoUpdateTimer();

    void unloadData();
    QWidget *gViewPort();

protected:
    void closeEvent(QCloseEvent *event);

public slots:
    void pointSelected(QPoint p);

private slots:
    void updateScene();
    void on_buttonBox_clicked(QAbstractButton *button);

    void on_ResetPosition_clicked();
    void on_GotoPoint_clicked();
    void on_animation_clicked(bool checked);


private:
    void documentWasModified();

    WorldData StartWldData;
    bool DrawObjects(QProgressDialog &progress);

    void setCurrentFile(const QString &fileName);
    void setDataBoxes();
    QString strippedName(const QString &fullFileName);
    QString latest_export_path;
    unsigned int FileType;

    Ui::WLD_SetPoint *ui;
};

#endif // WLD_SETPOINT_H
