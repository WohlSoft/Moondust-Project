/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "file_list_browser.h"

#include <QDir>
#include <QListView>
#include <QLabel>
#include <QToolButton>
#include <QComboBox>

FileListBrowser::FileListBrowser(QString searchDirectory, QString curFile, QWidget *parent) :
    QObject(parent),
    m_dialog(parent)
{
    m_dialog.setWindowIcon(QIcon(":/images/playmusic.png"));
    QListView *sidebar = m_dialog.findChild<QListView *>("sidebar");
    QComboBox *lookInCombo = m_dialog.findChild<QComboBox *>("lookInCombo");
    m_upButton = m_dialog.findChild<QToolButton*>("toParentButton");
    if(sidebar)
        sidebar->setVisible(false);
    if(lookInCombo)
        lookInCombo->setEnabled(false);

    m_dialog.setWindowIcon(QIcon(":/images/new.png"));
    m_dialog.setFileMode(QFileDialog::ExistingFile);
    m_dialog.setOption(QFileDialog::ReadOnly, true);
    m_dialog.setOption(QFileDialog::DontUseSheet, true);
    m_dialog.setOption(QFileDialog::DontUseNativeDialog, true);
    m_dialog.setOption(QFileDialog::HideNameFilterDetails, true);
    m_dialog.setViewMode(QFileDialog::List);
    m_dialog.setAcceptMode(QFileDialog::AcceptOpen);

    if(searchDirectory.endsWith('/'))
        searchDirectory.remove(searchDirectory.size() - 1, 1);

    m_currentFile = curFile;
    setDirectoryRoot(searchDirectory);

    QObject::connect(&m_dialog, &QFileDialog::directoryEntered,
                     this, &FileListBrowser::directoryEntered);
}

FileListBrowser::~FileListBrowser()
{}

void FileListBrowser::setDirectoryRoot(const QString &root)
{
    m_currentRoot = root;
    m_currentRelation = root;

    if(m_currentFile.isEmpty())
    {
        m_dialog.setDirectory(m_currentRoot);
        if(m_upButton)
            m_upButton->setEnabled(false);
    }
    else
    {
        QFileInfo fPath(root, m_currentFile);
        m_dialog.setDirectory(fPath.absoluteDir());
        m_dialog.selectFile(fPath.fileName());
        if(m_upButton)
            m_upButton->setEnabled(fPath.absoluteDir().absolutePath() != QDir(m_currentRoot).absolutePath());
    }
}

void FileListBrowser::setDirectoryRelation(const QString &rel)
{
    QString r = rel;
    if(r.endsWith('/'))
        r.remove(rel.size() - 1, 1);
    m_currentRelation = r;
    if(m_currentFile.isEmpty())
    {
        m_dialog.setDirectory(r);
        if(m_upButton)
            m_upButton->setEnabled(QDir(r).absolutePath() != QDir(m_currentRoot).absolutePath());
    }
    else
    {
        QFileInfo fPath(r, m_currentFile);
        m_dialog.setDirectory(fPath.absoluteDir());
        m_dialog.selectFile(fPath.fileName());
    }
}

void FileListBrowser::setFilters(const QString &filterName, const QStringList &filters)
{
    if(filters.isEmpty())
        m_dialog.setNameFilter(QString("%1 (*.*)").arg(tr("All files")));
    else
        m_dialog.setNameFilter(QString("%1 (%2)").arg(filterName).arg(filters.join(" ")) + ";;" +
                               QString("%1 (*.*)").arg(tr("All files")));
}

void FileListBrowser::setIcon(const QIcon &icon)
{
    m_dialog.setWindowIcon(icon);
}

void FileListBrowser::setWindowTitle(const QString &title)
{
    m_dialog.setWindowTitle(title);
}

QString FileListBrowser::currentFile()
{
    return m_currentFile;
}

int FileListBrowser::exec()
{
    int ret = m_dialog.exec();
    for(QString &f : m_dialog.selectedFiles())
    {
        QDir root(m_currentRelation);
        m_currentFile = root.relativeFilePath(f);
    }
    return ret;
}

void FileListBrowser::directoryEntered(const QString &directory)
{
    QDir dir_o(directory);
    QDir dir_r(m_currentRoot);

    if(!dir_o.absolutePath().startsWith(dir_r.absolutePath()))
        m_dialog.setDirectory(m_currentRoot);

    if(m_upButton)
        m_upButton->setEnabled(dir_o.absolutePath() != dir_r.absolutePath());
}
