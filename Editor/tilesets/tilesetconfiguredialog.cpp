/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "tilesetconfiguredialog.h"
#include "ui_tilesetconfiguredialog.h"

TilesetConfigureDialog::TilesetConfigureDialog(dataconfigs* conf, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TilesetConfigureDialog)
{
    ui->setupUi(this);
    ui->tilesetLayoutWidgetContainer->insertWidget(0,m_tileset = (new tileset(conf,tileset::LEVELTILESET_BLOCK,0,64,3,3)));

    ui->listView->setAcceptDrops(true);
    ui->listView->setDropIndicatorShown(true);
    ui->listView->setDragEnabled(true);
    ui->listView->setModel(m_model = (new PiecesModel(conf, PiecesModel::LEVELPIECE_BLOCK)));

    m_conf = conf;
    setUpItems(tileset::LEVELTILESET_BLOCK);

    connect(ui->spin_width,SIGNAL(valueChanged(int)),m_tileset,SLOT(setCols(int)));
    connect(ui->spin_height,SIGNAL(valueChanged(int)),m_tileset,SLOT(setRows(int)));
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)),this,SLOT(setUpItems(int)));
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)),this,SLOT(setUpTileset(int)));
    connect(ui->TilesetName, SIGNAL(textChanged(QString)), m_tileset, SLOT(setName(QString)));
}

TilesetConfigureDialog::~TilesetConfigureDialog()
{
    delete ui;
}

void TilesetConfigureDialog::on_pushButton_clicked()
{
    m_tileset->clear();
}

void TilesetConfigureDialog::setUpItems(int type)
{
    setUpItems(static_cast<tileset::TilesetType>(type));
}

void TilesetConfigureDialog::setUpTileset(int type)
{
    m_tileset->clear();
    m_tileset->setType(static_cast<tileset::TilesetType>(type));
}

void TilesetConfigureDialog::setUpItems(tileset::TilesetType type)
{
    delete m_model;
    ui->listView->setModel(m_model = (new PiecesModel(m_conf, toPieceType(type))));

    switch (type) {
    case tileset::LEVELTILESET_BLOCK:
    {
        for(int i = 0; i < m_conf->main_block.size(); ++i){
            m_model->addPiece(i);
        }
        break;
    }
    case tileset::LEVELTILESET_BGO:
    {
        for(int i = 0; i < m_conf->main_bgo.size(); ++i){
            m_model->addPiece(i);
        }
        break;
    }
    case tileset::LEVELTILESET_NPC:
    {
        for(int i = 0; i < m_conf->main_npc.size(); ++i){
            m_model->addPiece(i);
        }
        break;
    }
    case tileset::WORLDTILESET_TILE:
    {
        for(int i = 0; i < m_conf->main_wtiles.size(); ++i){
            m_model->addPiece(i);
        }
        break;
    }
    case tileset::WORLDTILESET_PATH:
        for(int i = 0; i < m_conf->main_wpaths.size(); ++i){
            m_model->addPiece(i);
        }
        break;
    case tileset::WORLDTILESET_SCENERY:
        for(int i = 0; i < m_conf->main_wscene.size(); ++i){
            m_model->addPiece(i);
        }
        break;
    case tileset::WORLDTILESET_LEVEL:
        for(int i = 0; i < m_conf->main_wlevels.size(); ++i){
            m_model->addPiece(i);
        }
        break;
    default:
        break;
    }

}

PiecesModel::PieceType TilesetConfigureDialog::toPieceType(tileset::TilesetType type)
{
    return static_cast<PiecesModel::PieceType>(static_cast<int>(type));
}

void TilesetConfigureDialog::on_SaveTileset_clicked()
{
    QDir(m_conf->config_dir).mkpath("tilesets/");

//    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Tileset"),
//            m_conf->config_dir, QString("PGE Tileset (*.ini)"));

    bool ok;
    QString fileName = QInputDialog::getText(this, tr("Please enter a filename!"),
                                              tr("Filename:"), QLineEdit::Normal,
                                              m_tileset->name(), &ok);
    if (!ok || fileName.isEmpty())
        return;

    if(!fileName.endsWith(".ini"))
        fileName += ".ini";

    tileset::SaveSimpleTileset(m_conf->config_dir + "tilesets/" + fileName,m_tileset->toSimpleTileset());
}

void TilesetConfigureDialog::on_OpenTileset_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Tileset"),
                                                    m_conf->config_dir + "tilesets/",QString("PGE Tileset (*.ini)"));
    if (fileName.isEmpty())
        return;

    tileset::SimpleTileset simple;
    if(!tileset::OpenSimpleTileset(fileName,simple)){
        QMessageBox::warning(this, tr("Failed to load tileset!"), tr("Failed to load tileset!\nData may be corrupted!"));
    }else{
        ui->TilesetName->setText(simple.tileSetName);
        ui->spin_width->setValue(simple.cols);
        ui->spin_height->setValue(simple.rows);
        ui->comboBox->setCurrentIndex(static_cast<int>(simple.type));
        setUpItems(simple.type);
        m_tileset->loadSimpleTileset(simple);
    }
}
