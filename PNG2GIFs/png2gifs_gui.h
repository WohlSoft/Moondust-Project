#ifndef PNG2GIFS_GUI_H
#define PNG2GIFS_GUI_H

#include <QObject>
#include <QDialog>
#include <QTimer>

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
    void closeEvent(QCloseEvent *event);

private slots:
    void on_LabelDropFiles_clicked();
    void on_targetPathBrowse_clicked();
    static void doConversion(PNG2GIFsGUI *ptr);
    void doLoop();

private:
    friend  void doConversion(PNG2GIFsGUI *ptr);
    Ui::PNG2GIFsGUI *ui;
    bool isConverting;
    QStringList ConversionTask;
    QTimer loop;
};

#endif // PNG2GIFS_GUI_H
