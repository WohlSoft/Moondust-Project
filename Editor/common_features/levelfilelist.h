#ifndef LEVELFILELIST_H
#define LEVELFILELIST_H

#include <QDialog>
#include <QListWidgetItem>

namespace Ui {
class LevelFileList;
}

class LevelFileList : public QDialog
{
    Q_OBJECT

public:
    explicit LevelFileList(QString Folder, QString current="", QWidget *parent = 0);
    ~LevelFileList();
    QString SelectedFile;

private slots:
    void on_FileList_itemDoubleClicked(QListWidgetItem *item);

    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    Ui::LevelFileList *ui;
};

#endif // LEVELFILELIST_H
