#ifndef MUSICFILELIST_H
#define MUSICFILELIST_H

#include <QDialog>
#include <QListWidgetItem>

namespace Ui {
class MusicFileList;
}

class MusicFileList : public QDialog
{
    Q_OBJECT

public:
    QString SelectedFile;
    explicit MusicFileList(QString Folder, QWidget *parent = 0);
    ~MusicFileList();

private slots:
    void on_FileList_itemDoubleClicked(QListWidgetItem *item);

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::MusicFileList *ui;
};

#endif // MUSICFILELIST_H
