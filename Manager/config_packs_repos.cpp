#include "config_packs_repos.h"
#include <ui_config_packs_repos.h>
#include "config_packs.h"
#include <QMessageBox>
#include <QInputDialog>

ConfPacksRepos::ConfPacksRepos(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfPacksRepos)
{
    ui->setupUi(this);
    lockAdd=false;


    clearList();
    for(int i=0; i<cpacks_reposList.size(); i++)
        addItemToList(cpacks_reposList[i]);
}

void ConfPacksRepos::addItemToList(ConfigPackRepo &rp)
{
    lockAdd=true;
    int itemFlags = (Qt::ItemIsEditable|Qt::ItemIsEnabled|Qt::ItemIsUserCheckable|Qt::ItemIsSelectable);
    QListWidgetItem *x = new QListWidgetItem(ui->repoList);
    x->setText(rp.url);
    x->setCheckState(rp.enabled?Qt::Checked:Qt::Unchecked);
    x->setFlags((Qt::ItemFlag)itemFlags);
    x->setData(Qt::UserRole, QVariant::fromValue<void* >(&rp));
    ui->repoList->addItem(x);
    lockAdd=false;
}


ConfPacksRepos::~ConfPacksRepos()
{
    clearList();
    delete ui;
}


void ConfPacksRepos::on_add_clicked()
{
    bool ok=false;
    tryAgain:
    QString url=QInputDialog::getText(this, tr("Add repository"), tr("Please enter repository URL"), QLineEdit::Normal, QString(), &ok);
    if(!ok) return;
    foreach(ConfigPackRepo rp, cpacks_reposList)
    {
        if(rp.url==url)
        {
            QMessageBox::warning(this, tr("Repo exist"), tr("This repository already exists in the list.\nPlease try again!"), QMessageBox::Ok);
            goto tryAgain;
        }
    }
    ConfigPackRepo rp;
    rp.url=url;
    rp.enabled=true;
    cpacks_reposList.push_back(rp);
    addItemToList(cpacks_reposList.last());
}

void ConfPacksRepos::on_checkRepo_clicked()
{

}

void ConfPacksRepos::on_remove_clicked()
{
    QList<QListWidgetItem*> itms = ui->repoList->selectedItems();
    if(itms.isEmpty()) return;
    if(QMessageBox::question(this, tr("Remove repository"), tr("Are you want to remove selected repository from list?"), QMessageBox::Yes|QMessageBox::No)!=QMessageBox::Yes)
        return;

    ConfigPackRepo rp=*(ConfigPackRepo*)itms.first()->data(Qt::UserRole).value<void*>();
    for(int i=0;i<cpacks_reposList.size(); i++)
        if(cpacks_reposList[i].url==rp.url)
        {
            cpacks_reposList.removeAt(i);
            break;
        }
    delete itms.first();
}

void ConfPacksRepos::clearList()
{
    QList<QListWidgetItem*> items = ui->repoList->findItems("*", Qt::MatchWildcard);
    foreach(QListWidgetItem*it, items)
        delete it;
}


void ConfPacksRepos::on_repoList_itemChanged(QListWidgetItem *item)
{
    if(lockAdd) return;
    if(!item) return;
    if(item->data(Qt::UserRole).isNull()) return;
    ConfigPackRepo &rp=*(ConfigPackRepo*)item->data(Qt::UserRole).value<void*>();
    rp.url=item->text();
    rp.enabled=(item->checkState()==Qt::Checked);
}
