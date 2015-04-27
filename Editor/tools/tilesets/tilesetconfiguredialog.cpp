/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2015 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <common_features/app_path.h>
#include <editing/_scenes/level/lvl_scene.h>
#include <editing/_scenes/world/wld_scene.h>
#include "tilesetconfiguredialog.h"
#include <ui_tilesetconfiguredialog.h>

TilesetConfigureDialog::TilesetConfigureDialog(dataconfigs* conf, QGraphicsScene *scene, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TilesetConfigureDialog)
{
    ui->setupUi(this);

    #ifdef Q_OS_MAC
    this->setWindowIcon(QIcon(":/cat_builder.icns"));
    #endif
    #ifdef Q_OS_WIN
    this->setWindowIcon(QIcon(":/cat_builder.ico"));
    #endif

    scn = scene;
    mode = GFX_Staff;
    if(scene!=NULL)
    {
        if(QString(scn->metaObject()->className())=="LvlScene") mode = GFX_Level;
        else
        if(QString(scn->metaObject()->className())=="WldScene") mode = GFX_World;
    }

    ui->customOnly->setVisible(mode!=GFX_Staff);
    ui->specific->setVisible(mode!=GFX_Staff);
    ui->specific->setChecked(mode!=GFX_Staff);
    ui->delete_me->setVisible(false);

    ui->tilesetLayoutWidgetContainer->insertWidget(0,m_tileset = (new tileset(conf,ItemTypes::LVL_Block,0,32,3,3, scn)));

    ui->listView->setAcceptDrops(true);
    ui->listView->setDropIndicatorShown(true);
    ui->listView->setDragEnabled(true);
    ui->listView->setModel(m_model = (new PiecesModel(conf, PiecesModel::LEVELPIECE_BLOCK)));

    m_conf = conf;
    lastFileName = "";

    setUpItems(ItemTypes::LVL_Block);

    connect(ui->spin_width,SIGNAL(valueChanged(int)),m_tileset,SLOT(setCols(int)));
    connect(ui->spin_height,SIGNAL(valueChanged(int)),m_tileset,SLOT(setRows(int)));
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)),this,SLOT(setUpItems(int)));
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)),this,SLOT(setUpTileset(int)));
    connect(ui->TilesetName, SIGNAL(textChanged(QString)), m_tileset, SLOT(setName(QString)));

    connect(this, SIGNAL(windowShowed()), SLOT(showNotify()), Qt::QueuedConnection);
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(update()));
    connect(ui->spin_width, SIGNAL(currentIndexChanged(int)), this, SLOT(update()));
    connect(ui->spin_height, SIGNAL(currentIndexChanged(int)), this, SLOT(update()));
}

TilesetConfigureDialog::~TilesetConfigureDialog()
{
    delete ui;
}

void TilesetConfigureDialog::on_clearTileset_clicked()
{
    int x=QMessageBox::question(this, tr("Clean tileset editor"),
                          tr("Do you want to clean tileset editor to create a new tileset?"),
                          QMessageBox::Yes|QMessageBox::No);
    if(x==QMessageBox::Yes)
    {
        m_tileset->clear();
        lastFileName="";
        lastFullPath="";
        ui->TilesetName->setText("");
        ui->delete_me->setVisible(false);
    }
}

//void TilesetConfigureDialog::setUpItems(int type)
//{
//    setUpItems(type);
//}

void TilesetConfigureDialog::setUpTileset(int type)
{
    m_tileset->clear();
    m_tileset->setType(type);
}

