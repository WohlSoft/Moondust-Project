/*
 * Playble Character Calibrator, a free tool for playable character sprite setup tune
 * This is a part of the Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2017-2021 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <QPainter>
#include <QGridLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QSpacerItem>

#include "matrix.h"
#include <ui_matrix.h>


void Matrix::clearGrid()
{
    m_matrixCheckBoxes.clear();
    m_matrixSelectors.clear();
    m_matrixCells.clear();
}

void Matrix::rebuildGrid()
{
    clearGrid();

    //Set data to matrix:
    for(int y = 0; y < m_gridH; ++y)
    {
        for(int x = 0; x < m_gridW; ++x)
        {
            QSharedPointer<QGridLayout> cell(new QGridLayout());
            cell->setMargin(0);
            cell->setSpacing(0);
            cell->addItem(new QSpacerItem(30, 20, QSizePolicy::Minimum, QSizePolicy::Expanding), 0, 0, 2, 3);
            cell->addItem(new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding), 2, 0, 1, 1);

            QSharedPointer<QCheckBox> cb(new QCheckBox);
            cb->setChecked(m_conf->frames[{x, y}].used);
            cb->setToolTip(tr("Include or exclude the %1 x %1 frame from the final calibration file").arg(x).arg(y));
            QCheckBox *s = cb.data();
            QObject::connect(s, static_cast<void (QCheckBox::*)(bool)>(&QCheckBox::clicked),
                             [this, s, x, y](bool en)->void
            {
                if(s->hasFocus())
                {
                    m_conf->frames[{x, y}].used = en;
                    if(m_frameX == x && m_frameY == y)
                        currentFrameSwitched(en);
                }
            });
            cell->addWidget(cb.data(), 2, 2);
            m_matrixCheckBoxes.insert({x, y}, cb);

            QSharedPointer<QPushButton> sb(new QPushButton);
            sb->setFixedSize(20, 20);
            sb->setToolTip(tr("Select the %1 x %1 frame").arg(x).arg(y));
            cell->addWidget(sb.data(), 2, 1);
            m_matrixSelectors.push_back(sb);
            QObject::connect(sb.data(), static_cast<void (QPushButton::*)(bool)>(&QPushButton::clicked),
                             [this, x, y](bool)->void
            {
                setFrame(x, y);
                emit frameSelected(x, y);
            });
            m_matrixCells.push_back(cell);

            auto *w = new QWidget();
            w->setLayout(cell.data());
            m_matrixGrid->addWidget(w, y, x);
        }
    }
}

Matrix::Matrix(Calibration *conf, QWidget *mw, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Matrix)
{
    ui->setupUi(this);
    m_conf = conf;
    m_frameX = 0;
    m_frameY = 0;

    m_matrixGrid = new QGridLayout(ui->SpriteMatrix);
    m_matrixGrid->setSpacing(0);
    m_matrixGrid->setMargin(0);
    ui->SpriteMatrix->setLayout(m_matrixGrid);

    auto *mmw = qobject_cast<CalibrationMain*>(mw);
    Q_ASSERT(mmw);
    updateScene(mmw->m_xImageSprite);
}

Matrix::~Matrix()
{
    delete ui;
}

void Matrix::changeGridSize(int w, int h)
{
    m_gridW = w;
    m_gridH = h;
    ui->SpriteMatrix->setGridSize(w, h);
    rebuildGrid();
}

void Matrix::updateScene(const QPixmap &sprite)
{
    updateGeometry();
    ui->SpriteMatrix->setTexture(sprite, m_gridW, m_gridH);
    qDebug() << "Building a scene: " << ui->SpriteMatrix->width() << ui->SpriteMatrix->height();
    rebuildGrid();
}

void Matrix::setFrameEnabled(int x, int y, bool enabled)
{
    if(m_matrixCheckBoxes.contains({x, y}))
        m_matrixCheckBoxes[{x, y}]->setChecked(enabled);
}

void Matrix::setFrame(int x, int y)
{
    m_frameX = x;
    m_frameY = y;

    ui->SpriteMatrix->setFrame(x, y);

    if(isModal())
        accept();
}
