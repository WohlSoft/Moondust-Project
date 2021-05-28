#ifndef PALETTE_FILTER_H
#define PALETTE_FILTER_H

#include <QDialog>
#include <QProcess>

namespace Ui {
class PaletteFilter;
}

class PaletteFilter : public QDialog
{
    Q_OBJECT

    QProcess m_paletteFilter;
public:
    explicit PaletteFilter(QWidget *parent = nullptr);
    ~PaletteFilter();

protected:
    void closeEvent(QCloseEvent *e);
    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent *e);

private slots:
    void on_doMadJob_clicked();
    void on_inPathBrowse_clicked();
    void processOutput();
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);

    void on_paletteImgBrowse_clicked();

private:
    void toggleBusy(bool busy);
    Ui::PaletteFilter *ui;
};

#endif // PALETTE_FILTER_H
