#ifndef SFX_TESTER_H
#define SFX_TESTER_H

#include <QDialog>

namespace Ui {
class SfxTester;
}

struct Mix_Chunk;

class SfxTester : public QDialog
{
    Q_OBJECT

public:
    explicit SfxTester(QWidget *parent = nullptr);
    ~SfxTester();

private slots:
    void on_sfx_open_clicked();
    void openSfx(const QString &path);
    void on_sfx_play_clicked();
    void on_sfx_fadeIn_clicked();
    void on_sfx_stop_clicked();
    void on_sfx_fadeout_clicked();

protected:
    void dropEvent(QDropEvent *e);
    void dragEnterEvent(QDragEnterEvent *e);
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *);

private:
    Ui::SfxTester *ui;

    Mix_Chunk *m_testSfx = nullptr;
    QString    m_testSfxDir;
};

#endif // SFX_TESTER_H