void TilesetConfigureDialog::setUpItems(int type)
{
    bool custom = ( (mode!=GFX_Staff) && (ui->customOnly->isChecked()) );

    delete m_model;
    ui->listView->setModel(m_model = (new PiecesModel(m_conf, toPieceType(type), 32, scn)));

    switch (type)
    {
    case ItemTypes::LVL_Block:
    {
        ui->listView->setViewMode(QListView::ListMode);
        if(custom)
        {
            if(mode==GFX_Level)
                for(int i = 0; i < dynamic_cast<LvlScene *>(scn)->uBlocks.size(); ++i){
                    m_model->addPiece(
                                dynamic_cast<LvlScene *>(scn)->index_blocks[
                                dynamic_cast<LvlScene *>(scn)->uBlocks[i].id].i
                                      );
                }

        }
        else
        for(int i = 0; i < m_conf->main_block.size(); ++i){
            m_model->addPiece(i);
        }
        break;
    }
    case ItemTypes::LVL_BGO:
    {
        ui->listView->setViewMode(QListView::ListMode);
        if(custom)
        {
            if(mode==GFX_Level)
                for(int i = 0; i < dynamic_cast<LvlScene *>(scn)->uBGOs.size(); ++i){
                    m_model->addPiece(
                                dynamic_cast<LvlScene *>(scn)->index_bgo[
                                dynamic_cast<LvlScene *>(scn)->uBGOs[i].id].i
                                      );
                }

        }
        else
        for(int i = 0; i < m_conf->main_bgo.size(); ++i){
            m_model->addPiece(i);
        }
        break;
    }
    case ItemTypes::LVL_NPC:
    {
        ui->listView->setViewMode(QListView::ListMode);
        if(custom)
        {
            if(mode==GFX_Level)
                for(int x=0; x < dynamic_cast<LvlScene *>(scn)->index_npc.size(); x++)
                {
                    if(dynamic_cast<LvlScene *>(scn)->index_npc[x].type==1)
                        m_model->addPiece( dynamic_cast<LvlScene *>(scn)->index_npc[x].gi-1);
                }
        }
        else
        for(int i = 0; i < m_conf->main_npc.size(); ++i){
            m_model->addPiece(i);
        }
        break;
    }
    case ItemTypes::WLD_Tile:
    {
        ui->listView->setViewMode(QListView::IconMode);
        if(custom)
        {
            if(mode==GFX_World)
                for(int i = 0; i < dynamic_cast<WldScene *>(scn)->uTiles.size(); ++i){
                    m_model->addPiece(
                                dynamic_cast<WldScene *>(scn)->index_tiles[
                                dynamic_cast<WldScene *>(scn)->uTiles[i].id].i
                                      );
                }

        }
        else
        for(int i = 0; i < m_conf->main_wtiles.size(); ++i){
            m_model->addPiece(i);
        }
        break;
    }
    case ItemTypes::WLD_Scenery:
        ui->listView->setViewMode(QListView::IconMode);
        if(custom)
        {
            if(mode==GFX_World)
                for(int i = 0; i < dynamic_cast<WldScene *>(scn)->uScenes.size(); ++i){
                    m_model->addPiece(
                                dynamic_cast<WldScene *>(scn)->index_scenes[
                                dynamic_cast<WldScene *>(scn)->uScenes[i].id].i
                                      );
                }

        }
        else
        for(int i = 0; i < m_conf->main_wscene.size(); ++i){
            m_model->addPiece(i);
        }
        break;
    case ItemTypes::WLD_Path:
        ui->listView->setViewMode(QListView::IconMode);
        if(custom)
        {
            if(mode==GFX_World)
                for(int i = 0; i < dynamic_cast<WldScene *>(scn)->uPaths.size(); ++i){
                    m_model->addPiece(
                                dynamic_cast<WldScene *>(scn)->index_paths[
                                dynamic_cast<WldScene *>(scn)->uPaths[i].id].i
                                      );
                }

        }
        else
        for(int i = 0; i < m_conf->main_wpaths.size(); ++i){
            m_model->addPiece(i);
        }
        break;
    case ItemTypes::WLD_Level:
        ui->listView->setViewMode(QListView::IconMode);
        if(custom)
        {
            if(mode==GFX_World)
                for(int i = 0; i < dynamic_cast<WldScene *>(scn)->uLevels.size(); ++i){
                    m_model->addPiece(
                                dynamic_cast<WldScene *>(scn)->index_levels[
                                dynamic_cast<WldScene *>(scn)->uLevels[i].id].i
                                      );
                }

        }
        else
        for(int i = 0; i < m_conf->main_wlevels.size(); ++i){
            m_model->addPiece(i);
        }
        break;
    default:
        break;
    }

}

PiecesModel::PieceType TilesetConfigureDialog::toPieceType(int type)
{
    return static_cast<PiecesModel::PieceType>(type);
}

void TilesetConfigureDialog::on_SaveTileset_clicked()
{
    QDir(m_conf->config_dir).mkpath("tilesets/");

    bool ok;
    QString fileName = QInputDialog::getText(this, tr("Please enter a filename!"),
                                              tr("Filename:"), QLineEdit::Normal,
                                              lastFileName.isEmpty()?m_tileset->name():lastFileName, &ok);
    if (!ok || fileName.isEmpty())
        return;

    lastFileName = fileName;

    if(!fileName.endsWith(".tileset.ini"))
        fileName += ".tileset.ini";

    QString savePath;
    switch(mode)
    {
        case GFX_Level:
            savePath = dynamic_cast<LvlScene *>(scn)->LvlData->path+
                    (ui->specific->isChecked()? "/"+dynamic_cast<LvlScene *>(scn)->LvlData->filename :"")+"/";
            break;
        case GFX_World:
            savePath = dynamic_cast<WldScene *>(scn)->WldData->path+
                    (ui->specific->isChecked()? "/"+dynamic_cast<WldScene *>(scn)->WldData->filename :"")+"/";
            break;
        default:
            savePath = m_conf->config_dir + "tilesets/";
    }

    QDir target(savePath);
    if(!target.exists()) target.mkpath(savePath);

    tileset::SaveSimpleTileset( savePath + fileName, m_tileset->toSimpleTileset());

    lastFullPath = QFileInfo(savePath + fileName).absoluteFilePath();
    ui->delete_me->setVisible(true);
}

