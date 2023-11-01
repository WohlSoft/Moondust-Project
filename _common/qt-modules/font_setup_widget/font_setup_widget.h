/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2023 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef FONTSETUPWIDGET_H
#define FONTSETUPWIDGET_H

#include <QWidget>
#include <QBrush>
#include <QPen>

class FontSetupColorPreview : public QWidget
{
    Q_OBJECT

    QColor m_color;
public:
    explicit FontSetupColorPreview(QWidget *parent);
    ~FontSetupColorPreview() override;

    void setColor(QColor color);
    QColor color() const;

    virtual QSize sizeHint() const override;

protected:
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *) override;

signals:
    void sizeChanged(QSize newSize);
};

class QLineEdit;
class QGraphicsSimpleTextItem;

class FontSetupWidget : public QWidget
{
    Q_OBJECT
    QFont m_font;
    QBrush m_brush;
    QPen m_pen;

    FontSetupColorPreview *m_colourPrev = nullptr;
    FontSetupColorPreview *m_colourBorderPrev = nullptr;
    QLineEdit *m_previewInput = nullptr;
    QGraphicsSimpleTextItem *m_previewFont = nullptr;

    void syncChanges();

public:
    explicit FontSetupWidget(QWidget *parent = nullptr);

    void setData(const QFont &font, const QBrush &brush, const QPen &pen);
    void getData(QFont &font, QBrush &brush, QPen &pen) const;

signals:
    void dataChanged(const QFont &font, const QBrush &brush, const QPen &pen);
};

#endif // FONTSETUPWIDGET_H
