#ifndef BOOKMARKS_BOX_H
#define BOOKMARKS_BOX_H

#include <QDockWidget>
#include <QModelIndex>
#include "mwdock_base.h"

class MainWindow;
class QListWidgetItem;

namespace Ui {
class BookmarksBox;
}

class BookmarksBox : public QDockWidget, public MWDock_Base
{
    Q_OBJECT

    friend class MainWindow;
private:
    explicit BookmarksBox(QWidget *parent = 0);
    ~BookmarksBox();

public:
    void updateBookmarkBoxByList();
    void updateBookmarkBoxByData();

public slots:
    void re_translate();

private slots:
    void on_BookmarksBox_visibilityChanged(bool visible);
    void on_bookmarkList_customContextMenuRequested(const QPoint &pos);
    void DragAndDroppedBookmark(QModelIndex /*sourceParent*/,int sourceStart,int sourceEnd,QModelIndex /*destinationParent*/,int destinationRow);
    void on_bookmarkList_doubleClicked(const QModelIndex &index);
//Modificators:
    void on_bookmarkAdd_clicked();
    void on_bookmarkRemove_clicked();
    void on_bookmarkList_itemChanged(QListWidgetItem *item);
//Go To...
    void on_bookmarkGoto_clicked();

private:
    Ui::BookmarksBox *ui;
};

#endif // BOOKMARKS_BOX_H
