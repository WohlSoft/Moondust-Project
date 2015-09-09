#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QDialog>

namespace Ui {
class MaintainerMain;
}

class MaintainerMain : public QDialog
{
    Q_OBJECT

public:
    explicit MaintainerMain(QWidget *parent = 0);
    ~MaintainerMain();

private slots:
    void on_quitOut_clicked();

    void on_music_converter_batch_clicked();

private:
    Ui::MaintainerMain *ui;
};

#endif // MAIN_WINDOW_H
