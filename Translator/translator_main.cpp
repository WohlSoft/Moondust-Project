#include <QFileDialog>
#include <QMessageBox>
#include <QtDebug>

#include "textdata/text_data_processor.h"
#include "textdata/files_list_model.h"
#include "textdata/files_strings.h"
#include "textdata/dialogues_list_model.h"
#include "qfile_dialogs_default_options.hpp"
#include "translate_field.h"
#include "dialogue_item.h"

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
    ui->fileStrings->setColumnWidth(0, 30);
    ui->fileStrings->setColumnWidth(1, 30);
    ui->fileStrings->setColumnWidth(2, 200);

    m_dialoguesListModel = new DialoguesListModel(&m_project, ui->dialoguesList);
    ui->dialoguesList->setModel(m_dialoguesListModel);
    ui->dialoguesList->setColumnWidth(0, 24);


    QObject::connect(ui->filesListTable->selectionModel(),
                     &QItemSelectionModel::selectionChanged,
    this,
    [this](const QItemSelection &selected, const QItemSelection &)->void
    {
        auto ar = selected.indexes();
        if(ar.isEmpty() || !ar.first().isValid())
        {
            m_dialogueItems.clear();
            m_dialoguesListModel->clear();
            m_filesStringsModel->clear();
            return;
        }

        m_dialogueItems.clear();
        auto &index = ar.first();
        QString key = index.data(FilesListModel::R_KEY).toString();
        int type = index.data(FilesListModel::R_TYPE).toInt();
        m_filesStringsModel->setData("origin", type, key);

        if(type == FilesListModel::T_LEVEL)
            m_dialoguesListModel->setData("origin", key);
        else
            m_dialoguesListModel->clear();
    });

    QObject::connect(ui->fileStrings->selectionModel(),
                     &QItemSelectionModel::selectionChanged,
    this,
    [this](const QItemSelection &selected, const QItemSelection &)->void
    {
        auto ar = selected.indexes();
        if(ar.isEmpty() || !ar.first().isValid())
        {
            ui->previewZone->clearText();
            ui->sourceLineRO->clear();
            return;
        }

        auto &index = ar.first();

        QString text = index.sibling(index.row(), FilesStringsModel::C_TITLE).data(Qt::DisplayRole).toString();
        ui->previewZone->setText(text);
        ui->sourceLineRO->setPlainText(text);
    });

    QObject::connect(ui->dialoguesList->selectionModel(),
                     &QItemSelectionModel::selectionChanged,
    this,
    [this](const QItemSelection &selected, const QItemSelection &)->void
    {
        auto ar = selected.indexes();
        if(ar.isEmpty() || !ar.first().isValid())
        {
            m_dialogueItems.clear();
            return;
        }

        auto &index = ar.first();

        int key = index.sibling(index.row(), DialoguesListModel::C_INDEX).data(Qt::DisplayRole).toInt();
        auto &l = m_dialoguesListModel->level();
        auto lk = m_dialoguesListModel->levelKey();

        m_dialogueItems.clear();

        for(TranslationData_DialogueNode &d : l.dialogues[key].messages)
        {
            if(d.item_index == -1)
                continue; // skip invalid items

            if(d.type == TranslationData_DialogueNode::T_END)
                break;

            switch(d.type)
            {
            case TranslationData_DialogueNode::T_NPC_TALK:
            {
                QSharedPointer<DialogueItem> dd(new DialogueItem(&m_project,
                                                                 "origin",
                                                                 lk,
                                                                 DialogueItem::T_NPC,
                                                                 d.item_index,
                                                                 ui->dialoguePreview));
                ui->dialoguePreviewLayout->insertWidget(ui->dialoguePreviewLayout->count() - 1, dd.data());
                QObject::connect(dd.data(),
                                 &DialogueItem::clicked,
                                 this,
                [this, d, lk]()->void
                {
                    auto &l = m_project["origin"].levels[lk];
                    auto t = l.npc[d.item_index].talk;
                    ui->previewZone->setText(t);
                    ui->sourceLineRO->setPlainText(t);
                });
                m_dialogueItems.push_back(std::move(dd));
                break;
            }

            case TranslationData_DialogueNode::T_EVENT_MSG:
            {
                QSharedPointer<DialogueItem> dd(new DialogueItem(&m_project,
                                                                 "origin",
                                                                 lk,
                                                                 DialogueItem::T_EVENT,
                                                                 d.item_index,
                                                                 ui->dialoguePreview));
                ui->dialoguePreviewLayout->insertWidget(ui->dialoguePreviewLayout->count() - 1, dd.data());
                QObject::connect(dd.data(),
                                 &DialogueItem::clicked,
                                 this,
                [this, d, lk]()->void
                {
                    auto &l = m_project["origin"].levels[lk];
                    auto t = l.events[d.item_index].message;
                    ui->previewZone->setText(t);
                    ui->sourceLineRO->setPlainText(t);
                });
                m_dialogueItems.push_back(std::move(dd));
                break;
            }
            default:
                break;
            }
        }
    });

    QObject::connect(ui->legacyLineBreak,
                     static_cast<void(QCheckBox::*)(bool)>(&QCheckBox::clicked),
                     ui->previewZone,
                     &MsgBoxPreview::setVanillaMode);
    ui->previewZone->setVanillaMode(ui->legacyLineBreak->isChecked());

    QObject::connect(ui->previewFontSize,
                     static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                     ui->previewZone,
                     &MsgBoxPreview::setFontSize);
    ui->previewZone->setFontSize(ui->previewFontSize->value());

    updateRecent();
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

void TranslatorMain::closeEvent(QCloseEvent *)
{
    saveSetup();
}

