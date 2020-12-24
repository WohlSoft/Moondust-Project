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

#include <QDir>
#include <QFileInfo>
#include <QFileDialog>
#include <QInputDialog>
#include <QKeyEvent>

#include <common_features/app_path.h>
#include <common_features/util.h>
#include <editing/_scenes/level/lvl_scene.h>
#include <editing/_scenes/world/wld_scene.h>
#include "tileset_editor.h"
#include <ui_tileset_editor.h>

#include "qfile_dialogs_default_options.hpp"


TilesetEditor::TilesetEditor(DataConfig *conf, QGraphicsScene *scene, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TilesetEditor)
{
    ui->setupUi(this);

#ifdef Q_OS_MAC
    this->setWindowIcon(QIcon(":/cat_builder.icns"));
#endif
#ifdef Q_OS_WIN
    this->setWindowIcon(QIcon(":/cat_builder.ico"));
#endif

    scn = scene;
    m_mode = GFX_Staff;
    if(scene != nullptr)
    {
        if(QString(scn->metaObject()->className()) == "LvlScene")
            m_mode = GFX_Level;
        else if(QString(scn->metaObject()->className()) == "WldScene")
            m_mode = GFX_World;
    }

    ui->customOnly->setVisible(m_mode != GFX_Staff);
    ui->defaultOnly->setVisible(m_mode != GFX_Staff);
    ui->specific->setVisible(m_mode != GFX_Staff);
    ui->specific->setChecked(m_mode != GFX_Staff);
    ui->delete_me->setVisible(false);

    ui->tilesetLayoutWidgetContainer->insertWidget(0, m_tileset = (new tileset(conf, ItemTypes::LVL_Block, nullptr, 32, 3, 3, scn)));

    m_model = new ElementsListModel(conf, ElementsListModel::LEVELPIECE_BLOCK, 32, nullptr, this);
    ui->listView->setModel(m_model);

    m_conf = conf;
    lastFileName = "";

    oldWidth = ui->spin_width->value();
    oldHeight = ui->spin_height->value();

    setUpItems(ItemTypes::LVL_Block);

    connect(ui->spin_width, &QSpinBox::editingFinished,
            [=]() {
                m_tileset->setCols(ui->spin_width->value());
            }
    );
    connect(ui->spin_height, &QSpinBox::editingFinished,
            [=]() {
                m_tileset->setRows(ui->spin_height->value());
            }
    );
    connect(ui->spin_width, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            [=](int newValue) {
                if(std::abs(oldWidth - newValue) == 1)
                    m_tileset->setCols(ui->spin_width->value());
                oldWidth = newValue;
            }
    );
    connect(ui->spin_height, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            [=](int newValue) {
                if(std::abs(oldHeight - newValue) == 1)
                    m_tileset->setRows(ui->spin_height->value());
                oldHeight = newValue;
            }
    );
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setUpItems(int)));
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setUpTileset(int)));
    connect(ui->TilesetName, SIGNAL(textChanged(QString)), m_tileset, SLOT(setName(QString)));

    connect(this, SIGNAL(windowShowed()), SLOT(showNotify()), Qt::QueuedConnection);
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(update()));
    connect(ui->spin_width, SIGNAL(valueChanged(int)), this, SLOT(update()));
    connect(ui->spin_height, SIGNAL(valueChanged(int)), this, SLOT(update()));

    {
        QAction *searchByName = m_searchSetup.addAction(tr("Search by Name", "Element search criteria"));
        searchByName->setCheckable(true);
        searchByName->setChecked(true);

        QAction *searchById = m_searchSetup.addAction(tr("Search by ID", "Element search criteria"));
        searchById->setCheckable(true);

        QAction *searchByIdContained = m_searchSetup.addAction(tr("Search by ID (Contained)", "Element search criteria"));
        searchByIdContained->setCheckable(true);

        m_searchSetup.addSeparator();
        QMenu   *sortBy = m_searchSetup.addMenu(tr("Sort by", "Search settings pop-up menu, sort submenu"));

        QAction *sortByName = sortBy->addAction(tr("Name", "Sort by name"));
        sortByName->setCheckable(true);
        sortByName->setChecked(true);

        QAction *sortById   = sortBy->addAction(tr("ID", "Sort by ID"));
        sortById->setCheckable(true);

        sortBy->addSeparator();
        QAction *sortBackward = sortBy->addAction(tr("Descending", "Descending sorting order"));
        sortBackward->setCheckable(true);

        m_searchSetup.connect(searchByName, &QAction::triggered,
        [=](bool)
        {
            searchByName->setChecked(true);
            searchById->setChecked(false);
            searchByIdContained->setChecked(false);
            m_searchBy = ElementsListModel::Search_ByName;
            ui->search->clear();
        });

        m_searchSetup.connect(searchById, &QAction::triggered,
        [=](bool)
        {
            searchByName->setChecked(false);
            searchById->setChecked(true);
            searchByIdContained->setChecked(false);
            m_searchBy = ElementsListModel::Search_ById;
            ui->search->clear();
        });

        m_searchSetup.connect(searchByIdContained, &QAction::triggered,
        [=](bool)
        {
            searchByName->setChecked(false);
            searchById->setChecked(false);
            searchByIdContained->setChecked(true);
            m_searchBy = ElementsListModel::Search_ByIdContained;
            ui->search->clear();
        });

        m_searchSetup.connect(sortByName, &QAction::triggered,
        [=](bool)
        {
            sortByName->setChecked(true);
            sortById->setChecked(false);
            m_model->setSort(ElementsListModel::Sort_ByName, sortBackward->isChecked());
        });

        m_searchSetup.connect(sortById, &QAction::triggered,
        [=](bool)
        {
            sortByName->setChecked(false);
            sortById->setChecked(true);
            m_model->setSort(ElementsListModel::Sort_ById, sortBackward->isChecked());
        });

        m_searchSetup.connect(sortBackward, &QAction::triggered,
        [=](bool)
        {
            m_model->setSort(sortByName->isChecked() ?
                                 ElementsListModel::Sort_ByName :
                                 ElementsListModel::Sort_ById,
                             sortBackward->isChecked());
        });

        ui->searchSetup->setMenu(&m_searchSetup);
    }
}

