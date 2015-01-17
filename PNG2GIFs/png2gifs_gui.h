#ifndef PNG2GIFS_GUI_H
#define PNG2GIFS_GUI_H

#include <QObject>
#include <QDialog>
#include <QFuture>

namespace Ui {
class PNG2GIFsGUI;
}

class PNG2GIFsGUI : public QDialog
{
    Q_OBJECT

public:
    explicit PNG2GIFsGUI(QWidget *parent = 0);
    virtual ~PNG2GIFsGUI();

protected:
    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent *e);

private slots:
    void on_LabelDropFiles_clicked();
    void on_targetPathBrowse_clicked();
    void doConversion(PNG2GIFsGUI *ptr);

private:
    friend void doConversion(PNG2GIFsGUI *ptr);
    Ui::PNG2GIFsGUI *ui;
    bool isConverting;
    QStringList ConversionTask;
    QFuture<void> taskFuture;
};

#endif // PNG2GIFS_GUI_H
