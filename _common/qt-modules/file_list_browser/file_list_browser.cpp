/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2021 Vitaly Novichkov <admin@wohlnet.ru>
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
#include <ui_file_list_browser.h>

#include <QtConcurrent>

FileListBrowser::FileListBrowser(QString searchDirectory, QString currentFile, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FileListBrowser)
{
    m_filters.append("*.*");
    m_parentDirectory = searchDirectory;
    m_lastCurrentFile = currentFile;
    ui->setupUi(this);
    connect(this, &FileListBrowser::itemAdded, this, &FileListBrowser::addItem);
    connect(this, &FileListBrowser::digFinished, this, &FileListBrowser::finalizeDig);
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
    fileWalker = QtConcurrent::run(this, &FileListBrowser::buildFileList);
}

QString FileListBrowser::currentFile()
{
    return m_currentFile;
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

void FileListBrowser::addItem(QString item)
{
    ui->FileList->addItem(item);
    if(m_lastCurrentFile == item)
    {
        QList<QListWidgetItem *> list = ui->FileList->findItems(item, Qt::MatchFixedString);
        if(!list.isEmpty())
        {
            list.first()->setSelected(true);
            ui->FileList->scrollToItem(list.first());
        }
    }
}

void FileListBrowser::finalizeDig()
{
    ui->FileList->sortItems(Qt::AscendingOrder);
    QList<QListWidgetItem *> list = ui->FileList->findItems(m_lastCurrentFile, Qt::MatchFixedString);
    if(!list.isEmpty())
    {
        list.first()->setSelected(true);
        ui->FileList->scrollToItem(list.first());
    }
    setCursor(Qt::ArrowCursor);
}

void FileListBrowser::on_FileList_itemDoubleClicked(QListWidgetItem *item)
{
    m_currentFile = item->text();
    if(fileWalker.isRunning())
        fileWalker.cancel();
    accept();
}

void FileListBrowser::on_buttonBox_accepted()
{
    foreach(QListWidgetItem *container, ui->FileList->selectedItems())
        m_currentFile = container->text();
    if(m_currentFile != "")
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