void TranslatorMain::on_actionOpen_project_triggered()
{
    QString d = QFileDialog::getExistingDirectory(this,
                                                  tr("Open Episode project"),
                                                  m_recentPath,
                                                  c_dirDialogOptions);
    if(d.isEmpty())
        return;

    openProject(d);
}

void TranslatorMain::on_actionRescan_triggered()
{
    if(m_currentPath.isEmpty())
        return; // No opened project

    TextDataProcessor t;
    t.scanEpisode(m_currentPath, m_project);
    ui->previewZone->clearText();
    m_dialogueItems.clear();
    m_dialoguesListModel->clear();
    m_filesStringsModel->clear();
    m_filesListModel->rebuildView(m_currentPath);

    updateTranslateFields();
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
    m_translateFields.clear();
    m_project.clear();
    ui->previewZone->clearText();
    m_dialogueItems.clear();
    m_dialoguesListModel->clear();
    m_filesStringsModel->clear();
    m_filesListModel->rebuildView(m_currentPath);
    m_currentPath.clear();
    updateActions();
}

void TranslatorMain::loadSetup()
{
    m_setup.beginGroup("Main");
    {
        m_recentPath = m_setup.value("recent-path").toString();
        int size = m_setup.beginReadArray("recent-files");
        for(int i = 0; i < size; ++i)
        {
            m_setup.setArrayIndex(i);
            m_recentProjects.push_back(m_setup.value("file").toString());
        }
        m_setup.endArray();
    }
    m_setup.endGroup();

    m_setup.beginGroup("UI");
    {
        restoreState(m_setup.value("window-state").toByteArray());
        restoreGeometry(m_setup.value("window-geometry").toByteArray());
        ui->splitter->restoreState(m_setup.value("splitter-state").toByteArray());
        ui->legacyLineBreak->setChecked(m_setup.value("preview-vanilla", false).toBool());
        ui->previewFontSize->setValue(m_setup.value("preview-font-size", 14).toInt());
    }
    m_setup.endGroup();
}

void TranslatorMain::saveSetup()
{
    m_setup.beginGroup("Main");
    {
        m_setup.setValue("recent-path", m_recentPath);
        m_setup.beginWriteArray("recent-files");
        for(int i = 0; i < m_recentProjects.size(); ++i)
        {
            m_setup.setArrayIndex(i);
            m_setup.setValue("file", m_recentProjects[i]);
        }
        m_setup.endArray();
    }
    m_setup.endGroup();

    m_setup.beginGroup("UI");
    {
        m_setup.setValue("window-state", saveState());
        m_setup.setValue("window-geometry", saveGeometry());
        m_setup.setValue("splitter-state", ui->splitter->saveState());
        m_setup.setValue("preview-vanilla", ui->legacyLineBreak->isChecked());
        m_setup.setValue("preview-font-size", ui->previewFontSize->value());
    }
    m_setup.endGroup();

    m_setup.sync();
}

void TranslatorMain::openProject(const QString &d)
{
    TextDataProcessor t;

    if(!t.loadProject(d, m_project))
    {
        QMessageBox::warning(this,
                             tr("Loading error"),
                             tr("Failed to load project from the directory: %1").arg(d),
                             QMessageBox::Ok);
    }

    ui->previewZone->clearText();
    m_dialogueItems.clear();
    m_dialoguesListModel->clear();
    m_filesStringsModel->clear();
    m_filesListModel->rebuildView(d);

    m_recentPath = d;
    m_currentPath = d;
    if(!m_recentProjects.contains(d))
    {
        m_recentProjects.push_front(d);
        if(m_recentProjects.size() > 10)
            m_recentProjects.pop_back();
    }
    else
    {
        int i = m_recentProjects.indexOf(d);
        if(i != 0)
            m_recentProjects.swapItemsAt(i, 0);
    }

    updateRecent();
    updateActions();
    saveSetup();

    updateTranslateFields();

    ui->statusbar->showMessage(tr("Project %1 has been loaded!").arg(d));
}

void TranslatorMain::updateActions()
{
    bool isLoaded = !m_currentPath.isEmpty();
    ui->actionRescan->setEnabled(isLoaded);
    ui->actionSaveTranslations->setEnabled(isLoaded);
    ui->actionCloseProject->setEnabled(isLoaded);
}

void TranslatorMain::updateRecent()
{
    ui->menuOpenRecentProject->clear();

    if(m_recentProjects.isEmpty())
        ui->menuOpenRecentProject->addAction(ui->actionRecentNoProject);
    else
    {
        for(auto &r : m_recentProjects)
        {
            QFileInfo d(r);
            auto *e = ui->menuOpenRecentProject->addAction(d.fileName());
            QObject::connect(e,
                             static_cast<void(QAction::*)(bool)>(&QAction::triggered),
                             this,
                             [this, r](bool)->void
            {
                openProject(r);
            });
        }
    }

    ui->menuOpenRecentProject->addSeparator();
    ui->menuOpenRecentProject->addAction(ui->actionClearRecentProjects);
}

void TranslatorMain::updateTranslateFields()
{
    m_translateFields.clear();

    for(auto k = m_project.begin(); k != m_project.end(); ++k)
    {
        if(k.key() == "origin")
            continue; // Ignore origin translation

        QSharedPointer<TranslateField> f(new TranslateField(ui->translationGroup));

        ui->translationLayout->insertWidget(ui->translationLayout->count() - 1, f.data());
        m_translateFields.insert(k.key(), std::move(f));
    }

}

void TranslatorMain::on_actionQuit_triggered()
{
    this->close();
}

void TranslatorMain::on_actionClearRecentProjects_triggered()
{
    m_recentProjects.clear();
    updateRecent();
}

