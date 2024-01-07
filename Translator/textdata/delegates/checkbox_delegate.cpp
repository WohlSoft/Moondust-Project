/*
 * Moondust Translator, a free tool for internationalisation of levels and episodes
 * This is a part of the Moondust Project, a free platform for game making
 * Copyright (c) 2023-2024 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "checkbox_delegate.h"
#include <QCheckBox>
#include <QApplication>

CheckBoxDelegate::CheckBoxDelegate(QObject* parent) : QStyledItemDelegate(parent)
{}

bool CheckBoxDelegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index)
{
    if(event->type() == QEvent::MouseButtonRelease)
    {
        int value = index.data(Qt::CheckStateRole).toInt();

        if((index.flags() & Qt::ItemIsEnabled) != 0) // if enabled
        {
            // invert checkbox state
            model->setData(index, value == Qt::Checked ? Qt::Unchecked : Qt::Checked, Qt::CheckStateRole);
            return true;
        }
    }

    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

QWidget* CheckBoxDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)

    // Creating a checkbox editor
    QCheckBox* editor = new QCheckBox(parent);
    editor->setEnabled((index.flags() & Qt::ItemIsEnabled) != 0);
    return editor;
}

void CheckBoxDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    // Set "selected" or "not selected"
    QCheckBox* cb = qobject_cast<QCheckBox*>(editor);
    cb->setEnabled((index.flags() & Qt::ItemIsEnabled) != 0);
    cb->setChecked(index.data().toInt());
}

void CheckBoxDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    // Write back data into the model
    QCheckBox* cb = static_cast<QCheckBox*>(editor);
    cb->setEnabled((index.flags() & Qt::ItemIsEnabled) != 0);
    int value = cb->checkState();
    model->setData(index, value, Qt::CheckStateRole);
}

void CheckBoxDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    Q_UNUSED(index);
    QStyleOptionButton checkboxstyle;
    QRect checkbox_rect = QApplication::style()->subElementRect(QStyle::SE_CheckBoxIndicator, &checkboxstyle);

    // Aligning to center
    checkboxstyle.rect = option.rect;
    checkboxstyle.rect.setLeft(option.rect.x() +
                               option.rect.width() / 2 - checkbox_rect.width() / 2);

    editor->setGeometry(checkboxstyle.rect);
}


void CheckBoxDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    // Getting data
    bool data = index.model()->data(index, Qt::CheckStateRole).toInt();

    // Making CheckBox style
    QStyleOptionButton checkboxstyle;
    QRect checkbox_rect = QApplication::style()->subElementRect(QStyle::SE_CheckBoxIndicator, &checkboxstyle);

    // Aligning to center
    checkboxstyle.rect = option.rect;
    checkboxstyle.rect.setLeft(option.rect.x() + option.rect.width() / 2 - checkbox_rect.width() / 2);

    // Selected or NOT selected
    if(data)
        checkboxstyle.state = QStyle::State_On;
    else
        checkboxstyle.state = QStyle::State_Off;

    if((index.flags() & Qt::ItemIsEnabled) != 0)
        checkboxstyle.state |= QStyle::State_Enabled;

    // Done! Displaying...
    QApplication::style()->drawControl(QStyle::CE_CheckBox, &checkboxstyle, painter);
}
