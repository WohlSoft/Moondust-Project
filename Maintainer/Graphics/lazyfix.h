#ifndef LAZYFIXTOOL_H
#define LAZYFIXTOOL_H

#include <QDialog>
#include <QProcess>

namespace Ui {
class LazyFixTool;
}

class LazyFixTool : public QDialog
{
    Q_OBJECT

    QProcess m_lazyfix;
public:
    explicit LazyFixTool(QWidget *parent = nullptr);
    ~LazyFixTool();

protected:
    void closeEvent(QCloseEvent *e);
    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent *e);

private slots:
    void on_doMadJob_clicked();
    void on_inPathBrowse_clicked();
    void processOutput();
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    void toggleBusy(bool busy);
    Ui::LazyFixTool *ui;
};

#endif // LAZYFIXTOOL_H
