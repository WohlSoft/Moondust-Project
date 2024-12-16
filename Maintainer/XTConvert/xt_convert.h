#ifndef xt_convert_H
#define xt_convert_H

#include <QDialog>
#include <QQueue>
#include <QProcess>
#include <files/episode_box.h>
#include <QFuture>

namespace Ui {
class XTConvertUI;
}

class XTConvertUI : public QDialog
{
    Q_OBJECT

public:
    explicit XTConvertUI(QWidget *parent = 0);
    ~XTConvertUI();
    void updateControls();
    void start();
    void stop(bool do_abort=false);

signals:
    void doNextStep(int retStatus, int exitStatus);
    void gotMax(int max);

private slots:
    void on_start_clicked();
    void on_browse_content_clicked();
    void on_browse_output_clicked();

protected:
    void closeEvent( QCloseEvent * e );

private:
    QFuture<void> process_thread;

    QString m_recent_content_path;
    QString m_recent_output_path;

    bool m_target_asset_pack = false;

    bool m_in_progress = false;
    Ui::XTConvertUI *ui;

    //statistics
    QStringList logs;
};

#endif // xt_convert_H
