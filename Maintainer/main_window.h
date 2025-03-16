#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QDialog>
#include <QMenu>

#include <translator-qt/translator.h>

namespace Ui
{
    class MaintainerMain;
}

class MaintainerMain : public QDialog
{
    Q_OBJECT

public:
    explicit MaintainerMain(QWidget *parent = nullptr);
    ~MaintainerMain();

    void showWIP();

private slots:
    void languageSwitched();

    void on_quitOut_clicked();
    void on_music_converter_batch_clicked();
    void on_episode_converter_clicked();
    void on_gifs2png_converter_clicked();
    void on_png2gifs_converter_clicked();
    void on_lazyfix_tool_clicked();
    void on_palette_filter_clicked();
    void on_pathcase_fixer_clicked();
#ifdef PGE_ENABLE_XTCONVERT
    void on_xt_convert_clicked();
#endif

private:
    Ui::MaintainerMain *ui;
    QMenu      m_langMenu;
    Translator m_translator;
};

#endif // MAIN_WINDOW_H
