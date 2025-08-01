/*
 * Moondust Translator, a free tool for internationalisation of levels and episodes
 * This is a part of the Moondust Project, a free platform for game making
 * Copyright (c) 2023-2025 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QFileDialog>
#include <QMessageBox>
#include <QProgressDialog>
#include <QAtomicInt>
#include <QtConcurrent>
#include <QDesktopServices>
#include <QFuture>
#include <QtDebug>

#include "textdata/files_strings.h"

#include "textdata/text_data_processor.h"
#include "textdata/files_list_model.h"
#include "textdata/dialogues_list_model.h"
#include "textdata/langs_list_model.h"

#include "textdata/delegates/checkbox_delegate.h"

#include "lang_select/lang_select.h"

#include "qfile_dialogs_default_options.hpp"
#include "translate_field.h"
#include "dialogue_item.h"
#include "about/about.h"
#include "syntax_msgbox_macros.h"

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
    ui->fileStrings->setColumnWidth(0, 80);
    ui->fileStrings->setColumnWidth(1, 24);
    ui->fileStrings->setColumnWidth(2, 300);

    m_dialoguesListModel = new DialoguesListModel(&m_project, ui->dialoguesList);
    ui->dialoguesList->setModel(m_dialoguesListModel);
    ui->dialoguesList->setColumnWidth(0, 24);

    m_langsListModel = new LangsListModel(&m_project, &m_setup, ui->languagesTable);
    ui->languagesTable->setModel(m_langsListModel);
    ui->languagesTable->setColumnWidth(0, 24);
    ui->languagesTable->setColumnWidth(1, 35);
    ui->languagesTable->setColumnWidth(2, 35);
    ui->languagesTable->setColumnWidth(3, 35);
    CheckBoxDelegate *langSelectVis = new CheckBoxDelegate(ui->languagesTable);
    ui->languagesTable->setItemDelegateForColumn(LangsListModel::C_VISIBLE, langSelectVis);

    QObject::connect(m_langsListModel, &LangsListModel::visibilityChanged,
                     this, &TranslatorMain::updateTranslateFields);

    m_highLighter = s_makeMsgBoxMacrosHighlighter(ui->sourceLineRO->document());

    ui->sourceLineRO->installEventFilter(this);
    ui->sourceLineNote->installEventFilter(this);

    QObject::connect(ui->filesListTable,
                     &QWidget::customContextMenuRequested,
                     this,
                     [this](const QPoint &pos)->void
    {
        const auto &i = ui->filesListTable->currentIndex();
        if(!i.isValid())
            return;

        QMenu menu(this);
        QAction *openFile = menu.addAction(tr("Open file..."));
        QAction *openDir = menu.addAction(tr("Open containing directory"));
        QAction *ret = menu.exec(ui->filesListTable->mapToGlobal(pos));

        QString path = m_currentPath + "/" + i.data(FilesListModel::R_KEY).toString();

        if(ret == openFile)
            QDesktopServices::openUrl(QUrl::fromLocalFile(path));
        else if(ret == openDir)
        {
            QFileInfo d(path);
            QDesktopServices::openUrl(QUrl::fromLocalFile(d.absoluteDir().absolutePath()));
        }
    });


    QObject::connect(ui->filesListTable->selectionModel(),
                     &QItemSelectionModel::selectionChanged,
    this,
    [this](const QItemSelection &selected, const QItemSelection &)->void
    {
        auto ar = selected.indexes();
        if(ar.isEmpty() || !ar.first().isValid())
        {
            resetTranslationFields();
            m_dialogueItems.clear();
            m_dialoguesListModel->clear();
            m_filesStringsModel->clear();
            return;
        }

        resetTranslationFields();
        m_dialogueItems.clear();
        auto &index = ar.first();
        QString key = index.data(FilesListModel::R_KEY).toString();
        int type = index.data(FilesListModel::R_TYPE).toInt();
        m_filesStringsModel->setFileData(m_recentLang, type, key);

        if(type == FilesListModel::T_LEVEL)
            m_dialoguesListModel->setData(m_recentLang, key);
        else
            m_dialoguesListModel->clear();
    });

    QObject::connect(ui->sourceLineRO, &GrowingTextEdit::textChanged,
                     this, &TranslatorMain::updateSourceLineContent);
    QObject::connect(ui->sourceLineNote, &QLineEdit::textChanged,
                     this, &TranslatorMain::updateSourceLineNote);

    QObject::connect(ui->fileStrings->selectionModel(), &QItemSelectionModel::selectionChanged,
                     this,
                     static_cast<void (TranslatorMain::*)(const QItemSelection &, const QItemSelection &)>(&TranslatorMain::syncTranslationFieldsContent));

    QObject::connect(ui->dialoguesList->selectionModel(), &QItemSelectionModel::selectionChanged,
                     this, &TranslatorMain::syncDialoguesList);

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


    // Synchronize values before use
    ui->previewZone->setEnableMacros(ui->preProcessorTest->isChecked());
    ui->previewZone->setMacroPlayerNum(ui->macroPlayerNum->value());
    ui->previewZone->setMacroPlayerState(ui->macroStateNum->value());

    QObject::connect(ui->preProcessorTest,
                     static_cast<void(QGroupBox::*)(bool)>(&QGroupBox::clicked),
                     ui->previewZone,
                     &MsgBoxPreview::setEnableMacros);

    QObject::connect(ui->macroPlayerNum,
                     static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                     ui->previewZone,
                     &MsgBoxPreview::setMacroPlayerNum);

    QObject::connect(ui->macroStateNum,
                     static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                     ui->previewZone,
                     &MsgBoxPreview::setMacroPlayerState);

    updateRecent();
    updateActions();
}

TranslatorMain::~TranslatorMain()
{
    m_highLighter->deleteLater();
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

bool TranslatorMain::eventFilter(QObject *object, QEvent *event)
{
    if(event->type() == QEvent::FocusIn)
    {
        if(object == ui->sourceLineRO || object == ui->sourceLineNote)
        {
            m_recentLang = "metadata";
            syncDialoguesListLang();
            syncStringsListStatus();
            ui->previewZone->setText(ui->sourceLineRO->toPlainText());
        }
    }

    return false;
}

void TranslatorMain::on_actionOpenProject_triggered()
{
    QString d = QFileDialog::getExistingDirectory(this,
                                                  tr("Open Episode project"),
                                                  m_recentPath,
                                                  c_dirDialogOptions);
    if(d.isEmpty())
        return;

    openProject(d);
}

void TranslatorMain::on_actionOpenSingleLevel_triggered()
{
    QString d = QFileDialog::getOpenFileName(this,
                                             tr("Open single-level project"),
                                             m_recentPath,
                                             QString("%1 (*.lvl *.lvlx);;%2 (*.*)")
                                                     .arg(tr("Level files")
                                                     .arg(tr("All files"))),
                                             nullptr,
                                             c_fileDialogOptions);
    if(d.isEmpty())
        return;

    openProject(d, true);
}

void TranslatorMain::on_actionRescan_triggered()
{
    if(m_currentPath.isEmpty())
        return; // No opened project

    ui->previewZone->clearText();
    m_dialogueItems.clear();
    m_dialoguesListModel->clear();
    m_filesStringsModel->clear();

    QFutureWatcher<bool> isOk;
    QEventLoop waitLoop;
    QAtomicInt waitLoopQuit(false);

    QProgressDialog progress(this);
    progress.setLabelText(tr("Scanning..."));
    progress.setCancelButton(nullptr);
    progress.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    progress.reset();

    QObject::connect(&isOk, &QFutureWatcher<bool>::finished,
    [&waitLoop, &waitLoopQuit]()->void
    {
        waitLoop.quit();
        waitLoopQuit = true;
    });

    progress.show();

    // Do the loading in a thread
    isOk.setFuture(QtConcurrent::run([this]()->bool
    {
        TextDataProcessor t;
        if(m_isSingleLevel)
            return t.scanSingleLevel(m_currentLevel, m_project);
        else
            return t.scanEpisode(m_currentPath, m_project);
    }));

    // Now wait until the config load in finished.
    if(!waitLoopQuit)
        waitLoop.exec();

    progress.hide();

    m_filesListModel->rebuildView(m_currentPath);

    updateTranslateFields();
}

void TranslatorMain::on_actionSaveTranslations_triggered()
{
    if(m_currentPath.isEmpty())
        return;

    TextDataProcessor t;
    if(m_isSingleLevel)
        t.saveProjectLevel(m_currentLevel, m_project);
    else
        t.saveProject(m_currentPath, m_project);
    ui->statusbar->showMessage(tr("Project %1 has been saved!").arg(m_currentPath));
}

void TranslatorMain::on_actionCloseProject_triggered()
{
    m_recentLang = "metadata";
    m_translateFields.clear();
    m_project.clear();
    ui->previewZone->clearText();
    m_dialogueItems.clear();
    m_dialoguesListModel->clear();
    m_filesStringsModel->clear();
    m_filesListModel->rebuildView(m_currentPath);
    m_langsListModel->clear();
    m_currentLevel.clear();
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

void TranslatorMain::openProject(const QString &d, bool singleLevel)
{
    QFutureWatcher<bool> isOk;
    QEventLoop waitLoop;
    QAtomicInt waitLoopQuit(false);

    m_isSingleLevel = singleLevel;

    QProgressDialog progress(this);
    progress.setLabelText(tr("Loading..."));
    progress.setCancelButton(nullptr);
    progress.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    progress.reset();

    QObject::connect(&isOk, &QFutureWatcher<bool>::finished,
    [&waitLoop, &waitLoopQuit]()->void
    {
        waitLoop.quit();
        waitLoopQuit = true;
    });

    progress.show();

    // Do the loading in a thread
    isOk.setFuture(QtConcurrent::run([this, &d, singleLevel]()->bool
    {
        TextDataProcessor t;
        if(singleLevel)
            return t.loadProjectLevel(d, m_project);
        else
            return t.loadProject(d, m_project);
    }));

    // Now wait until the config load in finished.
    if(!waitLoopQuit)
        waitLoop.exec();

    progress.hide();

    if(!isOk.result())
    {
        QMessageBox::warning(this,
                             tr("Loading error"),
                             tr("Failed to load project from the directory: %1").arg(d),
                             QMessageBox::Ok);
    }

    m_recentLang = "metadata";
    ui->previewZone->clearText();
    m_dialogueItems.clear();
    m_dialoguesListModel->clear();
    m_filesStringsModel->clear();
    m_filesListModel->rebuildView(d);
    m_langsListModel->refreshData();

    m_recentPath = d;
    if(singleLevel)
    {
        QFileInfo f(d);
        m_currentLevel = d;
        m_currentPath = f.absoluteDir().absolutePath();
    }
    else
    {
        m_currentPath = d;
    }

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
        {
            auto backup = m_recentProjects[i];
            m_recentProjects.erase(m_recentProjects.begin() + i);
            m_recentProjects.push_front(backup);
        }
    }

    updateRecent();
    updateActions();
    saveSetup();

    updateTranslateFields();

    ui->statusbar->showMessage(tr("Project %1 has been loaded!").arg(d));
}

void TranslatorMain::resetTranslationFields()
{
    m_sourceKey.root.clear();
    m_sourceKey.group = -1;
    m_sourceKey.key = -1;
    m_sourceKey.type =  -1;
    ui->previewZone->clearText();
    ui->sourceLineRO->blockSignals(true);
    ui->sourceLineRO->clear();
    ui->sourceLineRO->blockSignals(false);
    ui->sourceLineNote->blockSignals(true);
    ui->sourceLineNote->clear();
    ui->sourceLineNote->setEnabled(false);
    ui->sourceLineNote->blockSignals(false);
    m_sourceNotePtr = nullptr;
    for(auto &k : m_translateFields)
        k->clearItem();
}

void TranslatorMain::syncTranslationFieldsContent()
{
    syncTranslationFieldsContent(ui->fileStrings->selectionModel()->selection(),
                                 ui->fileStrings->selectionModel()->selection());
}

void TranslatorMain::syncTranslationFieldsContent(const QItemSelection &selected, const QItemSelection &)
{
    auto ar = selected.indexes();
    if(ar.isEmpty() || !ar.first().isValid())
    {
        resetTranslationFields();
        return;
    }

    auto &index = ar.first();
    updateTranslationFields(index.sibling(index.row(), FilesStringsModel::C_TITLE));
}

void TranslatorMain::syncDialoguesListLang()
{
    for(auto &d : m_dialogueItems)
        d->setLang(m_recentLang);
}

void TranslatorMain::syncStringsListStatus()
{
    m_filesStringsModel->updateStatus(m_recentLang);
}

void TranslatorMain::syncDialoguesList(const QItemSelection &selected, const QItemSelection &)
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
                                                             m_recentLang,
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
                                 auto &l = m_project["metadata"].levels[lk];
                                 auto &it = l.npc[d.item_index];
                                 updateTranslationFields(TextTypes::S_LEVEL,
                                                         lk,
                                                         TextTypes::LDT_NPC,
                                                         d.item_index,
                                                         it.talk.text,
                                                         it.talk.note);
                             });
            m_dialogueItems.push_back(std::move(dd));
            break;
        }

        case TranslationData_DialogueNode::T_EVENT_MSG:
        {
            QSharedPointer<DialogueItem> dd(new DialogueItem(&m_project,
                                                             m_recentLang,
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
                                 auto &l = m_project["metadata"].levels[lk];
                                 auto &it = l.events[d.item_index];
                                 updateTranslationFields(TextTypes::S_LEVEL,
                                                         lk,
                                                         TextTypes::LDT_EVENT,
                                                         d.item_index,
                                                         it.message.text,
                                                         it.message.note);
                             });
            m_dialogueItems.push_back(std::move(dd));
            break;
        }
        default:
            break;
        }
    }
}

void TranslatorMain::updateTranslationFields(const QModelIndex &s)
{
    QString text = s.data(Qt::DisplayRole).toString();
    QString root = s.data(FilesStringsModel::R_ROOT).toString();
    QString note = s.data(FilesStringsModel::R_NOTE).toString();
    int group = s.data(FilesStringsModel::R_GROUP).toInt();
    int type = s.data(FilesStringsModel::R_TYPE).toInt();
    int key = s.data(FilesStringsModel::R_KEY).toInt();

    m_sourceKey.lastString = s;

    updateTranslationFields(group, root, type, key, text, note);
}

void TranslatorMain::updateTranslationFields(int group,
                                             const QString &root,
                                             int type,
                                             int key,
                                             const QString &text,
                                             const QString &tr_note)
{
    bool isOrigin = m_recentLang == "metadata";
    bool translated = false;
    MsgBoxPreview::PreviewType previewType = MsgBoxPreview::PREVIEW_MESSAGEBOX;
    ui->sourceLineRO->blockSignals(true);
    ui->sourceLineRO->setPlainText(text);
    ui->sourceLineRO->blockSignals(false);
    ui->sourceLineNote->blockSignals(true);
    ui->sourceLineNote->setText(tr_note);
    ui->sourceLineNote->setEnabled(true);
    ui->sourceLineNote->blockSignals(false);
    m_sourceKey.root = root;
    m_sourceKey.group = group;
    m_sourceKey.key = key;
    m_sourceKey.type = type;

    switch(group)
    {
    case TextTypes::S_WORLD:
        switch(type)
        {
        case TextTypes::WDT_LEVEL:
            previewType = MsgBoxPreview::PREVIEW_LEVEL_TITLE;
            break;
        case TextTypes::WDT_CREDITS:
            previewType = MsgBoxPreview::PREVIEW_CREDITS;
            break;
        case TextTypes::WDT_TITLE:
            previewType = MsgBoxPreview::PREVIEW_FILE_TITLE;
            break;
        }

        break;
    case TextTypes::S_LEVEL:
        switch(type)
        {
        case TextTypes::LDT_TITLE:
            previewType = MsgBoxPreview::PREVIEW_FILE_TITLE;
            break;
        default:
            break;
        }

        break;
    case TextTypes::S_SCRIPT:
        previewType = MsgBoxPreview::PREVIEW_SCRIPT_PRINT;
        break;
    }

    for(auto &k : m_translateFields)
    {
        k->setItem(group, root, type, key);
        if(!isOrigin && k->getLang() == m_recentLang)
        {
            auto &ss = k->getText();
            ui->previewZone->setPreviewType(previewType);
            if(!ss.isEmpty())
            {
                ui->previewZone->setText(ss);
                translated = true;
            }
        }
    }

    if(!translated)
    {
        ui->previewZone->setPreviewType(previewType);
        ui->previewZone->setText(text);
    }
}

void TranslatorMain::updateSourceLineNote()
{
    if(m_sourceKey.root.isEmpty() || m_sourceKey.group < 0 || m_sourceKey.type < 0)
        return; // No source key selected

    QString text = ui->sourceLineNote->text();
    bool ok;

    TrLine &srcLine = TextDataProcessor::getItemRef(m_project,
                                                    "metadata",
                                                    m_sourceKey.root,
                                                    m_sourceKey.group,
                                                    m_sourceKey.type,
                                                    m_sourceKey.key,
                                                    &ok);

    if(ok)
    {
        srcLine.note = text;
        m_filesStringsModel->syncField(m_sourceKey.lastString, srcLine);

        qDebug() << "Updated note for the source line at " << m_sourceKey.root << "G=" << m_sourceKey.group << "T=" << m_sourceKey.type << "K=" << m_sourceKey.key;
    }
    else
        qWarning() << "Failed to find the source line at " << m_sourceKey.root << "G=" << m_sourceKey.group << "T=" << m_sourceKey.type << "K=" << m_sourceKey.key;
}

void TranslatorMain::updateSourceLineContent()
{
    if(m_sourceKey.root.isEmpty() || m_sourceKey.group < 0 || m_sourceKey.type < 0)
        return; // No source key selected

    QString text = ui->sourceLineRO->toPlainText();

    // TODO: Implement the proper source line edit and synchronization
    // The source line should be aslo changed at the original file itself
}

void TranslatorMain::updateActions()
{
    bool isLoaded = !m_currentPath.isEmpty();
    ui->languagesAdd->setEnabled(isLoaded);
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
            bool singleLevel = d.isFile();
            auto *e = ui->menuOpenRecentProject->addAction(d.fileName());
            QObject::connect(e,
                             static_cast<void(QAction::*)(bool)>(&QAction::triggered),
                             this,
                             [this, r, singleLevel](bool)->void
            {
                openProject(r, singleLevel);
            });
        }
    }

    ui->menuOpenRecentProject->addSeparator();
    ui->menuOpenRecentProject->addAction(ui->actionClearRecentProjects);
}

void TranslatorMain::updateTranslateFields()
{
    ui->sourceLineRO->clear();
    ui->sourceLineNote->clear();

    m_translateFields.clear();

    for(auto k = m_project.begin(); k != m_project.end(); ++k)
    {
        if(k.key() == "metadata")
            continue; // Ignore origin translation

        if(!m_langsListModel->langIsVisible(k.key()))
            continue;

        QSharedPointer<TranslateField> f(new TranslateField(&m_project, ui->translationGroup));
        f->setLang(k.key());
        QObject::connect(f.data(), &TranslateField::textChanged,
                         this,
                         [this](const QString &nt)->void
        {
            ui->previewZone->setText(nt);
        });

        QObject::connect(f.data(), &TranslateField::itemActivated,
                         this,
                         [this](const QString &lang)->void
        {
            if(m_recentLang != lang)
            {
                m_recentLang = lang;
                syncDialoguesListLang();
                syncStringsListStatus();
            }
        });

        QObject::connect(f.data(), &TranslateField::trStateUpdated,
                         this, &TranslatorMain::syncStringsListStatus);

        ui->translationLayout->insertWidget(ui->translationLayout->count() - 1, f.data());
        m_translateFields.insert(k.key(), std::move(f));
    }

    syncTranslationFieldsContent();
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

void TranslatorMain::on_actionAbout_triggered()
{
    about a(this);
    a.setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);
    a.exec();
}

void TranslatorMain::on_languagesAdd_clicked()
{
    if(m_currentPath.isEmpty())
        return; // No opened project

    LangSelect a(this);
    a.setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);
    a.setData(tr("Add new language to project"),
              tr("Please select a language to add into this project."));

    if(a.exec() == QDialog::Accepted)
    {
        QString newLang = a.currentLanguageCode();
        if(newLang.isEmpty())
        {
            QMessageBox::warning(this,
                                 tr("Language selection"),
                                 tr("An invalid language had been selected."),
                                 QMessageBox::Ok);
            return;
        }

        if(m_project.contains(newLang))
        {
            QMessageBox::information(this,
                                     tr("Language selection"),
                                     tr("The selected language %1 is already in the project.").arg(newLang),
                                     QMessageBox::Ok);
            return;
        }

        TextDataProcessor t;
        t.createTranslation(m_project, newLang);

        // m_recentLang = "metadata";
        // ui->previewZone->clearText();
        // m_dialogueItems.clear();
        // m_dialoguesListModel->clear();
        // m_filesStringsModel->clear();
        // m_filesListModel->rebuildView(m_recentPath);
        m_langsListModel->refreshData();
        updateTranslateFields();
    }
}
