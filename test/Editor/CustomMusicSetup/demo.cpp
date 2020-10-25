#include "demo.h"
#include "ui_demo.h"

Demo::Demo(QWidget *parent)
    : QMainWindow(parent)
    , m_musSetup(this)
    , ui(new Ui::Demo)
{
    ui->setupUi(this);
    m_musSetup.initLists();
    ui->musSetup->setEnabled(false);
    QObject::connect(&m_musSetup, &CustomMusicSetup::musicSetupChanged,
                     this, &Demo::musicChanged);
}

Demo::~Demo()
{
    delete ui;
}


void Demo::on_musSetup_clicked()
{
    m_musSetup.exec();
}

void Demo::on_music_editingFinished()
{
    m_musSetup.setMusicPath(ui->music->text());
    ui->musSetup->setEnabled(m_musSetup.settingsNeeded());
}

void Demo::musicChanged(const QString &music)
{
    ui->music->setText(music);
}
