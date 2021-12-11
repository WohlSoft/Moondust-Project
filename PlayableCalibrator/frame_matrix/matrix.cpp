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
#include <QMouseEvent>

#include "matrix.h"
#include <ui_matrix.h>


void Matrix::rebuildGrid()
{
    if(!m_allowEnableDisable)
        return;

    for(auto it = m_conf->frames.begin(); it != m_conf->frames.end(); ++it)
        ui->SpriteMatrix->setFrameEnabled(it.key().first, it.key().second, it.value().used);
}

Matrix::Matrix(Calibration *conf, QWidget *mw, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Matrix)
{
    ui->setupUi(this);
    m_conf = conf;
    m_frameX = 0;
    m_frameY = 0;

    QObject::connect(ui->SpriteMatrix, &MatrixWidget::cellClicked, [this](int x, int y)->void
    {
        setFrame(x, y);
        emit frameSelected(x, y);
    });

    QObject::connect(ui->SpriteMatrix, &MatrixWidget::cellToggled,
                     [this](int x, int y, bool en)->void
    {
        m_conf->frames[{x, y}].used = en;
        if(m_frameX == x && m_frameY == y)
            currentFrameSwitched(en);
        ui->SpriteMatrix->setFrameEnabled(x, y, en);
    });

    QObject::connect(ui->SpriteMatrix, &MatrixWidget::referenceSelected, [this](int x, int y)->void
    {
        emit referenceSelected(x, y);
    });

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
    ui->SpriteMatrix->setFrameEnabled(x, y, enabled);
}

void Matrix::allowEnableDisable(bool en)
{
    m_allowEnableDisable = en;
    rebuildGrid();
}

void Matrix::setFrame(int x, int y)
{
    m_frameX = x;
    m_frameY = y;

    ui->SpriteMatrix->setFrame(x, y);

    if(isModal())
        accept();
}

void Matrix::languageSwitched()
{
    ui->retranslateUi(this);
}
