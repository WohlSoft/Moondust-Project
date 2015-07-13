#include "config_packs_repos.h"
#include <ui_config_packs_repos.h>
#include "config_packs.h"

ConfPacksRepos::ConfPacksRepos(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfPacksRepos)
{
    ui->setupUi(this);

    int itemFlags = (Qt::ItemIsEditable|Qt::ItemIsEnabled|Qt::ItemIsUserCheckable|Qt::ItemIsSelectable);
    clearList();
    for(int i=0; i<cpacks_reposList.size(); i++)
    {
        QListWidgetItem *x = new QListWidgetItem(ui->repoList);
        x->setText(cpacks_reposList[i].url);
        x->setCheckState(cpacks_reposList[i].enabled?Qt::Checked:Qt::Unchecked);
        x->setFlags((Qt::ItemFlag)itemFlags);
        x->setData(Qt::UserRole, QVariant::fromValue<void* >(&cpacks_reposList[i]));
        ui->repoList->addItem(x);
    }
}

ConfPacksRepos::~ConfPacksRepos()
{
    clearList();
    delete ui;
}


void ConfPacksRepos::on_add_clicked()
{

}

void ConfPacksRepos::on_checkRepo_clicked()
{

}

void ConfPacksRepos::on_remove_clicked()
{

}

void ConfPacksRepos::on_repoList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{

}

void ConfPacksRepos::clearList()
{
    QList<QListWidgetItem*> items = ui->repoList->findItems("*", Qt::MatchWildcard);
    foreach(QListWidgetItem*it, items)
        delete it;
}

