#include "main_window.h"
#include "ui_main_window.h"

#include <Music/audiocvt_sox_gui.h>
#include <Graphics/gifs2png.h>
#include <Graphics/png2gifs.h>
#include <Graphics/lazyfix.h>
#include <EpisodeCvt/episode_converter.h>
#include <EpisodeCvt/case_fixer.h>
#include <common_features/app_path.h>
#include <QMessageBox>

void MaintainerMain::showWIP()
{
    QMessageBox::information(this,
                             tr("Under construction"),
                             tr("This feature is under construction, "
                                "it doesn't work properly yet. It may damage some of your stuff you'll try to process. "
                                "Therefore please, make a full backup of episode that you would to process by this thing."),
                             QMessageBox::Ok);
}

MaintainerMain::MaintainerMain(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MaintainerMain)
{
    ui->setupUi(this);

    QObject::connect(&m_translator, &Translator::languageSwitched,
                     this, &MaintainerMain::languageSwitched);
    ui->language->setMenu(&m_langMenu);
    m_translator.setSettings(&m_langMenu,
                             "maintainer",
                             AppPathManager::languagesDir(),
                             AppPathManager::settingsFile());
    m_translator.initTranslator();
}

MaintainerMain::~MaintainerMain()
{
    delete ui;
}

void MaintainerMain::languageSwitched()
{
    ui->retranslateUi(this);
}

void MaintainerMain::on_quitOut_clicked()
{
    this->close();
}

const Qt::WindowFlags g_dialogFlags =
        Qt::Window |
        Qt::WindowSystemMenuHint |
        Qt::WindowTitleHint |
        Qt::WindowCloseButtonHint |
        Qt::WindowMinimizeButtonHint;

void MaintainerMain::on_music_converter_batch_clicked()
{
    AudioCvt_Sox_gui mus(nullptr);
    mus.setWindowFlags(g_dialogFlags);
    mus.setWindowModality(Qt::NonModal);
    this->hide();
    mus.exec();
    this->show();
}

void MaintainerMain::on_episode_converter_clicked()
{
    EpisodeConverter eps(nullptr);
    eps.setWindowFlags(g_dialogFlags);
    eps.setWindowModality(Qt::NonModal);
    this->hide();
    eps.exec();
    this->show();
}

void MaintainerMain::on_gifs2png_converter_clicked()
{
    GIFs2PNG g2p(nullptr);
    g2p.setWindowFlags(g_dialogFlags);
    g2p.setWindowModality(Qt::NonModal);
    this->hide();
    g2p.exec();
    this->show();
}

void MaintainerMain::on_png2gifs_converter_clicked()
{
    PNG2GIFs p2g(nullptr);
    p2g.setWindowFlags(g_dialogFlags);
    p2g.setWindowModality(Qt::NonModal);
    this->hide();
    p2g.exec();
    this->show();
}

void MaintainerMain::on_lazyfix_tool_clicked()
{
    LazyFixTool lzf(nullptr);
    lzf.setWindowFlags(g_dialogFlags);
    lzf.setWindowModality(Qt::NonModal);
    this->hide();
    lzf.exec();
    this->show();
}

void MaintainerMain::on_pathcase_fixer_clicked()
{
    showWIP();
    CaseFixer casefixer(nullptr);
    casefixer.setWindowFlags(g_dialogFlags);
    casefixer.setWindowModality(Qt::NonModal);
    this->hide();
    casefixer.exec();
    this->show();
}
