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

#include <QObject>
#include <QMessageBox>
#include <QScrollArea>

#include <common_features/logger.h>
#include <common_features/themes.h>
#include <common_features/util.h>
#include <tools/tilesets/tileset.h>
#include <tools/tilesets/tilesetitembutton.h>
#include <tools/tilesets/tilesetconfiguredialog.h>
#include <data_configs/obj_tilesets.h>
#include <data_configs/data_configs.h>
#include <dev_console/devconsole.h>

#include <mainwindow.h>
#include <ui_mainwindow.h>

#include "tileset_item_box.h"
#include "ui_tileset_item_box.h"

void MainWindow::on_actionTilesetBox_triggered(bool checked)
{
    dock_TilesetBox->setVisible(checked);
    if(checked) dock_TilesetBox->raise();
}

 // Create and refresh tileset box data
void MainWindow::setTileSetBox()
{
    dock_TilesetBox->setTileSetBox();
}

TilesetItemBox::TilesetItemBox(QWidget *parent) :
    QDockWidget(parent),
    MWDock_Base(parent),
    ui(new Ui::TilesetItemBox)
{
    setVisible(false);
    ui->setupUi(this);
    connect(ui->customTilesetSearchEdit, SIGNAL(textChanged(QString)), this, SLOT(makeCurrentTileset()));
    ui->TileSetsCategories->setTabPosition(GlobalSettings::TSTToolboxPos);

    this->setFocusPolicy(Qt::ClickFocus);

    lockTilesetBox=false;

    QRect dg = qApp->desktop()->availableGeometry(qApp->desktop()->primaryScreen());
    int GOffset=240;

    mw()->addDockWidget(Qt::BottomDockWidgetArea, this);
    connect(mw(), SIGNAL(languageSwitched()), this, SLOT(re_translate()));
    setFloating(true);
    setGeometry(
                dg.x()+GOffset,
                dg.y()+dg.height()-600,
                800,
                300
                );

    mw()->docks_level_and_world.
          addState(this, &GlobalSettings::TilesetBoxVis,
            &setVisible, &isVisible);
}



void TilesetItemBox::focusInEvent(QFocusEvent *ev)
{
    QDockWidget::focusInEvent(ev);
    //ev->accept();
    //qApp->setActiveWindow(mw());
}

TilesetItemBox::~TilesetItemBox()
{
    delete ui;
}

void TilesetItemBox::re_translate()
{
    lockTilesetBox=true;
    ui->retranslateUi(this);
    lockTilesetBox=false;
}


namespace pge_tilesetbox
{
    int current = 0;
    int comboCurrent = 0;
}

void TilesetItemBox::on_TilesetItemBox_visibilityChanged(bool visible)
{
    if(!mw()) return;
    mw()->ui->actionTilesetBox->setChecked(visible);
    if(visible) setTileSetBox();  //!< update when it show
}

void TilesetItemBox::setTileSetBox(bool force)
{
    if(!mw()) return;
    if(lockTilesetBox) return;
    if((!this->isVisible()) && (!force)) return; //!< Don't update invisible

    pge_tilesetbox::current = ui->TileSetsCategories->currentIndex();

    clearTilesetGroups();

    //QStringList filters("*.tsgrp.ini");
    //QDir grpDir = configs.config_dir + "group_tilesets/";
    //QStringList entries;
    //entries = grpDir.entryList(filters, QDir::Files);
    foreach (SimpleTilesetGroup grp, mw()->configs.main_tilesets_grp) {
        prepareTilesetGroup(grp);
    }

    makeAllTilesets();
}



void TilesetItemBox::on_tilesetGroup_currentIndexChanged(int index)
{
    if(!mw()) return;
    Q_UNUSED(index);
    WriteToLog(QtDebugMsg, "TilesetBox -> change combobox's index");
    if(lockTilesetBox) return;
    makeCurrentTileset();
}



