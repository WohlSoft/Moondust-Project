#include <QFileDialog>
#include <QMessageBox>
#include <QtDebug>

#include "textdata/text_data_processor.h"
#include "textdata/files_list_model.h"
#include "textdata/files_strings.h"
#include "qfile_dialogs_default_options.hpp"

#include "translator_main.h"
#include "ui_translator_main.h"


TranslatorMain::TranslatorMain(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TranslatorMain)
{
    ui->setupUi(this);
    loadSetup();
    m_filesListModel = new FilesListModel(&m_project, ui->filesListTable);
    ui->filesListTable->setModel(m_filesListModel);

    m_filesStringsModel = new FilesStringsModel(&m_project, ui->fileStrings);
    ui->fileStrings->setModel(m_filesStringsModel);

    QObject::connect(ui->filesListTable, &QAbstractItemView::clicked,
    this,
    [this](const QModelIndex &index)->void
    {
        if(!index.isValid())
        {
            m_filesStringsModel->clear();
            return;
        }

        QString key = index.data(FilesListModel::R_KEY).toString();
        int type = index.data(FilesListModel::R_TYPE).toInt();
        m_filesStringsModel->setData("origin", type, key);
    });

    QObject::connect(ui->fileStrings, &QAbstractItemView::clicked,
    this,
    [this](const QModelIndex &index)->void
    {
        if(!index.isValid())
            return;

        QString text = index.sibling(index.row(), FilesStringsModel::C_TITLE).data(Qt::DisplayRole).toString();
        ui->previewZone->setText(text);
    });

    QObject::connect(ui->legacyLineBreak,
                     static_cast<void(QCheckBox::*)(bool)>(&QCheckBox::clicked),
                     ui->previewZone,
                     &MsgBoxPreview::setVanillaMode);
    ui->previewZone->setVanillaMode(ui->legacyLineBreak->isChecked());

    ui->previewFontSize->setValue(ui->previewZone->fontSize());
    QObject::connect(ui->previewFontSize,
                     static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                     ui->previewZone,
                     &MsgBoxPreview::setFontSize);

    updateActions();
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

    m_filesStringsModel->clear();
    m_filesListModel->rebuildView(d);

    m_recentPath = d;
    m_currentPath = d;
    updateActions();
    saveSetup();

    ui->statusbar->showMessage(tr("Project %1 has been loaded!").arg(d));
}

void TranslatorMain::on_actionRescan_triggered()
{
    if(m_currentPath.isEmpty())
        return; // No opened project

    TextDataProcessor t;
    t.scanEpisode(m_currentPath, m_project);
    m_filesStringsModel->clear();
    m_filesListModel->rebuildView(m_currentPath);
}

void TranslatorMain::on_actionSaveTranslations_triggered()
{
    if(m_currentPath.isEmpty())
        return;

    TextDataProcessor t;
    t.saveProject(m_currentPath, m_project);
    ui->statusbar->showMessage(tr("Project %1 has been saved!").arg(m_currentPath));
}

void TranslatorMain::on_actionCloseProject_triggered()
{
    m_project.clear();
    m_filesStringsModel->clear();
    m_filesListModel->rebuildView(m_currentPath);
    m_currentPath.clear();
    updateActions();
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

void TranslatorMain::updateActions()
{
    bool isLoaded = !m_currentPath.isEmpty();
    ui->actionRescan->setEnabled(isLoaded);
    ui->actionSaveTranslations->setEnabled(isLoaded);
    ui->actionCloseProject->setEnabled(isLoaded);
}

void TranslatorMain::on_actionQuit_triggered()
{
    this->close();
}
