#include "sfx_tester.h"
#include "ui_sfx_tester.h"
#include <QFocusEvent>
#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QCloseEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>

#include "../Player/mus_player.h"

SfxTester::SfxTester(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SfxTester)
{
    ui->setupUi(this);

    layout()->activate();
    adjustSize();

    QSettings setup;
    m_testSfxDir = setup.value("RecentSfxDir", "").toString();
}

SfxTester::~SfxTester()
{
    delete ui;
}

void SfxTester::dropEvent(QDropEvent *e)
{
    this->raise();
    this->setFocus(Qt::ActiveWindowFocusReason);

    QString sfxFile;

    for(const QUrl &url : e->mimeData()->urls())
    {
        const QString &fileName = url.toLocalFile();
        sfxFile = fileName;
    }

    if(!sfxFile.isEmpty())
        openSfx(sfxFile);

    e->accept();
}

void SfxTester::dragEnterEvent(QDragEnterEvent *e)
{
    if(e->mimeData()->hasUrls())
        e->acceptProposedAction();
}

void SfxTester::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void SfxTester::closeEvent(QCloseEvent *)
{
    if(m_testSfx)
        Mix_FreeChunk(m_testSfx);
    m_testSfx = nullptr;

    ui->sfx_file->clear();

    QSettings setup;
    setup.setValue("RecentSfxDir", m_testSfxDir);
    setup.sync();
}

void SfxTester::on_sfx_open_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Open SFX file"),
                   (m_testSfxDir.isEmpty() ? QApplication::applicationDirPath() : m_testSfxDir), "All (*.*)");

    if(file.isEmpty())
        return;
    openSfx(file);
}

void SfxTester::openSfx(const QString &path)
{
    if(m_testSfx)
    {
        Mix_HaltChannel(0);
        Mix_FreeChunk(m_testSfx);
        m_testSfx = nullptr;
    }

    m_testSfx = Mix_LoadWAV(path.toUtf8().data());
    if(!m_testSfx)
        QMessageBox::warning(this, "SFX open error!", QString("Mix_LoadWAV: ") + Mix_GetError());
    else
    {
        QFileInfo f(path);
        m_testSfxDir = f.absoluteDir().absolutePath();
        ui->sfx_file->setText(f.fileName());
    }
}


void SfxTester::on_sfx_play_clicked()
{
    if(!m_testSfx)
        return;

#if defined(SDL_MIXER_X) || defined(SDL_MIXER_GE21)
    if(Mix_PlayChannelTimedVolume(0,
                                  m_testSfx,
                                  ui->sfx_loops->value(),
                                  ui->sfx_timed->value(),
                                  ui->sfx_volume->value()) == -1)
    {
        QMessageBox::warning(this, "SFX play error!", QString("Mix_PlayChannelTimedVolume: ") + Mix_GetError());
    }
#else
    int chan = Mix_PlayChannelTimed(0,
                                    m_testSfx,
                                    ui->sfx_loops->value(),
                                    ui->sfx_timed->value());
    if(chan == -1)
        QMessageBox::warning(this, "SFX play error!", QString("Mix_PlayChannelTimed: ") + Mix_GetError());
    else
        Mix_Volume(chan, ui->sfx_volume->value());
#endif
}

void SfxTester::on_sfx_fadeIn_clicked()
{
    if(!m_testSfx)
        return;

#if defined(SDL_MIXER_X) || defined(SDL_MIXER_GE21)
    if(Mix_FadeInChannelTimedVolume(0,
                                    m_testSfx,
                                    ui->sfx_loops->value(),
                                    ui->sfx_fadems->value(),
                                    ui->sfx_timed->value(),
                                    ui->sfx_volume->value()) == -1)
    {
        QMessageBox::warning(this, "SFX play error!", QString("Mix_PlayChannelTimedVolume: ") + Mix_GetError());
    }
#else
    int chan = Mix_FadeInChannelTimed(0,
                                     m_testSfx,
                                     ui->sfx_loops->value(),
                                     ui->sfx_fadems->value(),
                                     ui->sfx_timed->value());
    if(chan == -1)
        QMessageBox::warning(this, "SFX play error!", QString("Mix_PlayChannelTimed: ") + Mix_GetError());
    else
        Mix_Volume(chan, ui->sfx_volume->value());
#endif
}

void SfxTester::on_sfx_stop_clicked()
{
    if(!m_testSfx)
        return;
    Mix_HaltChannel(0);
}

void SfxTester::on_sfx_fadeout_clicked()
{
    if(!m_testSfx)
        return;
    Mix_FadeOutChannel(0, ui->sfx_fadems->value());
}