void TilesetItemBox::on_newTileset_clicked()
{
    // THREAD TEST
    //future = QtConcurrent::run(loopForever); //<! Tiny test with thread
    //QMessageBox::information(this, "test", "test", QMessageBox::Ok);

    bool untitled=false;
    if(mw()->activeChildWindow()==1)
        untitled=mw()->activeLvlEditWin()->isUntitled;
    else if(mw()->activeChildWindow()==3)
        untitled=mw()->activeWldEditWin()->isUntitled;

    if(untitled)
    {
        QMessageBox::warning(this, tr("File not saved"),
        tr("File doesn't saved on disk."), QMessageBox::Ok);
            return;
    }

    TilesetConfigureDialog* tilesetConfDia;

    if(mw()->activeChildWindow()==1)
        tilesetConfDia = new TilesetConfigureDialog(&mw()->configs, mw()->activeLvlEditWin()->scene, mw());
    else if(mw()->activeChildWindow()==3)
        tilesetConfDia = new TilesetConfigureDialog(&mw()->configs, mw()->activeWldEditWin()->scene, mw());
    else
        tilesetConfDia = new TilesetConfigureDialog(&mw()->configs, NULL, mw());

    util::DialogToCenter(tilesetConfDia);

    tilesetConfDia->exec();
    delete tilesetConfDia;

    mw()->configs.loadTilesets();
    setTileSetBox();
}

void TilesetItemBox::setTabPosition(QTabWidget::TabPosition pos)
{
    ui->TileSetsCategories->setTabPosition(pos);
}

void TilesetItemBox::editSelectedTileset()
{
    TilesetConfigureDialog* tilesetConfDia;

    if(mw()->activeChildWindow()==1)
        tilesetConfDia = new TilesetConfigureDialog(&mw()->configs, mw()->activeLvlEditWin()->scene, mw());
    else if(mw()->activeChildWindow()==3)
        tilesetConfDia = new TilesetConfigureDialog(&mw()->configs, mw()->activeWldEditWin()->scene, mw());
    else
        tilesetConfDia = new TilesetConfigureDialog(&mw()->configs, NULL, mw());

    util::DialogToCenter(tilesetConfDia);

    QPushButton* b = qobject_cast<QPushButton*>(sender());
    if(!b){
        delete tilesetConfDia;
        return;
    }



    QWidget* parent = b->parentWidget();
    if(!parent){
        delete tilesetConfDia;
        return;
    }

    QGroupBox* box = qobject_cast<QGroupBox*>(parent);
    if(!box){
        delete tilesetConfDia;
        return;
    }

    QString tilesetName = box->title();
    QVector<SimpleTileset> ctilesets = loadCustomTilesets();
    for(int i = 0; i < ctilesets.size(); ++i){
        SimpleTileset &aTileset = ctilesets[i];
        if(aTileset.tileSetName == tilesetName)
        {
            tilesetConfDia->loadSimpleTileset(aTileset, aTileset.customDir);
            tilesetConfDia->exec();
            break;
        }
    }

    delete tilesetConfDia;

    mw()->configs.loadTilesets();
    setTileSetBox();
}

QScrollArea* TilesetItemBox::getFrameTilesetOfTab(QWidget* catTab)
{
    WriteToLog(QtDebugMsg, "TilesetBox -> find QScroll Area");

    QList<QScrollArea*> framechildren = catTab->findChildren<QScrollArea*>();
    foreach(QScrollArea* f, framechildren){
        // if(qobject_cast<QLabel*>(f)!=0)
        // continue;
        //DevConsole::log(QString("Class name %1").arg(f->metaObject()->className()), "Debug");
        if(QString(f->metaObject()->className())=="QScrollArea")
            return f;
    }
    return 0;
}

QComboBox* TilesetItemBox::getGroupComboboxOfTab(QWidget* catTab)
{
    WriteToLog(QtDebugMsg, "TilesetBox -> findCombobox");
    QComboBox* comboxes = catTab->findChild<QComboBox*>();
    return comboxes;
}

