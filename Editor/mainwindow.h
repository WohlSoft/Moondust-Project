#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include <QPixmap>
#include <QAbstractListModel>
#include <QList>
#include <QPoint>
#include <QString>
#include <QStringList>
#include "lvl_filedata.h"
#include "wld_filedata.h"
#include "npc_filedata.h"
#include "childwindow.h"
#include "leveledit.h"
#include "npcedit.h"

QT_BEGIN_NAMESPACE
class QMimeData;
QT_END_NAMESPACE

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QMdiArea *parent = 0);
    ~MainWindow();
    
protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void save();
    void save_as();
    void close_sw();
    void save_all();

    void SetCurrentLevelSection(int SctId, int open=0);

    LevelData ReadLevelFile(QFile &inf);
    NPCConfigFile ReadNpcTXTFile(QFile &inf);
    WorldData ReadWorldFile(QFile &inf);

    void OpenFile(QString FilePath);

    npcedit *createNPCChild();

    leveledit *createChild();
    void setActiveSubWindow(QWidget *window);

    void updateMenus();


    void on_OpenFile_activated();
    void on_Exit_activated();
    void on_actionAbout_activated();
    void on_LevelToolBox_visibilityChanged(bool visible);
    void on_actionLVLToolBox_activated();
    void on_actionWLDToolBox_activated();
    void on_WorldToolBox_visibilityChanged(bool visible);
    void on_actionLevelProp_activated();
    void on_pushButton_4_clicked();
    void on_actionSection_Settings_activated();
    void on_LevelSectionSettings_visibilityChanged(bool visible);
    void on_actionSave_activated();
    void on_actionSave_as_activated();
    void on_actionClose_activated();
    void on_actionSave_all_activated();
    void on_actionSection_1_activated();
    void on_actionSection_2_activated();
    void on_actionSection_3_activated();
    void on_actionSection_4_activated();
    void on_actionSection_5_activated();
    void on_actionSection_6_activated();
    void on_actionSection_7_activated();
    void on_actionSection_8_activated();
    void on_actionSection_9_activated();
    void on_actionSection_10_activated();
    void on_actionSection_11_activated();
    void on_actionSection_12_activated();
    void on_actionSection_13_activated();
    void on_actionSection_14_activated();
    void on_actionSection_15_activated();
    void on_actionSection_16_activated();
    void on_actionSection_17_activated();
    void on_actionSection_18_activated();
    void on_actionSection_19_activated();
    void on_actionSection_20_activated();
    void on_actionSection_21_activated();

private:
    Ui::MainWindow *ui;
    QMdiSubWindow *findMdiChild(const QString &fileName);
    QSignalMapper *windowMapper;

    int activeChildWindow();
    npcedit *activeNpcEditWin();
    leveledit *activeLvlEditWin();

};

void BadFileMsg(MainWindow *window, QString fileName_DATA, int str_count, QString line);

////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////

#endif // MAINWINDOW_H
