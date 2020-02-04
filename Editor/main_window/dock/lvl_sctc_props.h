#pragma once
#ifndef LVL_SCTC_PROPS_H
#define LVL_SCTC_PROPS_H

#include <QDockWidget>
#include <QMutex>
#include "mwdock_base.h"

class MainWindow;
class QPushButton;
class JsonSettingsWidget;

namespace Ui {
class LvlSectionProps;
}

class LvlSectionProps : public QDockWidget, public MWDock_Base
{
    Q_OBJECT

    friend class MainWindow;
private:
    explicit LvlSectionProps(QWidget *parent);
    ~LvlSectionProps();

public:
    bool m_externalLock = false;
    QPushButton *getEditBackground2IniButton();

public slots:
    void setSMBX64Strict(bool en);
    void re_translate();

    void initDefaults();
    void refreshFileData();

    void on_ResizeSection_clicked();

    void on_applyResize_clicked();
    void on_cancelResize_clicked();

    void on_LVLPropsMusicCustom_editingFinished();
    void on_LVLPropsBackImage_currentIndexChanged(int index);
    void on_editBackground2Ini_clicked();

    void switchResizeMode(bool mode);

    void loadMusic();

private slots:
    void on_LVLPropsWrapHorizontal_clicked(bool checked);
    void on_LVLPropsWrapVertical_clicked(bool checked);
    void on_LVLPropsOffScr_clicked(bool checked);
    void on_LVLPropsNoTBack_clicked(bool checked);
    void on_LVLPropsUnderWater_clicked(bool checked);

    void on_LVLPropsMusicNumber_currentIndexChanged(int index);
    void on_LVLPropsMusicCustomEn_toggled(bool checked);
    void on_LVLPropsMusicCustomBrowse_clicked();


protected:
    virtual void focusInEvent(QFocusEvent * ev);

private:
    void updateExtraSettingsWidget();
    void onExtraSettingsChanged();

    QMutex m_mutex;
    JsonSettingsWidget *m_extraSettings = nullptr;

    Ui::LvlSectionProps *ui;
};

#endif // LVL_SCTC_PROPS_H