void TilesetConfigureDialog::on_OpenTileset_clicked()
{

    QString openPath;
    switch(mode)
    {
        case GFX_Level:
            openPath = dynamic_cast<LvlScene *>(scn)->LvlData->path+"/";
            break;
        case GFX_World:
            openPath = dynamic_cast<WldScene *>(scn)->WldData->path+"/";
            break;
        default:
            openPath = m_conf->config_dir + "tilesets/";
    }

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Tileset"),
                                                    openPath, QString("PGE Tileset (*.tileset.ini)"));
    if (fileName.isEmpty())
        return;

    openTileset(fileName, ui->customOnly->isChecked());

}

void TilesetConfigureDialog::openTileset(QString filePath, bool isCustom)
{
    if (filePath.isEmpty())
        return;

    lastFileName = QFileInfo(filePath).baseName();
    lastFullPath = QFileInfo(filePath).absoluteFilePath();

    ui->delete_me->setVisible(false);

    SimpleTileset simple;
    if(!tileset::OpenSimpleTileset(filePath,simple)){
        QMessageBox::warning(this, tr("Failed to load tileset!"), tr("Failed to load tileset!\nData may be corrupted!"));
    }else{
        ui->TilesetName->setText(simple.tileSetName);
        ui->spin_width->setValue(simple.cols);
        ui->spin_height->setValue(simple.rows);
        ui->comboBox->setCurrentIndex(static_cast<int>(simple.type));
        setUpItems(simple.type);
        m_tileset->loadSimpleTileset(simple);
    }

    ui->specific->setChecked(isCustom);
    ui->delete_me->setVisible(true);
}

void TilesetConfigureDialog::loadSimpleTileset(const SimpleTileset &tileset, bool isCustom){
    ui->TilesetName->setText(tileset.tileSetName);
    ui->spin_width->setValue(tileset.cols);
    ui->spin_height->setValue(tileset.rows);
    ui->comboBox->setCurrentIndex(static_cast<int>(tileset.type));
    setUpItems(tileset.type);
    m_tileset->loadSimpleTileset(tileset);
    lastFileName = QString(tileset.fileName).remove(".tileset.ini");
    lastFullPath = QFileInfo(tileset.fileName).absoluteFilePath();

    ui->specific->setChecked(isCustom);
    ui->delete_me->setVisible(true);
}


void TilesetConfigureDialog::on_customOnly_clicked()
{
    if(mode==GFX_Staff) return;
    setUpItems(ui->comboBox->currentIndex());

}

void TilesetConfigureDialog::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);
    qApp->processEvents();
    emit windowShowed();
}

void TilesetConfigureDialog::showNotify()
{
    QSettings cCounters(AppPathManager::settingsFile(), QSettings::IniFormat);
    cCounters.setIniCodec("UTF-8");
    cCounters.beginGroup("message-boxes");
    bool showNotice=cCounters.value("tileset-editor-greeting",true).toBool();
    if((mode==GFX_Staff) && (showNotice))
    {
        QMessageBox msg;
        msg.setWindowTitle(tr("Tileset box editor"));
        msg.setWindowIcon(this->windowIcon());
        QCheckBox box;
        box.setText(tr("Don't show this message again."));
        msg.setCheckBox(&box);
        msg.setText(tr("Welcome to tileset editor!\n\nThis is an editor of global tilesets.\n"
                       "All tilesets which made here will be saved in this folder:\n%1\n"
                       "I.e. there are will work globally for this configuration package and can be used in the gropus of tilesets.\n\n"
                       "If you wish to create level/world specific tilesets with using of custom graphics, "
                       "please open the Tileset Item Box and find the button \"New Tileset\" "
                       "in the \"Custom\" tab.")
                       .arg(m_conf->config_dir + "tilesets/"));
        msg.setStandardButtons(QMessageBox::Ok);
        msg.exec();
        showNotice = !box.isChecked();
    }
    cCounters.setValue("tileset-editor-greeting",showNotice);
    cCounters.endGroup();
}

void TilesetConfigureDialog::on_delete_me_clicked()
{\
    int x=QMessageBox::question(this, tr("Remove tileset"),
                          tr("Do you want to remove this tileset?"),
                          QMessageBox::Yes|QMessageBox::No);

    if(x==QMessageBox::Yes)
    {
        QFile(lastFullPath).remove();
        QMessageBox::information(this, tr("Tileset removed"), tr("Tileset has been removed!"), QMessageBox::Ok);
        this->close();
    }
}