TilesetEditor::~TilesetEditor()
{
    clearFocus();
    delete ui;
}

void TilesetEditor::on_clearTileset_clicked()
{
    int x = QMessageBox::question(this, tr("Clean tileset editor"),
                                  tr("Do you want to clean tileset editor to create a new tileset?"),
                                  QMessageBox::Yes | QMessageBox::No);
    if(x == QMessageBox::Yes)
    {
        m_tileset->clear();
        lastFileName = "";
        lastFullPath = "";
        ui->TilesetName->setText("");
        ui->delete_me->setVisible(false);
    }
}

void TilesetEditor::setUpTileset(int type)
{
    m_tileset->clear();
    m_tileset->setType(type);
}

void TilesetEditor::setUpItems(int type)
{
    bool custom = ((m_mode != GFX_Staff) && (ui->customOnly->isChecked()));
    bool defstuff = ((m_mode != GFX_Staff) && (ui->defaultOnly->isChecked()));

    LvlScene *lvl_scene = dynamic_cast<LvlScene *>(scn);
    WldScene *wld_scene = dynamic_cast<WldScene *>(scn);

    m_model->clear();
    m_model->setElementsType(toElementType(type));

    m_model->setScene(scn);

    m_model->addElementsBegin();

    switch(type)
    {
    case ItemTypes::LVL_Block:
    {
        ui->listView->setViewMode(QListView::ListMode);
        ui->listView->setGridSize(QSize(34, 34));
        if(custom)
        {
            if(lvl_scene && (m_mode == GFX_Level))
            {
                for(int i = 0; i < lvl_scene->m_customBlocks.size(); ++i)
                {
                    m_model->addElement(static_cast<int>(lvl_scene->m_customBlocks[i]->setup.id));
                }
            }
        }
        else if(defstuff)
        {
            QHash<int, int> customElements;
            if(lvl_scene && (m_mode == GFX_Level))
            {
                for(int i = 0; i < lvl_scene->m_customBlocks.size(); ++i)
                {
                    customElements[static_cast<int>(lvl_scene->m_customBlocks[i]->setup.id)] = i;
                }
            }
            for(int i = 1; i < m_conf->main_block.size(); ++i)
            {
                if(!customElements.contains(i))
                    m_model->addElement(i);
            }
        }
        else
            for(int i = 1; i < m_conf->main_block.size(); ++i)
            {
                m_model->addElement(i);
            }
        break;
    }
    case ItemTypes::LVL_BGO:
    {
        ui->listView->setViewMode(QListView::ListMode);
        ui->listView->setGridSize(QSize(34, 34));
        if(custom)
        {
            if(lvl_scene && (m_mode == GFX_Level))
            {
                for(int i = 0; i < lvl_scene->m_customBGOs.size(); ++i)
                {
                    m_model->addElement(static_cast<int>(lvl_scene->m_customBGOs[i]->setup.id));
                }
            }
        }
        else if(defstuff)
        {
            QHash<int, int> customElements;
            if(lvl_scene && (m_mode == GFX_Level))
            {
                for(int i = 0; i < lvl_scene->m_customBGOs.size(); ++i)
                {
                    customElements[static_cast<int>(lvl_scene->m_customBGOs[i]->setup.id)] = i;
                }
            }
            for(int i = 1; i < m_conf->main_bgo.size(); ++i)
            {
                if(!customElements.contains(i))
                    m_model->addElement(i);
            }
        }
        else
            for(int i = 1; i < m_conf->main_bgo.size(); i++)
            {
                m_model->addElement(i);
            }
        break;
    }
    case ItemTypes::LVL_NPC:
    {
        ui->listView->setViewMode(QListView::ListMode);
        ui->listView->setGridSize(QSize(34, 34));
        if(custom)
        {
            if(lvl_scene && (m_mode == GFX_Level))
            {
                for(int i = 0; i < lvl_scene->m_customNPCs.size(); ++i)
                {
                    m_model->addElement(static_cast<int>(lvl_scene->m_customNPCs[i]->setup.id));
                }
            }
        }
        else if(defstuff)
        {
            QHash<int, int> customElements;
            if(lvl_scene && (m_mode == GFX_Level))
            {
                for(int i = 0; i < lvl_scene->m_customNPCs.size(); ++i)
                {
                    customElements[static_cast<int>(lvl_scene->m_customNPCs[i]->setup.id)] = i;
                }
            }
            for(int i = 1; i < m_conf->main_npc.size(); ++i)
            {
                if(!customElements.contains(i))
                    m_model->addElement(i);
            }
        }
        else
            for(int i = 1; i < m_conf->main_npc.size(); ++i)
            {
                m_model->addElement(i);
            }
        break;
    }
    case ItemTypes::WLD_Tile:
    {
        ui->listView->setViewMode(QListView::IconMode);
        ui->listView->setGridSize(QSize(34, 45));
        if(custom)
        {
            if(wld_scene && (m_mode == GFX_World))
            {
                for(int i = 0; i < wld_scene->m_customTerrain.size(); ++i)
                {
                    m_model->addElement(static_cast<int>(wld_scene->m_customTerrain[i]->setup.id));
                }
            }
        }
        else if(defstuff)
        {
            QHash<int, int> customElements;
            if(wld_scene && (m_mode == GFX_World))
            {
                for(int i = 0; i < wld_scene->m_customTerrain.size(); ++i)
                {
                    customElements[static_cast<int>(wld_scene->m_customTerrain[i]->setup.id)] = i;
                }
            }
            for(int i = 1; i < m_conf->main_wtiles.size(); ++i)
            {
                if(!customElements.contains(i))
                    m_model->addElement(i);
            }
        }
        else
            for(int i = 1; i < m_conf->main_wtiles.size(); ++i)
            {
                m_model->addElement(i);
            }
        break;
    }
    case ItemTypes::WLD_Scenery:
        ui->listView->setViewMode(QListView::IconMode);
        ui->listView->setGridSize(QSize(34, 45));
        if(custom)
        {
            if(wld_scene && (m_mode == GFX_World))
            {
                for(int i = 0; i < wld_scene->m_customSceneries.size(); ++i)
                {
                    m_model->addElement(static_cast<int>(wld_scene->m_customSceneries[i]->setup.id));
                }
            }
        }
        else if(defstuff)
        {
            QHash<int, int> customElements;
            if(wld_scene && (m_mode == GFX_World))
            {
                for(int i = 0; i < wld_scene->m_customSceneries.size(); ++i)
                {
                    customElements[static_cast<int>(wld_scene->m_customSceneries[i]->setup.id)] = i;
                }
            }
            for(int i = 1; i < m_conf->main_wscene.size(); ++i)
            {
                if(!customElements.contains(i))
                    m_model->addElement(i);
            }
        }
        else
            for(int i = 1; i < m_conf->main_wscene.size(); ++i)
            {
                m_model->addElement(i);
            }
        break;
    case ItemTypes::WLD_Path:
        ui->listView->setViewMode(QListView::IconMode);
        ui->listView->setGridSize(QSize(34, 45));
        if(custom)
        {
            if(wld_scene && (m_mode == GFX_World))
            {
                for(int i = 0; i < wld_scene->m_customPaths.size(); ++i)
                {
                    m_model->addElement(static_cast<int>(wld_scene->m_customPaths[i]->setup.id));
                }
            }
        }
        else if(defstuff)
        {
            QHash<int, int> customElements;
            if(wld_scene && (m_mode == GFX_World))
            {
                for(int i = 0; i < wld_scene->m_customPaths.size(); ++i)
                {
                    customElements[static_cast<int>(wld_scene->m_customPaths[i]->setup.id)] = i;
                }
            }
            for(int i = 1; i < m_conf->main_wpaths.size(); ++i)
            {
                if(!customElements.contains(i))
                    m_model->addElement(i);
            }
        }
        else
            for(int i = 1; i < m_conf->main_wpaths.size(); ++i)
            {
                m_model->addElement(i);
            }
        break;
    case ItemTypes::WLD_Level:
        ui->listView->setViewMode(QListView::IconMode);
        ui->listView->setGridSize(QSize(34, 45));
        if(custom)
        {
            if(wld_scene && (m_mode == GFX_World))
            {
                for(int i = 0; i < wld_scene->m_customLevels.size(); ++i)
                {
                    m_model->addElement(static_cast<int>(wld_scene->m_customLevels[i]->setup.id));
                }
            }
        }
        else if(defstuff)
        {
            QHash<int, int> customElements;
            if(wld_scene && (m_mode == GFX_World))
            {
                for(int i = 0; i < wld_scene->m_customLevels.size(); ++i)
                {
                    customElements[static_cast<int>(wld_scene->m_customLevels[i]->setup.id)] = i;
                }
            }
            for(int i = 0; i < m_conf->main_wlevels.size(); ++i)
            {
                if(!customElements.contains(i))
                    m_model->addElement(i);
            }
        }
        else
            for(int i = 0; i < m_conf->main_wlevels.size(); ++i)
            {
                m_model->addElement(i);
            }
        break;
    default:
        break;
    }

    // We should change drag/drop properties after calling of `setViewMode()` as it probably resets everything
    ui->listView->setDropIndicatorShown(false);
    ui->listView->setAcceptDrops(false);
    ui->listView->setDragEnabled(true);
    ui->listView->setDragDropMode(QListView::DragOnly);

    m_model->addElementsEnd();
}

