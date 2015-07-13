#ifndef CONFPACKREPOS_H
#define CONFPACKREPOS_H

#include <QDialog>
#include <QListWidgetItem>

namespace Ui {
class ConfPacksRepos;
}

class ConfPacksRepos : public QDialog
{
Q_OBJECT

public:
    explicit ConfPacksRepos(QWidget *parent=0);
    ~ConfPacksRepos();

private slots:
    void on_add_clicked();
    void on_checkRepo_clicked();
    void on_remove_clicked();
    void on_repoList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void clearList();

private:
    Ui::ConfPacksRepos *ui;
};

#endif // CONFPACKREPOS_H
