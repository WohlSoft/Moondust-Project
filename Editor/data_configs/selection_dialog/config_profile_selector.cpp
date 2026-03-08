/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2026 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <QSettings>
#include <QIcon>
#include <QMultiMap>
#include <QFileInfo>
#include <QListWidgetItem>
#include <QtDebug>
#include <QMessageBox>

#include "config_profile_selector.h"
#include "ui_config_profile_selector.h"
#include <common_features/util.h>


ConfigProfileSelect::ConfigProfileSelect(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigProfileSelect)
{
    ui->setupUi(this);
}

ConfigProfileSelect::~ConfigProfileSelect()
{
    util::memclear(ui->profilesList);
    delete ui;
}

void ConfigProfileSelect::addSingleItem(const ProfileEntry &profile)
{
    QListWidgetItem *item = new QListWidgetItem(ui->profilesList);
    item->setText(profile.title);
    item->setToolTip(profile.dir);

    if(!profile.icon.isEmpty())
    {
        QPixmap p;
        if(p.load(profile.icon))
            item->setIcon(p);
    }

    item->setData(RoleIsNew, false);
    item->setData(RolePath, profile.path);
}

void ConfigProfileSelect::setItemsList(const QStringList &items)
{
    QListWidgetItem *item;

    m_selectedItem.clear();
    util::memclear(ui->profilesList);

    // TODO: Move entire profiles list outside, to DataConfigs,
    // and make it ensure no duplicated entries added (no profiles with matching path)
    // and auto-delete profiles that has invalid path (moved or deleted)
    // Also, implement the mechanics of auto-recognised profiles by config-side implemented
    // script that searches known paths for existing profiles to let user
    // not bother to select them manually.
    // Also, this global list can be used to build the nice config menu that can quickly switch
    // between proviled without config reboot.
    QMultiMap<QString, ProfileEntry> profilesSorted;
    QSet<QString> profilesAvailable;

    foreach(const QString &item, items)
    {
        ProfileEntry entry;
        QSettings profile(item, QSettings::IniFormat);
        QFileInfo pathInfo;

        entry.path = item;

        profile.beginGroup("main");
        entry.dir = profile.value("application-path", QString()).toString();
        entry.title = profile.value("application-title", QString()).toString();
        entry.icon = profile.value("application-icon", QString()).toString();
        profile.endGroup();

        if(entry.dir.isEmpty())
        {
            qWarning() << "Rejected invalid profile:" << item << "(Path is empty)";
            continue;
        }

        if(profilesAvailable.contains(entry.dir))
        {
            qWarning() << "Rejected duplicated profile:" << item;
            QFile::remove(item); // Remove it just in a case!
            continue;
        }

        pathInfo.setFile(entry.dir);

        if(!pathInfo.exists() || !pathInfo.isDir())
        {
            qWarning() << "Rejected invalid profile:" << item << "(Directory is not exists)";
            return; // Invalid path!
        }

        if(entry.title.isEmpty())
            entry.title = pathInfo.baseName();
        else
            entry.title = QString("%1 (%2)").arg(entry.title, pathInfo.baseName());

        profilesSorted.insert(entry.title, entry);
        profilesAvailable.insert(entry.dir);
    }

    foreach(const ProfileEntry &item, profilesSorted)
        addSingleItem(item);

    item = new QListWidgetItem(ui->profilesList);
    item->setText(tr("<New profile>"));
    item->setToolTip(tr("Create new integration profile"));
    item->setIcon(QIcon(":/dock/pencil_16x16.png"));
    item->setData(RoleIsNew, true);
}

void ConfigProfileSelect::setCurrentItem(const QString &item)
{
    int items = ui->profilesList->count();

    m_selectedItem.clear();

    for(int i = 0; i < items; ++i)
    {
        auto *it = ui->profilesList->item(i);
        if(it->data(RolePath).toString() == item)
        {
            m_selectedItem = item;
            ui->profilesList->clearSelection();
            it->setSelected(true);
            break;
        }
    }
}

QString ConfigProfileSelect::currentItem() const
{
    return m_selectedItem;
}

bool ConfigProfileSelect::isNewProfile() const
{
    return m_isNew;
}

void ConfigProfileSelect::on_profilesList_itemDoubleClicked(QListWidgetItem *item)
{
    setCurrentItem(item);
    accept();
}

void ConfigProfileSelect::on_ConfigProfileSelect_accepted()
{
    auto list = ui->profilesList->selectedItems();

    if(list.isEmpty())
    {
        QMessageBox::information(this, tr("No selected profile"), tr("Please select profile from the list."));
        reject();
        return;
    }

    setCurrentItem(list.first());
}

void ConfigProfileSelect::setCurrentItem(QListWidgetItem *item)
{
    Q_ASSERT(item);

    m_isNew = item->data(RoleIsNew).toBool();
    m_selectedItem = item->data(RolePath).toString();
}
