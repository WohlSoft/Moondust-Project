#ifndef PNG2GIFS_H
#define PNG2GIFS_H

#include <QDialog>
#include <QProcess>

namespace Ui {
class PNG2GIFs;
}

class PNG2GIFs : public QDialog
{
    Q_OBJECT

    QProcess m_png2gifs;
public:
    explicit PNG2GIFs(QWidget *parent = nullptr);
    ~PNG2GIFs();

protected:
    void closeEvent(QCloseEvent *e);
    void changeEvent(QEvent *e);
    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent *e);

private slots:
    void on_doMadJob_clicked();
    void on_inPathBrowse_clicked();
    void processOutput();
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    void toggleBusy(bool busy);
    Ui::PNG2GIFs *ui;
};

#endif // PNG2GIFS_H
