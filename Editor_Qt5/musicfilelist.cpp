#include "musicfilelist.h"
#include "ui_musicfilelist.h"
#include <QDir>

MusicFileList::MusicFileList(QString Folder, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MusicFileList)
{
    QStringList filters;
    QDir musicDir(Folder);
    filters << "*.mp3" << "*.ogg" << "*.wav" << "*.mid";
    musicDir.setSorting(QDir::Name);
    musicDir.setNameFilters(filters);
    ui->setupUi(this);
    ui->FileList->insertItems(musicDir.entryList().size(), musicDir.entryList(filters) );
}

MusicFileList::~MusicFileList()
{
    delete ui;
}

void MusicFileList::on_FileList_itemDoubleClicked(QListWidgetItem *item)
{
    SelectedFile = item->text();
    accept();
}

void MusicFileList::on_buttonBox_accepted()
{
    foreach (QListWidgetItem * container, ui->FileList->selectedItems()) {
    SelectedFile = container->text();
    }
    if(SelectedFile!="")
        accept();
}

void MusicFileList::on_buttonBox_rejected()
{
    reject();
}
