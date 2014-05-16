#ifndef NPCDIALOG_H
#define NPCDIALOG_H

#include <QDialog>
#include "data_configs/data_configs.h"

namespace Ui {
class NpcDialog;
}

class NpcDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NpcDialog(dataconfigs *configs, QWidget *parent = 0);
    ~NpcDialog();
    int selectedNPC;
    bool isEmpty;
    bool isCoin;
    int coins;
    int mode;

    void setState(int npcID, int mode=0);

private slots:
    void on_npcList_doubleClicked(const QModelIndex &index);

    void on_npcList_activated(const QModelIndex &index);

    void on_buttonBox_accepted();

    void on_NPCfromList_toggled(bool checked);

    void on_coinsNPC_toggled(bool checked);

private:
    dataconfigs * pConfigs;
    Ui::NpcDialog *ui;
};

#endif // NPCDIALOG_H
