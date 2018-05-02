/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2016 Vitaly Novichkov <admin@wohlnet.ru>
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
#include <QGroupBox>
#include <QMessageBox>
#include <QScrollArea>
#include <QTabBar>
#include <QToolButton>
#include <QDesktopWidget>

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
    setAttribute(Qt::WA_ShowWithoutActivating);
    ui->setupUi(this);
    connect(ui->customTilesetSearchEdit, SIGNAL(textChanged(QString)), this, SLOT(makeCurrentTileset()));
    ui->TileSetsCategories->setTabPosition(GlobalSettings::TSTToolboxPos);

    this->setFocusPolicy(Qt::ClickFocus);

    lockTilesetBox = false;

    QRect dg = qApp->desktop()->availableGeometry(qApp->desktop()->primaryScreen());
    int GOffset = 240;

    mw()->addDockWidget(Qt::BottomDockWidgetArea, this);
    connect(mw(), SIGNAL(languageSwitched()), this, SLOT(re_translate()));
    setFloating(true);
    setGeometry(
        dg.x() + GOffset,
        dg.bottom() - height() - 5,
        800,
        250
    );

    m_lastVisibilityState = isVisible();
    mw()->docks_level_and_world.
    addState(this, &m_lastVisibilityState);
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
    lockTilesetBox = true;
    ui->retranslateUi(this);
    lockTilesetBox = false;
    setTileSetBox();
}


namespace pge_tilesetbox
{
    static int current = 0;
    //static int comboCurrent = 0;
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
    QList<SimpleTilesetGroup > t_groups = mw()->configs.main_tilesets_grp;
    qSort(t_groups);//Sort alphabetically by group name
    for(SimpleTilesetGroup &grp : t_groups)
        prepareTilesetGroup(grp);

    makeAllTilesets();
}



void TilesetItemBox::on_tilesetGroup_currentIndexChanged(int index)
{
    if(!mw()) return;
    Q_UNUSED(index);
    #ifdef _DEBUG_
    WriteToLog(QtDebugMsg, "TilesetBox -> change combobox's index");
    #endif
    if(lockTilesetBox) return;
    makeCurrentTileset();
}



