#include "configstatus.h"
#include "ui_configstatus.h"

ConfigStatus::ConfigStatus(dataconfigs &conf, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigStatus)
{
    configs = &conf;
    ui->setupUi(this);

    //Create Statistics
    ui->ItemsStatus->clear();
    QListWidgetItem * item;

    ui->ConfigNameLabel->setText(ConfStatus::configName);
    ui->ConfigPathLabel->setText(configs->config_dir);

    //Level Blocks
    item = new QListWidgetItem;
    item->setText(tr("Level Blocks (%1/%1)").arg(configs->main_block.size()).arg(ConfStatus::total_blocks));
    if(configs->main_block.size()==0)
        item->setIcon(QIcon(QPixmap(":/images/conf_bad.png")));
    else
    if(configs->main_block.size()<ConfStatus::total_blocks)
        item->setIcon(QIcon(QPixmap(":/images/conf_warn.png")));
    else
        item->setIcon(QIcon(QPixmap(":/images/conf_good.png")));
    ui->ItemsStatus->addItem(item);

    //Level BGO
    item = new QListWidgetItem;
    item->setText(tr("Level Background objects (%1/%1)").arg(configs->main_bgo.size()).arg(ConfStatus::total_bgo));
    if(configs->main_bgo.size()==0)
        item->setIcon(QIcon(QPixmap(":/images/conf_bad.png")));
    else
    if(configs->main_bgo.size()<ConfStatus::total_bgo)
        item->setIcon(QIcon(QPixmap(":/images/conf_warn.png")));
    else
        item->setIcon(QIcon(QPixmap(":/images/conf_good.png")));
    ui->ItemsStatus->addItem(item);

    //Level BG
    item = new QListWidgetItem;
    item->setText(tr("Level Background images (%1/%1)").arg(configs->main_bg.size()).arg(ConfStatus::total_bg));
    if(configs->main_bg.size()==0)
        item->setIcon(QIcon(QPixmap(":/images/conf_bad.png")));
    else
    if(configs->main_bg.size()<ConfStatus::total_bg)
        item->setIcon(QIcon(QPixmap(":/images/conf_warn.png")));
    else
        item->setIcon(QIcon(QPixmap(":/images/conf_good.png")));
    ui->ItemsStatus->addItem(item);

    //Level NPC
    item = new QListWidgetItem;
    item->setText(tr("Level NPCs (%1/%1)").arg(configs->main_npc.size()).arg(ConfStatus::total_npc));
    if(configs->main_npc.size()==0)
        item->setIcon(QIcon(QPixmap(":/images/conf_bad.png")));
    else
    if(configs->main_npc.size()<ConfStatus::total_npc)
        item->setIcon(QIcon(QPixmap(":/images/conf_warn.png")));
    else
        item->setIcon(QIcon(QPixmap(":/images/conf_good.png")));
    ui->ItemsStatus->addItem(item);

    //MUSIC
    item = new QListWidgetItem;
    item->setText(tr("Music (level) (%1/%1)").arg(configs->main_music_lvl.size()).arg(ConfStatus::total_music_lvl));
    if(configs->main_music_lvl.size()==0)
        item->setIcon(QIcon(QPixmap(":/images/conf_bad.png")));
    else
    if(configs->main_music_lvl.size()<ConfStatus::total_music_lvl)
        item->setIcon(QIcon(QPixmap(":/images/conf_warn.png")));
    else
        item->setIcon(QIcon(QPixmap(":/images/conf_good.png")));
    ui->ItemsStatus->addItem(item);

    item = new QListWidgetItem;
    item->setText(tr("Music (world) (%1/%1)").arg(configs->main_music_wld.size()).arg(ConfStatus::total_music_wld));
    if(configs->main_music_wld.size()==0)
        item->setIcon(QIcon(QPixmap(":/images/conf_bad.png")));
    else
    if(configs->main_music_wld.size()<ConfStatus::total_music_wld)
        item->setIcon(QIcon(QPixmap(":/images/conf_warn.png")));
    else
        item->setIcon(QIcon(QPixmap(":/images/conf_good.png")));
    ui->ItemsStatus->addItem(item);

    item = new QListWidgetItem;
    item->setText(tr("Music (special) (%1/%1)").arg(configs->main_music_spc.size()).arg(ConfStatus::total_music_spc));
    if(configs->main_music_spc.size()==0)
        item->setIcon(QIcon(QPixmap(":/images/conf_bad.png")));
    else
    if(configs->main_music_spc.size()<ConfStatus::total_music_spc)
        item->setIcon(QIcon(QPixmap(":/images/conf_warn.png")));
    else
        item->setIcon(QIcon(QPixmap(":/images/conf_good.png")));
    ui->ItemsStatus->addItem(item);

    //SOUND
    item = new QListWidgetItem;
    item->setText(tr("Sounds (%1/%1)").arg(configs->main_sound.size()).arg(ConfStatus::total_sound));
    if(configs->main_sound.size()==0)
        item->setIcon(QIcon(QPixmap(":/images/conf_bad.png")));
    else
    if(configs->main_sound.size()<ConfStatus::total_sound)
        item->setIcon(QIcon(QPixmap(":/images/conf_warn.png")));
    else
        item->setIcon(QIcon(QPixmap(":/images/conf_good.png")));
    ui->ItemsStatus->addItem(item);



    ////////////////////////DirList/////////////////////////
    QTableWidgetItem * itemDir;
    ui->ItemsDirs->insertRow(0);
        itemDir = new QTableWidgetItem(tr("Level data"));
        ui->ItemsDirs->setItem(0, 0, itemDir);

        itemDir = new QTableWidgetItem(configs->dirs.glevel);
        ui->ItemsDirs->setItem(0, 1, itemDir);
    ui->ItemsDirs->insertRow(1);
        itemDir = new QTableWidgetItem(tr("World data"));
        ui->ItemsDirs->setItem(1, 0, itemDir);

        itemDir = new QTableWidgetItem(configs->dirs.gworld);
        ui->ItemsDirs->setItem(1, 1, itemDir);

    ui->ItemsDirs->insertRow(2);
        itemDir = new QTableWidgetItem(tr("Characters"));
        ui->ItemsDirs->setItem(2, 0, itemDir);

        itemDir = new QTableWidgetItem(configs->dirs.gplayble);
        ui->ItemsDirs->setItem(2, 1, itemDir);

    ui->ItemsDirs->insertRow(3);
        itemDir = new QTableWidgetItem(tr("Game worlds"));
        ui->ItemsDirs->setItem(3, 0, itemDir);

        itemDir = new QTableWidgetItem(configs->dirs.worlds);
        ui->ItemsDirs->setItem(3, 1, itemDir);

    ui->ItemsDirs->insertRow(4);
        itemDir = new QTableWidgetItem(tr("Music"));
        ui->ItemsDirs->setItem(4, 0, itemDir);

        itemDir = new QTableWidgetItem(configs->dirs.music);
        ui->ItemsDirs->setItem(4, 1, itemDir);

    ui->ItemsDirs->insertRow(5);
        itemDir = new QTableWidgetItem(tr("Sounds"));
        ui->ItemsDirs->setItem(5, 0, itemDir);

        itemDir = new QTableWidgetItem(configs->dirs.sounds);
        ui->ItemsDirs->setItem(5, 1, itemDir);

    ui->ItemsDirs->insertRow(6);
        itemDir = new QTableWidgetItem(tr("Custom data"));
        ui->ItemsDirs->setItem(6, 0, itemDir);

        itemDir = new QTableWidgetItem(configs->dirs.gcustom);
        ui->ItemsDirs->setItem(6, 1, itemDir);

}

ConfigStatus::~ConfigStatus()
{
    delete ui;
}


void ConfigStatus::on_buttonBox_accepted()
{
    this->close();
}