ElementsListModel::ElementType TilesetEditor::toElementType(int type)
{
    return static_cast<ElementsListModel::ElementType>(type);
}

void TilesetEditor::on_SaveTileset_clicked()
{
    QDir(m_conf->config_dir).mkpath("tilesets/");

    bool ok;
    QString fileName = QInputDialog::getText(this, tr("Please enter a filename!"),
                       tr("Filename:"), QLineEdit::Normal,
                       lastFileName.isEmpty() ? m_tileset->name() : lastFileName, &ok);
    if(!ok || fileName.isEmpty())
        return;

    //Filter input from forbidden characters
    fileName = util::filePath(fileName);
    lastFileName = fileName;

    if(!fileName.endsWith(".tileset.ini"))
        fileName += ".tileset.ini";

    QString savePath;
    switch(m_mode)
    {
    case GFX_Level:
        savePath = dynamic_cast<LvlScene *>(scn)->m_data->meta.path +
                   (ui->specific->isChecked() ? "/" + dynamic_cast<LvlScene *>(scn)->m_data->meta.filename : "") + "/";
        break;
    case GFX_World:
        savePath = dynamic_cast<WldScene *>(scn)->m_data->meta.path +
                   (ui->specific->isChecked() ? "/" + dynamic_cast<WldScene *>(scn)->m_data->meta.filename : "") + "/";
        break;
    case GFX_Staff:
        savePath = m_conf->config_dir + "tilesets/";
        break;
    }

    QDir target(savePath);
    if(!target.exists()) target.mkpath(savePath);

    tileset::SaveSimpleTileset(savePath + fileName, m_tileset->toSimpleTileset());

    lastFullPath = QFileInfo(savePath + fileName).absoluteFilePath();
    ui->delete_me->setVisible(true);
}

