#ifndef GIFS2PNG_H
#define GIFS2PNG_H

#include <QDialog>
#include <QProcess>

namespace Ui {
class GIFs2PNG;
}

class GIFs2PNG : public QDialog
{
    Q_OBJECT

    QProcess m_gifs2png;
public:
    explicit GIFs2PNG(QWidget *parent = nullptr);
    ~GIFs2PNG();

protected:
    void closeEvent(QCloseEvent *e);
    void changeEvent(QEvent *e);
    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent *e);

private slots:
    void on_doMadJob_clicked();
    void on_configPackPathBrowse_clicked();
    void on_inPathBrowse_clicked();
    void processOutput();
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    void toggleBusy(bool busy);
    Ui::GIFs2PNG *ui;
};

#endif // GIFS2PNG_H
