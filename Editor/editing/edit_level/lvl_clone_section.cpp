#include "lvl_clone_section.h"
#include "ui_lvl_clone_section.h"

#include <QMessageBox>

#include "./common_features/util.h"
#include "./file_formats/file_formats.h"

#include <QtDebug>

LvlCloneSection::LvlCloneSection(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LvlCloneSection)
{
    ui->setupUi(this);

    clone_margin=32;
    clone_source = NULL;
    clone_source_id = 0;
    clone_target = NULL;
    clone_target_id = 0;
}


void LvlCloneSection::addLevelList(QList<LevelEdit* > _levels, LevelEdit *active)
{
    levels = _levels;
    int ActiveIndex=0;

    ui->FileList_src->clear();
    ui->FileList_dst->clear();

    foreach(LevelEdit * x, levels)
    {
        ui->FileList_src->addItem(x->windowTitle(), qVariantFromValue(x) );
        ui->FileList_dst->addItem(x->windowTitle(), qVariantFromValue(x) );

        if(active==x)
            ActiveIndex=ui->FileList_src->count()-1;
    }

    ui->FileList_src->setCurrentIndex(ActiveIndex);
    ui->FileList_dst->setCurrentIndex(ActiveIndex);
}



LvlCloneSection::~LvlCloneSection()
{
    delete ui;

}

void LvlCloneSection::on_FileList_src_currentIndexChanged(int index)
{
    ui->SectionList_src->setEnabled(index>=0);
    if(index<0) return;
    LevelEdit * x = ui->FileList_src->currentData().value<LevelEdit *>();

    util::memclear(ui->SectionList_src);
    QListWidgetItem * item;
    foreach(LevelSection y, x->LvlData.sections)
    {
        item = new QListWidgetItem(tr("Section")+QString(" %1 %2").arg(y.id+1)
                                   .arg(
                                       ( (y.size_bottom==0) && (y.size_left==0) && (y.size_top==0) && (y.size_right==0))?
                                           tr("[Uninitialized]"):
                                           tr("")
                                       )
                                   );
        item->setData(3, QString::number(y.id));
        item->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled );

        ui->SectionList_src->addItem(item);
    }
}


void LvlCloneSection::on_FileList_dst_currentIndexChanged(int index)
{
    ui->SectionList_dst->setEnabled(index>=0);
    if(index<0) return;
    LevelEdit * x = ui->FileList_dst->currentData().value<LevelEdit *>();

    util::memclear(ui->SectionList_dst);
    QListWidgetItem * item;
    foreach(LevelSection y, x->LvlData.sections)
    {
        item = new QListWidgetItem(tr("Section")+QString(" %1 %2").arg(y.id+1)
                                   .arg(
                                       ( (y.size_bottom==0) && (y.size_left==0) && (y.size_top==0) && (y.size_right==0))?
                                           tr(""):
                                           tr("[Busy]")
                                       )
                                   );
        item->setData(3, QString::number(y.id));
        item->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled );

        ui->SectionList_dst->addItem(item);
    }

}



void LvlCloneSection::on_buttonBox_accepted()
{
    if(ui->SectionList_src->selectedItems().isEmpty() ||
       ui->SectionList_dst->selectedItems().isEmpty() )
    {
        QMessageBox::warning(this, tr("Sections aren't selected"),
                             tr("Source and Destination sections should be selected!"));
        return;
    }

    clone_margin= ui->padding_src->value();

    LevelSection tmps;
    tmps = FileFormats::dummyLvlSection();

    clone_source = ui->FileList_src->currentData().value<LevelEdit *>();
    clone_source_id = ui->SectionList_src->selectedItems().first()->data(3).toInt();

    foreach(LevelSection x, clone_source->LvlData.sections)
    {
        if(x.id==clone_source_id){ tmps = x; break;}
    }

//        qDebug() << "source " << clone_source_id << tmps.size_bottom << tmps.size_left
//                                                 << tmps.size_right << tmps.size_top;

    if( (tmps.size_bottom==0) && (tmps.size_left==0) && (tmps.size_top==0) && (tmps.size_right==0) )
    {
        QMessageBox::warning(this, tr("Empty section"),
                             tr("Source section is empty!\nPlease select another section."));
        return;
    }

    clone_target = ui->FileList_dst->currentData().value<LevelEdit *>();
    clone_target_id = ui->SectionList_dst->selectedItems().first()->data(3).toInt();

    foreach(LevelSection x, clone_target->LvlData.sections)
    {
        if(x.id==clone_target_id){ tmps = x; break;}
    }

//        qDebug() << "target " << clone_target_id << tmps.size_bottom << tmps.size_left
//                                                 << tmps.size_right << tmps.size_top;

    if( (tmps.size_bottom!=0) || (tmps.size_left!=0) || (tmps.size_top!=0) || (tmps.size_right!=0))
    {
        QMessageBox::warning(this, tr("Section is busy"),
                             tr("Destination section is busy!\nPlease select another section\nor delete them first with 'Delete Section' option."));
        return;
    }

    this->accept();
}
