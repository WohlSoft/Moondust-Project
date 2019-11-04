#pragma once
#ifndef WLD_SETPOINT_H
#define WLD_SETPOINT_H

#include <QDialog>
#include <memory>

#include <PGE_File_Formats/wld_filedata.h>
#include <common_features/logger.h>
#include <common_features/main_window_ptr.h>
#include <editing/_scenes/world/wld_scene.h>

namespace Ui
{
class WLD_SetPoint;
}

class MainWindow;

class WLD_SetPoint : public QDialog
{
    Q_OBJECT

public:
    explicit WLD_SetPoint(QWidget *parent = 0);
    ~WLD_SetPoint();
    QPoint  m_mapPoint;
    bool    m_mapPointIsNull = true;

    WorldData m_wldData;

    long m_currentMusic = 0;

    bool loadFile(const QString &fileName, WorldData FileData, dataconfigs &configs, EditingSettings options);

    QString userFriendlyCurrentFile();
    QString currentFile()
    {
        return m_curFile;
    }

    void resetPosition();
    void goTo(long x, long y, QPoint offset = QPoint(0, 0));

    MainWindow *m_mw = nullptr;

    std::unique_ptr<WldScene> m_scene;

    bool m_sceneCreated = false;
    bool m_isUntitled = true;

    QString m_curFile;

    std::unique_ptr<QTimer> m_updateTimer;
    void setAutoUpdateTimer(int ms);
    void stopAutoUpdateTimer();

    void unloadData();
    QWidget *gViewPort();

protected:
    void closeEvent(QCloseEvent *event);
    virtual void keyPressEvent(QKeyEvent *e);

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
    QString m_lastExportPath;
    unsigned int m_fileType = 0;

    std::unique_ptr<Ui::WLD_SetPoint> ui;
};

#endif // WLD_SETPOINT_H
