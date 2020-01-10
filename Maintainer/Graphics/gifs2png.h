#ifndef GIFS2PNG_H
#define GIFS2PNG_H

#include <QDialog>

namespace Ui {
class GIFs2PNG;
}

class GIFs2PNG : public QDialog
{
    Q_OBJECT

public:
    explicit GIFs2PNG(QWidget *parent = nullptr);
    ~GIFs2PNG();

protected:
    void changeEvent(QEvent *e);

private slots:
    void on_DoMadJob_clicked();

    void on_configPackPathBrowse_clicked();

    void on_inPathBrowse_clicked();

private:
    Ui::GIFs2PNG *ui;
};

#endif // GIFS2PNG_H