QWidget* TilesetItemBox::findTabWidget(const QString &categoryItem)
{
    WriteToLog(QtDebugMsg, "TilesetBox -> find Tab");

    QTabWidget* cat = ui->TileSetsCategories;
    for(int i = 0; i < cat->count(); ++i){
        if(cat->tabText(i) == categoryItem){
            return cat->widget(i);
        }
    }
    return 0;
}

QWidget* TilesetItemBox::makeCategory(const QString &categoryItem)
{
    QTabWidget* TileSetsCategories = ui->TileSetsCategories;
    QWidget* catWid;
    QWidget* scrollWid;
    QGridLayout* catLayout;
    QLabel* grpLabel;
    QComboBox* tilesetGroup;
    QSpacerItem* spItem;
    QScrollArea* TileSets;
    FlowLayout *theLayOut;


    catWid = new QWidget();
    scrollWid = new QWidget();
    catLayout = new QGridLayout(catWid);
    catLayout->setSpacing(0);
    catLayout->setContentsMargins(0, 0, 0, 0);
    grpLabel = new QLabel(catWid);
    grpLabel->setText(tr("Group:"));
    catLayout->addWidget(grpLabel, 0, 0, 1, 1);

    tilesetGroup = new QComboBox(catWid);

    catLayout->addWidget(tilesetGroup, 0, 1, 1, 1);
    tilesetGroup->setInsertPolicy(QComboBox::InsertAlphabetically);
    spItem = new QSpacerItem(1283, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    catLayout->addItem(spItem, 0, 2, 1, 1);
    TileSets = new QScrollArea(catWid);
    TileSets->setWidget(scrollWid);
    TileSets->setWidgetResizable(true);
    TileSets->setFrameShape(QFrame::StyledPanel);
    TileSets->setFrameShadow(QFrame::Raised);
    TileSets->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    TileSets->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    theLayOut = new FlowLayout(scrollWid);
    theLayOut->setSizeConstraint(QLayout::SetNoConstraint);

    catLayout->addWidget(TileSets, 1, 0, 1, 3);

    TileSetsCategories->addTab(catWid, QString());
    TileSetsCategories->setTabText(TileSetsCategories->indexOf(catWid), categoryItem);

    return catWid;
}

void TilesetItemBox::prepareTilesetGroup(const SimpleTilesetGroup &tilesetGroups)
{
    if(lockTilesetBox) return;

    QWidget *t = findTabWidget(tilesetGroups.groupCat);
    if(!t){
        t = makeCategory(tilesetGroups.groupCat);
    }
    QComboBox *c = getGroupComboboxOfTab(t);
    if(!c)
        return;
    c->setInsertPolicy(QComboBox::InsertAlphabetically);
    if(!util::contains(c,tilesetGroups.groupName)){
        c->addItem(tilesetGroups.groupName);
    }
    c->model()->sort(0);
    c->setCurrentIndex(0);
    connect(c, SIGNAL(currentIndexChanged(int)), this, SLOT(on_tilesetGroup_currentIndexChanged(int)));
}

void TilesetItemBox::clearTilesetGroups()
{
    if(lockTilesetBox) return;

    QTabWidget* cat = ui->TileSetsCategories;
    int i = 0;
    int f = cat->count();
    while(f > i)
    { //include custom tab
        if(cat->tabText(i) == "Custom")
        {
            ++i;
            continue;
        }

        QWidget* acCat = cat->widget(i);
        cat->removeTab(i);
        delete acCat;
        f = cat->count();
    }
}


void TilesetItemBox::makeSelectedTileset(int tabIndex)
{
    if(lockTilesetBox) return;

    QTabWidget* cat = ui->TileSetsCategories;
    if(!(cat->tabText(tabIndex) == "Custom"))
    {
        QWidget* current = cat->widget( tabIndex );
        if(!current)
            return;

        QString category = cat->tabText( tabIndex );

        #ifdef _DEBUG_
        DevConsole::log(QString("Category %1").arg(cat->tabText(cat->currentIndex())), "Debug");
        #endif

        QScrollArea* currentFrame = getFrameTilesetOfTab(current);
        QComboBox* currentCombo = getGroupComboboxOfTab(current);
        if(!currentFrame || !currentCombo)
            return;

        QWidget* scrollWid = currentFrame->widget();
        if(!scrollWid)
            return;

        qDeleteAll(scrollWid->findChildren<QGroupBox*>());

        if(scrollWid->layout() == 0)
            scrollWid->setLayout(new FlowLayout());

        currentFrame->setWidgetResizable(true);

        #ifdef _DEBUG_
        DevConsole::log(QString("size %1 %2")
                        .arg(scrollWid->layout()->geometry().width())
                        .arg(scrollWid->layout()->geometry().height())
                        , "Debug");
        #endif

        QString currentGroup = currentCombo->currentText();
        for(int i = 0; i < mw()->configs.main_tilesets_grp.size(); i++)
        {
            if((mw()->configs.main_tilesets_grp[i].groupCat == category)
             &&(mw()->configs.main_tilesets_grp[i].groupName == currentGroup))//category
            {
                #ifdef _DEBUG_
                DevConsole::log(QString("Group %1").arg(configs.main_tilesets_grp[i].groupName), "Debug");
                DevConsole::log(QString("Tilesets %1").arg(configs.main_tilesets_grp[i].tilesets.size()), "Debug");
                #endif

                QStringList l = mw()->configs.main_tilesets_grp[i].tilesets;
                foreach(QString s, l)
                {
                    for(int j = 0; j < mw()->configs.main_tilesets.size(); j++)
                    {
                        if(s == mw()->configs.main_tilesets[j].fileName)
                        {
                            SimpleTileset &s = mw()->configs.main_tilesets[j];
                            QGroupBox* tilesetNameWrapper = new QGroupBox(s.tileSetName, scrollWid);
                            ((FlowLayout*)scrollWid->layout())->addWidget(tilesetNameWrapper);
                            QGridLayout* l = new QGridLayout(tilesetNameWrapper);
                            l->setContentsMargins(4,4,4,4);
                            l->setSpacing(2);
                            for(int k=0; k<s.items.size(); k++)
                            {
                                SimpleTilesetItem &item = s.items[k];
                                TilesetItemButton* tbutton = new TilesetItemButton(&mw()->configs, NULL, tilesetNameWrapper);
                                tbutton->applySize(32,32);
                                tbutton->applyItem(s.type, item.id);
                                l->addWidget(tbutton, item.row, item.col);
                                connect(tbutton, SIGNAL(clicked(int,ulong)), mw(), SLOT(SwitchPlacingItem(int,ulong)));
                            }
                            break;
                        }
                    }
                }
            break;
            }
        }
    }
    else
    {
        QWidget* current = cat->widget( tabIndex );
        if(!current)
            return;

        QScrollArea* currentFrame = getFrameTilesetOfTab(current);
        //        QComboBox* currentCombo = getGroupComboboxOfTab(current);
        //        if(!currentFrame || !currentCombo)
        //            return;

        QWidget* scrollWid = currentFrame->widget();
        if(!scrollWid)
            return;

        qDeleteAll(scrollWid->findChildren<QGroupBox*>());

        if(scrollWid->layout() == 0)
            scrollWid->setLayout(new FlowLayout());

        currentFrame->setWidgetResizable(true);

        #ifdef _DEBUG_
        DevConsole::log(QString("size %1 %2")
                        .arg(scrollWid->layout()->geometry().width())
                        .arg(scrollWid->layout()->geometry().height())
                        , "Debug");
        #endif


        QVector<SimpleTileset> ctsets = loadCustomTilesets();
        if(!ctsets.isEmpty()){
            for(int j = 0; j < ctsets.size(); j++)
            {
                unsigned int mostRighter = 0;
                SimpleTileset &s = ctsets[j];
                if(!s.tileSetName.contains(ui->customTilesetSearchEdit->text(), Qt::CaseInsensitive))
                    continue;
                QGroupBox* tilesetNameWrapper = new QGroupBox(s.tileSetName, scrollWid);
                ((FlowLayout*)scrollWid->layout())->addWidget(tilesetNameWrapper);
                QGridLayout* l = new QGridLayout(tilesetNameWrapper);
                l->setContentsMargins(4,4,4,4);
                l->setSpacing(2);
                for(int k=0; k<s.items.size(); k++)
                {
                    SimpleTilesetItem &item = s.items[k];
                    TilesetItemButton* tbutton = new TilesetItemButton(&mw()->configs, NULL, tilesetNameWrapper);
                    tbutton->applySize(32,32);
                    tbutton->applyItem(s.type, item.id);
                    l->addWidget(tbutton, item.row, item.col);
                    if(item.col >= mostRighter){
                        mostRighter = item.col + 1;
                    }
                    connect(tbutton, SIGNAL(clicked(int,ulong)), mw(), SLOT(SwitchPlacingItem(int,ulong)));
                }
                QPushButton* b = new QPushButton(Themes::icon(Themes::pencil),"",tilesetNameWrapper);
                b->setMaximumSize(32,32);
                b->setFlat(true);
                l->addWidget(b, 0, mostRighter);
                connect(b, SIGNAL(clicked()), this, SLOT(editSelectedTileset()));
            }
        }
    }
}

QVector<SimpleTileset> TilesetItemBox::loadCustomTilesets(){
    QVector<SimpleTileset> ctsets;

    QString path;
    QString cfolder;
    bool doIt=false;
    if(mw()->activeChildWindow()==1)
    {
        path = mw()->activeLvlEditWin()->LvlData.path+"/";
        cfolder = mw()->activeLvlEditWin()->LvlData.filename+"/";
        doIt = !mw()->activeLvlEditWin()->LvlData.untitled;
    }
    else
    if(mw()->activeChildWindow()==3)
    {
        path = mw()->activeWldEditWin()->WldData.path+"/";
        cfolder = mw()->activeWldEditWin()->WldData.filename+"/";
        doIt = !mw()->activeWldEditWin()->WldData.untitled;
    }

    if(doIt)
    {
        QStringList paths;
        paths << path << path+cfolder;

        foreach(QString p, paths)
        {
            QStringList filters;
            filters << "*.tileset.ini";
            QDir tilesetDir(p);
            tilesetDir.setSorting(QDir::Name);
            tilesetDir.setNameFilters(filters);
            QStringList files = tilesetDir.entryList(filters);
            foreach(QString file, files)
            {
                SimpleTileset xxx;
                if(tileset::OpenSimpleTileset(p + file, xxx))
                {
                    xxx.customDir = (p == path+cfolder);
                    ctsets.push_back(xxx);
                }
            }
        }
    }
    return ctsets;
}

void TilesetItemBox::makeCurrentTileset()
{
    if(lockTilesetBox) return;

    QTabWidget* cat = ui->TileSetsCategories;
    makeSelectedTileset(cat->currentIndex());
}

void TilesetItemBox::makeAllTilesets()
{
    if(lockTilesetBox) return;

    using namespace pge_tilesetbox;
    QTabWidget* cat = ui->TileSetsCategories;

    DevConsole::log(QString("index %1 count %2").arg(current).arg(cat->count()), "Debug");

    for(int i=0; i< cat->count(); i++)
        makeSelectedTileset(i);

    if(cat->count() > current)
        cat->setCurrentIndex(current);

    DevConsole::log(QString("index %1 count %2").arg(current).arg(cat->count()), "Debug");
}
