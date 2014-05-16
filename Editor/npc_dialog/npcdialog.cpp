#include "npcdialog.h"
#include "ui_npcdialog.h"

NpcDialog::NpcDialog(dataconfigs *configs, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NpcDialog)
{
    pConfigs = configs;
    ui->setupUi(this);
    isEmpty = false;
    isCoin = false;
    selectedNPC = 0;
    coins = 0;

    QListWidgetItem * item;
    QPixmap tmpI;

    ui->npcList->clear();

    foreach(obj_npc npcItem, pConfigs->main_npc)
    {
        //Add category

            tmpI = npcItem.image.copy(0,0, npcItem.image.width(), npcItem.gfx_h );

            item = new QListWidgetItem( npcItem.name );
            item->setIcon( QIcon( tmpI ) );
            item->setData(3, QString::number(npcItem.id) );
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );
            item->setSelected( npcItem.id==1 );

            ui->npcList->addItem( item );
    }

}

NpcDialog::~NpcDialog()
{
    delete ui;
}

void NpcDialog::on_npcList_doubleClicked(const QModelIndex &index)
{
    selectedNPC = index.data(3).toInt();
    isEmpty = false;
    isCoin = false;
    this->accept();
}

void NpcDialog::on_npcList_activated(const QModelIndex &index)
{
    on_npcList_doubleClicked(index);
}

void NpcDialog::on_buttonBox_accepted()
{

    if(ui->NPCfromList->isChecked())
    {
        if(ui->npcList->selectedItems().isEmpty()) return;
        isEmpty = false;
        isCoin = false;
        selectedNPC = ui->npcList->selectedItems()[0]->data(3).toInt();
        coins=0;
    }
    else
    if(ui->emptyNPC->isChecked())
    {
        isEmpty = true;
        isCoin = false;
        selectedNPC = 0;
        coins=0;
    }
    else
    if(ui->emptyNPC->isChecked())
    {
        isEmpty = true;
        isCoin = false;
        selectedNPC = 0;
        coins=ui->coinsInBlock->value();
    }

    this->accept();
}

void NpcDialog::on_NPCfromList_toggled(bool checked)
{
    ui->npcList->setEnabled(checked);
}

void NpcDialog::on_coinsNPC_toggled(bool checked)
{
     ui->coinsInBlock->setEnabled(checked);
}