void TilesetEditor::on_OpenTileset_clicked()
{

    QString openPath;
    switch(m_mode)
    {
    case GFX_Level:
        openPath = dynamic_cast<LvlScene *>(scn)->m_data->meta.path + "/";
        break;
    case GFX_World:
        openPath = dynamic_cast<WldScene *>(scn)->m_data->meta.path + "/";
        break;
    case GFX_Staff:
        openPath = m_conf->config_dir + "tilesets/";
        break;
    }

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Tileset"),
                       openPath, QString("PGE Tileset (*.tileset.ini)"), nullptr, c_fileDialogOptions);
    if(fileName.isEmpty())
        return;

    openTileset(fileName, ui->customOnly->isChecked());

}

void TilesetEditor::openTileset(QString filePath, bool isCustom)
{
    if(filePath.isEmpty())
        return;

    QFileInfo finfo(filePath);
    lastFileName = finfo.baseName();
    lastFullPath = finfo.absoluteFilePath();

    ui->delete_me->setVisible(false);

    SimpleTileset simple;
    if(!tileset::OpenSimpleTileset(filePath, simple))
    {
        QMessageBox::warning(this, tr("Failed to load tileset!"), tr("Failed to load tileset!\nData may be corrupted!"));
    }
    else
    {
        ui->TilesetName->setText(simple.tileSetName);
        ui->spin_width->setValue(static_cast<int>(simple.cols));
        ui->spin_height->setValue(static_cast<int>(simple.rows));
        ui->comboBox->setCurrentIndex(static_cast<int>(simple.type));
        setUpItems(simple.type);
        m_tileset->loadSimpleTileset(simple);
    }

    ui->specific->setChecked(isCustom);
    ui->delete_me->setVisible(true);
}

