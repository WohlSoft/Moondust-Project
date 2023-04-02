#include <QFileDialog>
#include <QMessageBox>
#include <QtDebug>

#include "textdata/text_data_processor.h"
#include "qfile_dialogs_default_options.hpp"

#include "translator_main.h"
#include "ui_translator_main.h"


TranslatorMain::TranslatorMain(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TranslatorMain)
{
    ui->setupUi(this);
    loadSetup();
}

TranslatorMain::~TranslatorMain()
{
    delete ui;
}

void TranslatorMain::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void TranslatorMain::on_actionOpen_project_triggered()
{
    QString d = QFileDialog::getExistingDirectory(this,
                                                  tr("Open Episode project"),
                                                  m_recentPath,
                                                  c_dirDialogOptions);
    if(d.isEmpty())
        return;

    TextDataProcessor t;

    if(!t.loadProject(d, m_project))
    {
        QMessageBox::warning(this,
                             tr("Loading error"),
                             tr("Failed to load project from the directory: %1").arg(d),
                             QMessageBox::Ok);
    }

    m_recentPath = d;
    m_currentPath = d;

    saveSetup();
}

void TranslatorMain::on_actionRescan_triggered()
{
    if(m_currentPath.isEmpty())
        return; // No opened project

    TextDataProcessor t;
    t.scanEpisode(m_currentPath, m_project);
}

void TranslatorMain::loadSetup()
{
    m_setup.beginGroup("Main");
    m_recentPath = m_setup.value("recent-path").toString();
    m_setup.endGroup();
}

void TranslatorMain::saveSetup()
{
    m_setup.beginGroup("Main");
    m_setup.setValue("recent-path", m_recentPath);
    m_setup.endGroup();
    m_setup.sync();
}

