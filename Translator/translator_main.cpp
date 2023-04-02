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
                                                  QString(),
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
}