void TilesetEditor::loadSimpleTileset(const SimpleTileset &tileset, bool isCustom)
{
    ui->TilesetName->setText(tileset.tileSetName);
    ui->spin_width->setValue(static_cast<int>(tileset.cols));
    ui->spin_height->setValue(static_cast<int>(tileset.rows));
    ui->comboBox->setCurrentIndex(static_cast<int>(tileset.type));
    setUpItems(tileset.type);
    m_tileset->loadSimpleTileset(tileset);
    lastFileName = QString(tileset.fileName).remove(".tileset.ini");

    switch(m_mode)
    {
    case GFX_Level:
        lastFullPath = dynamic_cast<LvlScene *>(scn)->m_data->meta.path +
                       (isCustom ? "/" + dynamic_cast<LvlScene *>(scn)->m_data->meta.filename : "") + "/";
        break;
    case GFX_World:
        lastFullPath = dynamic_cast<WldScene *>(scn)->m_data->meta.path +
                       (isCustom ? "/" + dynamic_cast<WldScene *>(scn)->m_data->meta.filename : "") + "/";
        break;
    case GFX_Staff:
        lastFullPath = m_conf->config_dir + "tilesets/";
        break;
    }
    lastFullPath.append(tileset.fileName);

    ui->specific->setChecked(isCustom);
    ui->delete_me->setVisible(true);
}


void TilesetEditor::on_customOnly_clicked()
{
    if(m_mode == GFX_Staff)
        return;
    ui->defaultOnly->setChecked(false);
    ui->search->clear();
    setUpItems(ui->comboBox->currentIndex());
}

void TilesetEditor::on_defaultOnly_clicked()
{
    if(m_mode == GFX_Staff)
        return;
    ui->customOnly->setChecked(false);
    ui->search->clear();
    setUpItems(ui->comboBox->currentIndex());
}


void TilesetEditor::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);
    qApp->processEvents();
    emit windowShowed();
}

void TilesetEditor::showNotify()
{
    QSettings cCounters(AppPathManager::settingsFile(), QSettings::IniFormat);
    cCounters.setIniCodec("UTF-8");
    cCounters.beginGroup("message-boxes");
    bool showNotice = cCounters.value("tileset-editor-greeting", true).toBool();
    if((m_mode == GFX_Staff) && (showNotice))
    {
        QMessageBox msg;
        msg.setWindowTitle(tr("Tileset box editor"));
        msg.setWindowIcon(this->windowIcon());
        QCheckBox box;
        box.setParent(this);
        box.setWindowModality(Qt::WindowModal);
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
    cCounters.setValue("tileset-editor-greeting", showNotice);
    cCounters.endGroup();
}

void TilesetEditor::on_delete_me_clicked()
{
    int x = QMessageBox::question(this, tr("Remove tileset"),
                                  tr("Do you want to remove this tileset?"),
                                  QMessageBox::Yes | QMessageBox::No);

    if(x == QMessageBox::Yes)
    {
        QFile(lastFullPath).remove();
        QMessageBox::information(this, tr("Tileset removed"), tr("Tileset has been removed!"), QMessageBox::Ok);
        this->close();
    }
}

void TilesetEditor::keyPressEvent(QKeyEvent *event)
{
    //Prevent Enter/Return key to spawn tileset saving dialog after resize
    if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
        return;
    QDialog::keyPressEvent(event);
}

void TilesetEditor::on_search_textChanged(const QString &arg1)
{
    m_model->setFilter(arg1, m_searchBy);
}