void TilesetItemBox::on_newTileset_clicked()
{
    // THREAD TEST
    //future = QtConcurrent::run(loopForever); //<! Tiny test with thread
    //QMessageBox::information(this, "test", "test", QMessageBox::Ok);

    bool untitled = false;
    if(mw()->activeChildWindow() == 1)
        untitled = mw()->activeLvlEditWin()->isUntitled;
    else if(mw()->activeChildWindow() == 3)
        untitled = mw()->activeWldEditWin()->isUntitled;

    if(untitled)
    {
        QMessageBox::warning(this, tr("File not saved"),
                             tr("File doesn't saved on disk."), QMessageBox::Ok);
        return;
    }

    TilesetConfigureDialog *tilesetConfDia;

    if(mw()->activeChildWindow() == 1)
        tilesetConfDia = new TilesetConfigureDialog(&mw()->configs, mw()->activeLvlEditWin()->scene, mw());
    else if(mw()->activeChildWindow() == 3)
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
    TilesetConfigureDialog *tilesetConfDia;

    if(mw()->activeChildWindow() == 1)
        tilesetConfDia = new TilesetConfigureDialog(&mw()->configs, mw()->activeLvlEditWin()->scene, mw());
    else if(mw()->activeChildWindow() == 3)
        tilesetConfDia = new TilesetConfigureDialog(&mw()->configs, mw()->activeWldEditWin()->scene, mw());
    else
        tilesetConfDia = new TilesetConfigureDialog(&mw()->configs, NULL, mw());

    util::DialogToCenter(tilesetConfDia);

    QPushButton *b = qobject_cast<QPushButton *>(sender());
    if(!b)
    {
        delete tilesetConfDia;
        return;
    }

    QWidget *parent = b->parentWidget();
    if(!parent)
    {
        delete tilesetConfDia;
        return;
    }

    QGroupBox *box = qobject_cast<QGroupBox *>(parent);
    if(!box)
    {
        delete tilesetConfDia;
        return;
    }

    QString fileName = b->property("tileset-file-name").toString();//box->title();
    QVector<SimpleTileset> ctilesets = loadCustomTilesets();
    for(int i = 0; i < ctilesets.size(); ++i)
    {
        SimpleTileset &aTileset = ctilesets[i];
        if(aTileset.fileName == fileName)
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

QScrollArea *TilesetItemBox::getFrameTilesetOfTab(QWidget *catTab)
{
    #ifdef _DEBUG_
    WriteToLog(QtDebugMsg, "TilesetBox -> find QScroll Area");
    #endif

    QList<QScrollArea *> framechildren = catTab->findChildren<QScrollArea *>();
    foreach(QScrollArea *f, framechildren)
    {
        // if(qobject_cast<QLabel*>(f)!=0)
        // continue;
        //DevConsole::log(QString("Class name %1").arg(f->metaObject()->className()), "Debug");
        if(QString(f->metaObject()->className()) == "QScrollArea")
            return f;
    }
    return nullptr;
}

QTabBar *TilesetItemBox::getGroupComboboxOfTab(QWidget *catTab)
{
    #ifdef _DEBUG_
    WriteToLog(QtDebugMsg, "TilesetBox -> findCombobox");
    #endif
    QTabBar *comboxes = catTab->findChild<QTabBar *>();
    return comboxes;
}

QWidget *TilesetItemBox::findTabWidget(const QString &categoryItem)
{
    #ifdef _DEBUG_
    WriteToLog(QtDebugMsg, "TilesetBox -> find Tab");
    #endif
    QTabWidget *cat = ui->TileSetsCategories;
    for(int i = 0; i < cat->count(); ++i)
    {
        if(cat->tabText(i) == categoryItem)
            return cat->widget(i);
    }
    return nullptr;
}

QWidget *TilesetItemBox::makeCategory(const QString &categoryItem)
{
    QTabWidget *TileSetsCategories = ui->TileSetsCategories;
    QWidget *catWid;
    QWidget *scrollWid;
    QGridLayout *catLayout;
    QLabel *grpLabel;
    QTabBar *tilesetGroup;
    QToolButton *menuButton;
    QMenu       *menuGroups;
    //QSpacerItem *spItem;
    QScrollArea *TileSets;
    FlowLayout *theLayOut;


    catWid = new QWidget();
    scrollWid = new QWidget();
    catLayout = new QGridLayout(catWid);
    catLayout->setSpacing(0);
    catLayout->setContentsMargins(0, 0, 0, 0);
    grpLabel = new QLabel(catWid);
    grpLabel->setText(tr("Group:"));
    catLayout->addWidget(grpLabel, 0, 0, 1, 1);

    tilesetGroup = new QTabBar(catWid);
    tilesetGroup->setExpanding(false);

    catLayout->addWidget(tilesetGroup, 0, 1, 1, 1);
    //tilesetGroup->setInsertPolicy(QTabBar::InsertAlphabetically);
    //spItem = new QSpacerItem(500, 20, QSizePolicy::Preferred, QSizePolicy::Minimum);
    //catLayout->addItem(spItem, 0, 2, 1, 1);
    catLayout->setColumnStretch(0, 0);
    catLayout->setColumnStretch(1, 100);

    menuButton = new QToolButton(catWid);
    menuButton->setText("▼");
    menuGroups = new QMenu(menuButton);
    menuButton->setMenu(menuGroups);
    menuButton->setPopupMode(QToolButton::InstantPopup);
    catLayout->addWidget(menuButton, 0, 2, 1, 1);

    tilesetGroup->setProperty("menuButton", qVariantFromValue(menuButton));
    tilesetGroup->setProperty("menu", qVariantFromValue(menuGroups));

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
    if(!t)
        t = makeCategory(tilesetGroups.groupCat);
    QTabBar *c = getGroupComboboxOfTab(t);
    if(!c)
        return;
    //QToolButton *cbutton = qvariant_cast<QToolButton*>(c->property("menuButton"));
    QMenu *cmenu = qvariant_cast<QMenu*>(c->property("menu"));
//            tilesetGroup->setProperty("menuButton", qVariantFromValue(menuButton));
//            tilesetGroup->setProperty("menu", qVariantFromValue(menuInButton));
    //c->setInsertPolicy(QComboBox::InsertAlphabetically);
    if(!util::contains(c, tilesetGroups.groupName))
    {
        QString g = tilesetGroups.groupName;
        g.replace("&", "&&");
        int tabIndex = c->addTab(g);//Escaped ampersands
        QAction *a = cmenu->addAction(g);
        a->setData(tabIndex);
        c->connect(a, reinterpret_cast<void(QAction::*)(void)>(&QAction::triggered),
                   [c,a](){ c->setCurrentIndex(a->data().toInt()); }
        );
    }
    //c->model()->sort(0);
    c->setCurrentIndex(0);
    c->connect(c, SIGNAL(currentChanged(int)), this, SLOT(on_tilesetGroup_currentIndexChanged(int)));
}

void TilesetItemBox::clearTilesetGroups()
{
    if(lockTilesetBox) return;

    QTabWidget *cat = ui->TileSetsCategories;
    int i = 0;
    int f = cat->count();
    while(f > i)
    {
        //include custom tab
        if(cat->tabText(i) == "Custom")
        {
            ++i;
            continue;
        }

        QWidget *acCat = cat->widget(i);
        cat->removeTab(i);
        delete acCat;
        f = cat->count();
    }
}


void TilesetItemBox::makeSelectedTileset(int tabIndex)
{
    if(lockTilesetBox)
        return;

    QGraphicsScene *scene = nullptr;

    LevelEdit *lvlEdit = mw()->activeLvlEditWin();
    WorldEdit *wldEdit = mw()->activeWldEditWin();

    if((lvlEdit) && (lvlEdit->sceneCreated))
        scene = lvlEdit->scene;
    else if((wldEdit) && (wldEdit->sceneCreated))
        scene = wldEdit->scene;

    QTabWidget *cat = ui->TileSetsCategories;
    if(!(cat->tabText(tabIndex) == "Custom"))
    {
        QWidget *current = cat->widget(tabIndex);
        if(!current)
            return;

        QString category = cat->tabText(tabIndex);

        #ifdef _DEBUG_
        DevConsole::log(QString("Category %1").arg(cat->tabText(cat->currentIndex())), "Debug");
        #endif

        QScrollArea *currentFrame   = getFrameTilesetOfTab(current);
        QTabBar     *currentCombo   = getGroupComboboxOfTab(current);
        if(!currentFrame || !currentCombo)
            return;

        QWidget *scrollWid = currentFrame->widget();
        if(!scrollWid)
            return;

        qDeleteAll(scrollWid->findChildren<QGroupBox *>());

        if(scrollWid->layout() == nullptr)
            scrollWid->setLayout(new FlowLayout());

        currentFrame->setWidgetResizable(true);

        #ifdef _DEBUG_
        DevConsole::log(QString("size %1 %2")
                        .arg(scrollWid->layout()->geometry().width())
                        .arg(scrollWid->layout()->geometry().height())
                        , "Debug");
        #endif

        QString currentGroup = currentCombo->tabText(currentCombo->currentIndex());
        currentGroup.replace("&&", "&");//Restore non-escaped ampersands
        for(int i = 0; i < mw()->configs.main_tilesets_grp.size(); i++)
        {
            if((mw()->configs.main_tilesets_grp[i].groupCat == category)
               && (mw()->configs.main_tilesets_grp[i].groupName == currentGroup)) //category
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
                            QGroupBox *tilesetNameWrapper = new QGroupBox(s.tileSetName, scrollWid);
                            ((FlowLayout *)scrollWid->layout())->addWidget(tilesetNameWrapper);
                            QGridLayout *l = new QGridLayout(tilesetNameWrapper);
                            l->setContentsMargins(4, 4, 4, 4);
                            l->setSpacing(2);
                            for(int k = 0; k < s.items.size(); k++)
                            {
                                SimpleTilesetItem &item = s.items[k];
                                TilesetItemButton *tbutton = new TilesetItemButton(&mw()->configs, scene, tilesetNameWrapper);
                                tbutton->applySize(32, 32);
                                tbutton->applyItem(s.type, item.id);
                                l->addWidget(tbutton, item.row, item.col);
                                connect(tbutton, SIGNAL(clicked(int, ulong)), mw(), SLOT(SwitchPlacingItem(int, ulong)));
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
        QWidget *current = cat->widget(tabIndex);
        if(!current)
            return;

        QScrollArea *currentFrame = getFrameTilesetOfTab(current);
        //        QComboBox* currentCombo = getGroupComboboxOfTab(current);
        //        if(!currentFrame || !currentCombo)
        //            return;

        QWidget *scrollWid = currentFrame->widget();

        if(!scrollWid)
            return;

        qDeleteAll(scrollWid->findChildren<QGroupBox *>());

        if(scrollWid->layout() == nullptr)
            scrollWid->setLayout(new FlowLayout());

        currentFrame->setWidgetResizable(true);

        #ifdef _DEBUG_
        DevConsole::log(QString("size %1 %2")
                        .arg(scrollWid->layout()->geometry().width())
                        .arg(scrollWid->layout()->geometry().height())
                        , "Debug");
        #endif

        QVector<SimpleTileset> ctsets = loadCustomTilesets();
        if(!ctsets.isEmpty())
        {
            for(int j = 0; j < ctsets.size(); j++)
            {
                unsigned int mostRighter = 0;
                SimpleTileset &s = ctsets[j];
                if(!s.tileSetName.contains(ui->customTilesetSearchEdit->text(), Qt::CaseInsensitive))
                    continue;

                QGroupBox *tilesetNameWrapper = new QGroupBox(s.tileSetName, scrollWid);
                ((FlowLayout *)scrollWid->layout())->addWidget(tilesetNameWrapper);
                QGridLayout *l = new QGridLayout(tilesetNameWrapper);
                l->setContentsMargins(4, 4, 4, 4);
                l->setSpacing(2);
                for(int k = 0; k < s.items.size(); k++)
                {
                    SimpleTilesetItem &item = s.items[k];
                    TilesetItemButton *tbutton = new TilesetItemButton(&mw()->configs, scene, tilesetNameWrapper);
                    tbutton->applySize(32, 32);
                    tbutton->applyItem(s.type, (int)item.id);
                    l->addWidget(tbutton, (int)item.row, (int)item.col);
                    if(item.col >= mostRighter)
                        mostRighter = item.col + 1;
                    connect(tbutton, SIGNAL(clicked(int, ulong)), mw(), SLOT(SwitchPlacingItem(int, ulong)));
                }
                QPushButton *b = new QPushButton(Themes::icon(Themes::pencil), "", tilesetNameWrapper);
                b->setProperty("tileset-file-name", s.fileName);
                b->setMaximumSize(32, 32);
                b->setFlat(true);
                l->addWidget(b, 0, (int)mostRighter);
                connect(b, SIGNAL(clicked()), this, SLOT(editSelectedTileset()));
            }
        }
    }
}

QVector<SimpleTileset> TilesetItemBox::loadCustomTilesets()
{
    QVector<SimpleTileset> ctsets;

    QString path;
    QString cfolder;
    bool doIt = false;
    if(mw()->activeChildWindow() == 1)
    {
        path = mw()->activeLvlEditWin()->LvlData.meta.path + "/";
        cfolder = mw()->activeLvlEditWin()->LvlData.meta.filename + "/";
        doIt = !mw()->activeLvlEditWin()->LvlData.meta.untitled;
    }
    else if(mw()->activeChildWindow() == 3)
    {
        path = mw()->activeWldEditWin()->WldData.meta.path + "/";
        cfolder = mw()->activeWldEditWin()->WldData.meta.filename + "/";
        doIt = !mw()->activeWldEditWin()->WldData.meta.untitled;
    }

    if(doIt)
    {
        QStringList paths;
        paths << path << path + cfolder;

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
                    xxx.customDir = (p == path + cfolder);
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

    QTabWidget *cat = ui->TileSetsCategories;
    makeSelectedTileset(cat->currentIndex());
}

void TilesetItemBox::makeAllTilesets()
{
    if(lockTilesetBox) return;

    using namespace pge_tilesetbox;
    QTabWidget *cat = ui->TileSetsCategories;

    #ifdef _DEBUG_
    DevConsole::log(QString("index %1 count %2").arg(current).arg(cat->count()), "Debug");
    #endif

    for(int i = 0; i < cat->count(); i++)
        makeSelectedTileset(i);

    if(cat->count() > current)
        cat->setCurrentIndex(current);

    #ifdef _DEBUG_
    DevConsole::log(QString("index %1 count %2").arg(current).arg(cat->count()), "Debug");
    #endif
}
