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

#pragma once
#ifndef CONFIG_PROFILE_SELECTOR_H
#define CONFIG_PROFILE_SELECTOR_H

#include <QDialog>
#include <QStringList>

namespace Ui
{
class ConfigProfileSelect;
}

class QListWidgetItem;

class ConfigProfileSelect : public QDialog
{
    Q_OBJECT

    enum CustomRoles
    {
        RoleIsNew = Qt::UserRole,
        RolePath = Qt::UserRole + 1
    };

    struct ProfileEntry
    {
        QString path;
        QString title;
        QString dir;
        QString icon;
    };

    QString m_selectedItem;
    bool m_isNew = false;

    void addSingleItem(const ProfileEntry &profilePath);

public:
    explicit ConfigProfileSelect(QWidget *parent = nullptr);
    ~ConfigProfileSelect();

    void setItemsList(const QStringList &items);
    void setCurrentItem(const QString &item);
    QString currentItem() const;
    bool isNewProfile() const;

private slots:
    void on_profilesList_itemDoubleClicked(QListWidgetItem *item);
    void on_ConfigProfileSelect_accepted();

private:
    void setCurrentItem(QListWidgetItem *item);

    Ui::ConfigProfileSelect *ui;
};

#endif // CONFIG_PROFILE_SELECTOR_H
