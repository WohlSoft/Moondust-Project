#ifndef LVL_SCTC_PROPS_H
#define LVL_SCTC_PROPS_H

#include <QDockWidget>
#include "mwdock_base.h"

class MainWindow;

namespace Ui {
class LvlSectionProps;
}

class LvlSectionProps : public QDockWidget, public MWDock_Base
{
    Q_OBJECT

    friend class MainWindow;
public:
    explicit LvlSectionProps(QWidget *parent);
    ~LvlSectionProps();
    bool lockSctSettingsProps;

public slots:
    void re_translate();

    void setLevelSectionData();

    void on_ResizeSection_clicked();

    void on_applyResize_clicked();
    void on_cancelResize_clicked();

    void on_LVLPropsMusicCustom_editingFinished();
    void on_LVLPropsBackImage_currentIndexChanged(int index);

    void switchResizeMode(bool mode);

    void loadMusic();

private slots:
    void on_LvlSectionProps_visibilityChanged(bool visible);

    void on_LVLPropsLevelWarp_clicked(bool checked);
    void on_LVLPropsOffScr_clicked(bool checked);
    void on_LVLPropsNoTBack_clicked(bool checked);
    void on_LVLPropsUnderWater_clicked(bool checked);

    void on_LVLPropsMusicNumber_currentIndexChanged(int index);
    void on_LVLPropsMusicCustomEn_toggled(bool checked);
    void on_LVLPropsMusicCustomBrowse_clicked();

protected:
    virtual void focusInEvent(QFocusEvent * ev);

private:
    Ui::LvlSectionProps *ui;
};

#endif // LVL_SCTC_PROPS_H
