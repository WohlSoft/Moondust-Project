#ifndef IMAGESELECTOR_H
#define IMAGESELECTOR_H

#include <QWidget>
#include <QDialog>

class QPushButton;
class QVBoxLayout;
class QGridLayout;
class ImageSelector;
class QListWidget;
class QLineEdit;

class ImageSelectorMenu : public QDialog
{
    Q_OBJECT

    QGridLayout *m_layout;
    QListWidget *m_list;
    QLineEdit *m_filter;

public:
    explicit ImageSelectorMenu(QWidget *parent = nullptr);
    ~ImageSelectorMenu();

protected:
    void focusOutEvent(QFocusEvent *event);

signals:

};

class ImageSelector : public QWidget
{
    Q_OBJECT
    QPushButton *m_button;
    QVBoxLayout *m_layout;
    ImageSelectorMenu *m_menuBox;
public:
    explicit ImageSelector(QWidget *parent = nullptr);
    ~ImageSelector();

signals:

};

#endif // IMAGESELECTOR_H
