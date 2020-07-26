#ifndef IMAGESELECTOR_H
#define IMAGESELECTOR_H

#include <QWidget>
#include <QDialog>
#include <QMap>

class QPushButton;
class QToolButton;
class QVBoxLayout;
class QGridLayout;
class ImageSelector;
class QListWidget;
class QListWidgetItem;
class QLineEdit;

class ImageSelectorMenu : public QDialog
{
    Q_OBJECT

    QGridLayout *m_layout;
    QListWidget *m_list;
    QLineEdit *m_filter;
    int m_currentItem = -1;

    QMap<int, QListWidgetItem*> m_items;

public:
    explicit ImageSelectorMenu(QWidget *parent = nullptr);
    ~ImageSelectorMenu();

    void clear();
    void addItem(const QPixmap &picture, const QString &title, int key);
    void updateItem(int key, const QPixmap &picture, const QString &title);
    void getItem(int key, QPixmap &picture, QString &title);
    void setItem(int key);
    int  currentItem();

    void autoSelect();

private slots:
    void itemClicked(QListWidgetItem *item);
    void doAccept();
    void doReject();

protected:
    void focusOutEvent(QFocusEvent *event);

signals:
    void currentItemChanged(int key);
};

class ImageSelector : public QWidget
{
    Q_OBJECT
    QToolButton *m_button;
    QVBoxLayout *m_layout;
    ImageSelectorMenu *m_menuBox;

    void updateView();

private slots:
    void itemChangeProcess(int key);

public:
    explicit ImageSelector(QWidget *parent = nullptr);
    ~ImageSelector();

    void clear();
    void addItem(const QPixmap &picture, const QString &title, int key);
    void updateItem(int key, const QPixmap &picture, const QString &title);

    void setItem(int key);
    int  currentItem();
    QSize sizeHint() const override;

signals:
    void currentItemChanged(int key);

};

#endif // IMAGESELECTOR_H
