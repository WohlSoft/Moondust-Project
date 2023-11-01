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

#include "font_setup_widget.h"
#include <QGridLayout>
#include <QFrame>
#include <QFontDialog>
#include <QColorDialog>
#include <QResizeEvent>
#include <QPainter>
#include <QFontMetrics>
#include <QLineEdit>
#include <QPushButton>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsSimpleTextItem>
#include <QSpinBox>
#include <QLabel>


/*****************************************************************************/

FontSetupColorPreview::FontSetupColorPreview(QWidget* parent) :
    QWidget(parent)
{
    setMinimumSize(16, 16);
}

FontSetupColorPreview::~FontSetupColorPreview()
{}

void FontSetupColorPreview::setColor(QColor color)
{
    m_color = color;
    repaint();
}

QColor FontSetupColorPreview::color() const
{
    return m_color;
}

QSize FontSetupColorPreview::sizeHint() const
{
    QSize s_new = size();
    s_new.setWidth(s_new.height());
    return s_new;
}

void FontSetupColorPreview::resizeEvent(QResizeEvent* event)
{
    QSize s_old = event->oldSize();
    QSize s_new = event->size();
    if(s_old != s_new)
    {
        s_new.setWidth(s_new.height());
        resize(s_new);
    }

    QWidget::resizeEvent(event);
}

void FontSetupColorPreview::paintEvent(QPaintEvent*)
{
    QSize size = this->size();
    int w = size.width(), h = size.height();

    QBrush b = QBrush(m_color);

    QImage img(w, h, QImage::Format_ARGB32_Premultiplied);
    img.fill(0);

    QBrush lg = QBrush(Qt::lightGray);
    QBrush dg = QBrush(Qt::darkGray);
    // QBrush bl = QBrush(Qt::black);
    QPen noPen = QPen(Qt::NoPen);

    QPainter painter(this);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.setPen(noPen);
    painter.setBrush(dg);

    painter.fillRect(0, 0, w, h, lg);
    for(int i = 0; i < h / 4; i++)
    {
        for(int j = 0; j < w / 4; j++)
        {
            painter.drawRect(i * 8, j * 8, 4, 4);
            painter.drawRect(i * 8 + 4, j * 8 + 4, 4, 4);
        }
    }

    painter.fillRect(0, 0, w, h, b);

    painter.setPen(QPen(Qt::black));
    painter.setBrush(QBrush(Qt::NoBrush));
    painter.drawRect(0, 0, w - 1, h - 1);

    painter.end();
}
/*****************************************************************************/


void FontSetupWidget::syncChanges()
{
    m_colourPrev->setColor(m_brush.color());
    m_colourBorderPrev->setColor(m_pen.color());

    m_previewFont->setPen(m_pen);
    m_previewFont->setBrush(m_brush);
    m_previewFont->setFont(m_font);
}

FontSetupWidget::FontSetupWidget(QWidget *parent)
    : QWidget(parent),
    m_font("Times", 25, QFont::Black),
    m_brush(Qt::white),
    m_pen(Qt::black, 2)
{
    QGridLayout *l = new QGridLayout(this);
    setLayout(l);

    // Font Family / Size / Decoration / Weight

    QPushButton *m_changeFont = new QPushButton(tr("Set font..."), this);
    l->addWidget(m_changeFont, 0, 0);

    QObject::connect(m_changeFont, static_cast<void(QPushButton::*)(bool)>(&QPushButton::clicked),
                    this,
                    [this](bool)->void
                    {
                        QFontDialog d(this);
                        d.setCurrentFont(m_font);
                        if(d.exec() == QDialog::Accepted)
                        {
                            m_font = d.currentFont();
                            syncChanges();
                        }
                    });


    // Font colour

    QFrame *m_colour = new QFrame(this);
    QGridLayout *m_colour_l = new QGridLayout(this);
    m_colour->setLayout(m_colour_l);

    m_colourPrev = new FontSetupColorPreview(this);
    m_colourPrev->setColor(m_brush.color());
    m_colour_l->addWidget(m_colourPrev, 0, 0);
    m_colour_l->setSizeConstraint(QLayout::SetMinimumSize);

    QPushButton *m_colourChange = new QPushButton(tr("Set colour..."), this);
    m_colour_l->addWidget(m_colourChange, 0, 1);

    QObject::connect(m_colourChange, static_cast<void(QPushButton::*)(bool)>(&QPushButton::clicked),
                    this,
                    [this](bool)->void
                    {
                        QColorDialog d(this);
                        d.setCurrentColor(m_brush.color());
                        if(d.exec() == QDialog::Accepted)
                        {
                            m_brush.setColor(d.currentColor());
                            syncChanges();
                        }
                    });

    l->addWidget(m_colour, 0, 1);


    // Border colour

    QFrame *m_colourBorder = new QFrame(this);
    QGridLayout *m_colourBorder_l = new QGridLayout(this);
    m_colourBorder->setLayout(m_colourBorder_l);
    m_colourBorder_l->setSizeConstraint(QLayout::SetMinimumSize);

    m_colourBorderPrev = new FontSetupColorPreview(this);
    m_colourBorderPrev->setColor(m_pen.color());
    m_colourBorder_l->addWidget(m_colourBorderPrev, 0, 0);

    QPushButton *m_colourBorderChange = new QPushButton(tr("Set border colour..."), this);
    m_colourBorder_l->addWidget(m_colourBorderChange, 0, 1);

    QObject::connect(m_colourBorderChange, static_cast<void(QPushButton::*)(bool)>(&QPushButton::clicked),
                    this,
                    [this](bool)->void
                    {
                        QColorDialog d(this);
                        d.setCurrentColor(m_pen.color());
                        if(d.exec() == QDialog::Accepted)
                        {
                            m_pen.setColor(d.currentColor());
                            syncChanges();
                        }
                    });

    l->addWidget(m_colourBorder, 0, 2);


    // Font preview

    QLabel *m_previewLabel = new QLabel(tr("Preview:"), this);
    l->addWidget(m_previewLabel, 1, 0, 1, 3);

    m_previewInput = new QLineEdit(this);
    m_previewInput->setText(tr("Example text"));
    l->addWidget(m_previewInput, 2, 0, 1, 3);

    QGraphicsView *previewText = new QGraphicsView(this);
    l->addWidget(previewText, 3, 0, 1, 3);

    QGraphicsScene *previewScene = new QGraphicsScene(previewText);
    previewText->setScene(previewScene);

    m_previewFont = new QGraphicsSimpleTextItem(m_previewInput->text());
    m_previewFont->setFont(m_font);
    m_previewFont->setPen(m_pen);
    m_previewFont->setBrush(m_brush);
    m_previewFont->setBoundingRegionGranularity(1);

    QObject::connect(m_previewInput, &QLineEdit::textChanged,
                    this,
                    [this](const QString &t)->void
                    {
                        m_previewFont->setText(t);
                    });

    previewScene->addItem(m_previewFont);
}

void FontSetupWidget::setData(const QFont& font, const QBrush& brush, const QPen& pen)
{
    m_font = font;
    m_brush = brush;
    m_pen = pen;
    syncChanges();
}

void FontSetupWidget::getData(QFont& font, QBrush& brush, QPen& pen) const
{
    font = m_font;
    brush = m_brush;
    pen = m_pen;
}
