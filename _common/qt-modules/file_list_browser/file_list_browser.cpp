/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2024 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <QDir>

#include "file_list_browser.h"
#include "file_list_model.h"
#include <ui_file_list_browser.h>

#include <QtConcurrent>

FileListBrowser::FileListBrowser(QString searchDirectory, QString currentFile, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FileListBrowser)
{
    m_filters.append("*.*");
    m_parentDirectory = searchDirectory;
    m_lastCurrentFile = currentFile;

    if(!m_parentDirectory.endsWith('/'))
        m_parentDirectory.append('/');

    int args = m_lastCurrentFile.indexOf('|');
    if(args >= 0) // Remove possible music arguments
        m_lastCurrentFile.remove(args, (m_lastCurrentFile.size() - args));

    ui->setupUi(this);
    // Hide when there is no content
    ui->extraWidget->setVisible(false);

    m_listModel = new FileListModel(this);
    ui->FileList->setModel(m_listModel);
    m_listModel->setDirectory(m_parentDirectory);
    QObject::connect(ui->FileList, &QListView::doubleClicked, this,
                     &FileListBrowser::fileListItem_doubleClicked);

    QObject::connect(ui->FileList->selectionModel(), &QItemSelectionModel::currentChanged,
                     this, [this](const QModelIndex &selected, const QModelIndex &)->void
    {
        if(!selected.isValid())
            return;

        emit itemSelected(m_listModel->data(selected, Qt::DisplayRole).toString());
    });

    QObject::connect(this, &FileListBrowser::itemAdded, this, &FileListBrowser::addItem);
    QObject::connect(this, &FileListBrowser::digFinished, this, &FileListBrowser::finalizeDig);

    QObject::connect(ui->filterLine, &QLineEdit::textChanged, this, &FileListBrowser::filterUpdated);
}

FileListBrowser::~FileListBrowser()
{
    if(fileWalker.isRunning())
        fileWalker.cancel();
    fileWalker.waitForFinished();
    delete ui;
}

void FileListBrowser::setDescription(const QString &description)
{
    ui->label->setText(description);
}

void FileListBrowser::setFilters(const QStringList &filters)
{
    m_filters = filters;
}

void FileListBrowser::setIcon(const QIcon &icon)
{
    setWindowIcon(icon);
}

void FileListBrowser::startListBuilder()
{
    setCursor(Qt::BusyCursor);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    fileWalker = QtConcurrent::run(this, &FileListBrowser::buildFileList);
#else
    fileWalker = QtConcurrent::run(&FileListBrowser::buildFileList, this);
#endif
}

void FileListBrowser::setExtraWidgetLayout(QLayout *layout)
{
    ui->extraWidget->setVisible(true);
    ui->extraWidget->setLayout(layout);
    updateGeometry();
}

QString FileListBrowser::currentFile()
{
    return m_currentFile;
}

QString FileListBrowser::currentSelectedFile()
{
    for(const QModelIndex &it : ui->FileList->selectionModel()->selectedRows())
        return m_listModel->data(it, Qt::DisplayRole).toString();

    return QString();
}

void FileListBrowser::buildFileList()
{
    QDir musicDir(m_parentDirectory);
    musicDir.setSorting(QDir::Name);
    musicDir.setNameFilters(m_filters);
    QDirIterator dirsList(m_parentDirectory, m_filters,
                          QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot,
                          QDirIterator::Subdirectories);

    while(dirsList.hasNext())
    {
        dirsList.next();
        emit itemAdded(musicDir.relativeFilePath(dirsList.filePath()));
        if(fileWalker.isCanceled())
            break;
    }
    digFinished();
}

void FileListBrowser::addItem(const QString &item)
{
    m_listModel->addEntry(item);
    if(m_lastCurrentFile == item)
    {
        QModelIndex it = m_listModel->index(m_listModel->rowCount(QModelIndex()) - 1);
        if(it.isValid())
        {
            ui->FileList->selectionModel()->select(it, QItemSelectionModel::Select);
            ui->FileList->scrollTo(it);
        }
    }
}

void FileListBrowser::finalizeDig()
{
    m_listModel->sortData();
    int num = m_listModel->rowCount(QModelIndex());
    for(int i = 0; i < num; ++i)
    {
        QModelIndex it = m_listModel->index(i);
        if(it.isValid() && m_listModel->data(it, Qt::DisplayRole).toString().compare(m_lastCurrentFile, Qt::CaseInsensitive) == 0)
        {
            ui->FileList->selectionModel()->select(it, QItemSelectionModel::Select);
            ui->FileList->scrollTo(it);
            break;
        }
    }

    setCursor(Qt::ArrowCursor);
}

void FileListBrowser::fileListItem_doubleClicked(const QModelIndex &item)
{
    m_currentFile = m_listModel->data(item, Qt::DisplayRole).toString();
    if(fileWalker.isRunning())
        fileWalker.cancel();
    accept();
}

void FileListBrowser::on_buttonBox_accepted()
{
    for(const QModelIndex &it : ui->FileList->selectionModel()->selectedRows())
    {
        m_currentFile = m_listModel->data(it, Qt::DisplayRole).toString();
        break;
    }

    if(!m_currentFile.isEmpty())
    {
        if(fileWalker.isRunning())
            fileWalker.cancel();
        accept();
    }
}

void FileListBrowser::on_buttonBox_rejected()
{
    if(fileWalker.isRunning())
        fileWalker.cancel();
    reject();
}

void FileListBrowser::filterUpdated(const QString &filter)
{
    m_listModel->setFilter(filter);
}

QString FileListBrowser::directoryPath() const
{
    return m_parentDirectory;
}
