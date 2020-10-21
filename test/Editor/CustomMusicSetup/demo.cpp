#include "demo.h"
#include "ui_demo.h"

Demo::Demo(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Demo)
{
    ui->setupUi(this);
    m_musSetup.initLists();
    ui->musSetup->setEnabled(false);
}

Demo::~Demo()
{
    delete ui;
}


void Demo::on_musSetup_clicked()
{
    m_musSetup.show();
}

void Demo::on_music_editingFinished()
{
    m_musSetup.setMusicPath(ui->music->text());
    ui->musSetup->setEnabled(m_musSetup.settingsNeeded());
}
