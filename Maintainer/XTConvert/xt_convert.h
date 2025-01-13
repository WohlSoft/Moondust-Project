#ifndef XT_CONVERT_H
#define XT_CONVERT_H

#include <QDialog>
#include <QQueue>
#include <QThread>
#include <memory>

#include "libxtconvert.h"

struct XTConvertUpdate
{
    // if -1, is a progress update; if -2, a completion report
    XTConvert::LogCategory log_category = XTConvert::LogCategory::Category_Count;
    QString file_name; // or error message (if any) if log_category is -2

    int cur_stage;
    int stage_count;
    QString stage_name;
    int cur_file;
    int file_count;
};

Q_DECLARE_METATYPE(XTConvert::Spec);
Q_DECLARE_METATYPE(XTConvertUpdate);

class XTConvert_Worker : public QObject
{
    Q_OBJECT

    bool is_running = false;
    bool is_cancelled = false;

public:

    void log_file_callback(XTConvert::LogCategory log_category, const std::string& filename);
    int progress_callback(int cur_stage, int stage_count, const std::string& stage_name, int cur_file, int file_count, const std::string& file_name);

signals:
    void status_update(XTConvertUpdate update);
    void finish();

public slots:
    void do_cancel();
    void do_run(XTConvert::Spec spec);
};

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

signals:
    void do_run(XTConvert::Spec);
    void do_cancel();

private slots:
    void on_start_clicked();
    void on_browse_content_clicked();
    void on_browse_output_clicked();

    void on_status_update(XTConvertUpdate update);
    void on_finish();

protected:
    void closeEvent( QCloseEvent * e );

private:
    QThread process_thread;
    std::unique_ptr<QObject> worker;

    QString m_recent_content_path;
    QString m_recent_output_path;

    bool m_target_asset_pack = false;

    bool m_in_progress = false;
    Ui::XTConvertUI *ui;

    //statistics
    QStringList logs;
};

#endif // XT_CONVERT_H
