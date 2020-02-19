#pragma once
#ifndef WLD_SETTINGS_BOX_H
#define WLD_SETTINGS_BOX_H

#include <QDockWidget>
#include <QCheckBox>
#include <QMap>
#include <QSet>
#include "mwdock_base.h"

class MainWindow;
struct WorldLevelTile;

namespace Ui
{
    class WorldSettingsBox;
}

class WorldSettingsBox : public QDockWidget, public MWDock_Base
{
    Q_OBJECT

    friend class MainWindow;
    explicit WorldSettingsBox(QWidget *parent = nullptr);
    ~WorldSettingsBox();

public slots:
    void re_translate();
    void setCurrentWorldSettings();

private slots:
    void on_WorldSettingsBox_visibilityChanged(bool visible);

    void on_WLD_Title_editingFinished();
    void on_WLD_NoWorldMap_clicked(bool checked);
    void updateLevelIntroLabel();
    void on_WLD_RestartLevel_clicked(bool checked);
    void on_WLD_AutostartLvl_editingFinished();
    void on_WLD_AutostartLvlBrowse_clicked();
    void on_WLD_Stars_valueChanged(int arg1);
    void on_WLD_DoCountStars_clicked();
    void on_WLD_Credirs_textChanged();
    void characterActivated(bool checked);

signals:
    void countedStar(int);

private:
    Ui::WorldSettingsBox *ui;
    bool m_lockSettings = false;
    QMap<QCheckBox *, int> m_charactersCheckBoxes;

    //for star counter
    unsigned long doStarCount(QString dir, QList<WorldLevelTile > levels, QString introLevel);
    unsigned long starCounter_checkLevelFile(QString FilePath, QSet<QString> &exists);
    bool m_starCounter_canceled = false;
};

#endif // WLD_SETTINGS_BOX_H
