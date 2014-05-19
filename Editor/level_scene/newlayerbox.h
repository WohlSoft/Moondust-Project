#ifndef NEWLAYERBOX_H
#define NEWLAYERBOX_H

#include <QDialog>
#include "lvl_filedata.h"

namespace Ui {
class ToNewLayerBox;
}

class ToNewLayerBox : public QDialog
{
    Q_OBJECT

public:
    explicit ToNewLayerBox(LevelData *lData, QWidget *parent = 0);
    ~ToNewLayerBox();
    QString lName;
    bool lHidden;
    bool lLocked;
    bool valid;

    LevelData * LvlData;

private slots:
    void on_buttonBox_accepted();

private:
    Ui::ToNewLayerBox *ui;
};

#endif // NEWLAYERBOX_H
