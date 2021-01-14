/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2021 Vitaly Novichkov <admin@wohlnet.ru>
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
#include <tools/tilesets/tileset_editor.h>
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

    m_lockSettings = false;

    QRect dg = util::getScreenGeometry();
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
    m_lockSettings = true;
    ui->retranslateUi(this);
    m_lockSettings = false;
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
    if(m_lockSettings) return;
    if((!this->isVisible()) && (!force)) return; //!< Don't update invisible

    pge_tilesetbox::current = ui->TileSetsCategories->currentIndex();

    // Clean up old stuff
    clearTilesetGroups();
    // Generate blank groups and categories
    prepareCategoriesAndGroups();
    // Fill them with tilesets
    makeAllTilesets();
}



void TilesetItemBox::on_tilesetGroup_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    if(!mw())
        return;
#ifdef _DEBUG_
    WriteToLog(QtDebugMsg, "TilesetBox -> change combobox's index");
#endif
    if(m_lockSettings)
        return;
    makeCurrentTileset();
}



void TilesetItemBox::on_newTileset_clicked()
{
    // THREAD TEST
    //future = QtConcurrent::run(loopForever); //<! Tiny test with thread
    //QMessageBox::information(this, "test", "test", QMessageBox::Ok);

    bool untitled = false;
    if(mw()->activeChildWindow() == MainWindow::WND_Level)
        untitled = mw()->activeLvlEditWin()->isUntitled();
    else if(mw()->activeChildWindow() == MainWindow::WND_World)
        untitled = mw()->activeWldEditWin()->isUntitled();

    if(untitled)
    {
        QMessageBox::warning(this, tr("File not saved"),
                             tr("File doesn't saved on disk."), QMessageBox::Ok);
        return;
    }

    TilesetEditor *tilesetConfDia;

    if(mw()->activeChildWindow() == MainWindow::WND_Level)
        tilesetConfDia = new TilesetEditor(&mw()->configs, mw()->activeLvlEditWin()->scene, mw());
    else if(mw()->activeChildWindow() == MainWindow::WND_World)
        tilesetConfDia = new TilesetEditor(&mw()->configs, mw()->activeWldEditWin()->scene, mw());
    else
        tilesetConfDia = new TilesetEditor(&mw()->configs, nullptr, mw());

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
    TilesetEditor *tilesetConfDia;

    if(mw()->activeChildWindow() == MainWindow::WND_Level)
        tilesetConfDia = new TilesetEditor(&mw()->configs, mw()->activeLvlEditWin()->scene, mw());
    else if(mw()->activeChildWindow() == MainWindow::WND_World)
        tilesetConfDia = new TilesetEditor(&mw()->configs, mw()->activeWldEditWin()->scene, mw());
    else
        tilesetConfDia = new TilesetEditor(&mw()->configs, nullptr, mw());

    util::DialogToCenter(tilesetConfDia);

    auto *b = qobject_cast<QPushButton *>(sender());
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

    auto *box = qobject_cast<QGroupBox *>(parent);
    if(!box)
    {
        delete tilesetConfDia;
        return;
    }

    QString fileName = b->property("tileset-file-name").toString();//box->title();
    QVector<SimpleTileset> cTileSets = loadCustomTilesets();
    for(auto &tileSet : cTileSets)
    {
        if(tileSet.fileName == fileName)
        {
            tilesetConfDia->loadSimpleTileset(tileSet, tileSet.customDir);
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
    return catTab->findChild<QTabBar *>();
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
    QTabBar *tileSetGroup;
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

    tileSetGroup = new QTabBar(catWid);
    tileSetGroup->setExpanding(false);

    catLayout->addWidget(tileSetGroup, 0, 1, 1, 1);
    //tileSetGroup->setInsertPolicy(QTabBar::InsertAlphabetically);
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

    tileSetGroup->setProperty("menuButton", QVariant::fromValue(menuButton));
    tileSetGroup->setProperty("menu", QVariant::fromValue(menuGroups));

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

void TilesetItemBox::prepareCategoriesAndGroups()
{
    for(const SimpleTilesetCachedCategory &cat : mw()->configs.main_tileset_categogies)
    {
        QWidget *t = findTabWidget(cat.name);
        if(!t)
            makeCategory(cat.name);
    }

    const QList<SimpleTilesetGroup > &t_groups = mw()->configs.main_tilesets_grp;
    for(const SimpleTilesetGroup &grp : t_groups)
        prepareTilesetGroup(grp);
}

void TilesetItemBox::prepareTilesetGroup(const SimpleTilesetGroup &tilesetGroups)
{
    if(m_lockSettings) return;

    QWidget *t = findTabWidget(tilesetGroups.groupCat);
    if(!t)
        t = makeCategory(tilesetGroups.groupCat);
    QTabBar *c = getGroupComboboxOfTab(t);
    if(!c)
        return;
    auto *cMenu = qvariant_cast<QMenu*>(c->property("menu"));
    if(!util::contains(c, tilesetGroups.groupName))
    {
        QString g = tilesetGroups.groupName;
        g.replace("&", "&&");
        int tabIndex = c->addTab(g);//Escaped ampersands
        QAction *a = cMenu->addAction(g);
        a->setData(tabIndex);
        c->connect(a, reinterpret_cast<void(QAction::*)(bool)>(&QAction::triggered),
                   [c,a](bool){ c->setCurrentIndex(a->data().toInt()); }
        );
    }
    //c->model()->sort(0);
    c->setCurrentIndex(0);
    c->connect(c, SIGNAL(currentChanged(int)), this, SLOT(on_tilesetGroup_currentIndexChanged(int)));
}

void TilesetItemBox::clearTilesetGroups()
{
    if(m_lockSettings) return;

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
    if(m_lockSettings)
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
        for(auto &tsGroup : mw()->configs.main_tilesets_grp)
        {
            if((tsGroup.groupCat == category)
               && (tsGroup.groupName == currentGroup)) //category
            {
#ifdef _DEBUG_
                DevConsole::log(QString("Group %1").arg(tsGroup.groupName), "Debug");
                DevConsole::log(QString("Tilesets %1").arg(tsGroup.tilesets.size()), "Debug");
#endif

                QStringList l = tsGroup.tilesets;
                foreach(QString s, l)
                {
                    for(auto &tileSet : mw()->configs.main_tilesets)
                    {
                        if(s == tileSet.fileName)
                        {
                            auto *tileSetNameWrapper = new QGroupBox(tileSet.tileSetName, scrollWid);
                            ((FlowLayout *)scrollWid->layout())->addWidget(tileSetNameWrapper);
                            auto *tsLayout = new QGridLayout(tileSetNameWrapper);
                            tsLayout->setContentsMargins(4, 4, 4, 4);
                            tsLayout->setSpacing(2);
                            for(int k = 0; k < tileSet.items.size(); k++)
                            {
                                SimpleTilesetItem &item = tileSet.items[k];
                                auto *tButton = new TilesetItemButton(&mw()->configs, scene, tileSetNameWrapper);
                                tButton->applySize(32, 32);
                                tButton->applyItem(tileSet.type, item.id);
                                tsLayout->addWidget(tButton, item.row, item.col);
                                connect(tButton, SIGNAL(clicked(int, ulong)), mw(), SLOT(SwitchPlacingItem(int, ulong)));
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

        QVector<SimpleTileset> cTileSets = loadCustomTilesets();
        if(!cTileSets.isEmpty())
        {
            for(auto &ts : cTileSets)
            {
                unsigned int mostRighter = 0;
                if(!ts.tileSetName.contains(ui->customTilesetSearchEdit->text(), Qt::CaseInsensitive))
                    continue;

                auto *tilesetNameWrapper = new QGroupBox(ts.tileSetName, scrollWid);
                ((FlowLayout *)scrollWid->layout())->addWidget(tilesetNameWrapper);
                auto *l = new QGridLayout(tilesetNameWrapper);
                l->setContentsMargins(4, 4, 4, 4);
                l->setSpacing(2);
                for(int k = 0; k < ts.items.size(); k++)
                {
                    SimpleTilesetItem &item = ts.items[k];
                    auto *tButton = new TilesetItemButton(&mw()->configs, scene, tilesetNameWrapper);
                    tButton->applySize(32, 32);
                    tButton->applyItem(ts.type, (int)item.id);
                    l->addWidget(tButton, (int)item.row, (int)item.col);
                    if(item.col >= mostRighter)
                        mostRighter = item.col + 1;
                    connect(tButton, SIGNAL(clicked(int, ulong)), mw(), SLOT(SwitchPlacingItem(int, ulong)));
                }
                QPushButton *b = new QPushButton(Themes::icon(Themes::pencil), "", tilesetNameWrapper);
                b->setProperty("tileset-file-name", ts.fileName);
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
    if(mw()->activeChildWindow() == MainWindow::WND_Level)
    {
        path = mw()->activeLvlEditWin()->LvlData.meta.path + "/";
        cfolder = mw()->activeLvlEditWin()->LvlData.meta.filename + "/";
        doIt = !mw()->activeLvlEditWin()->LvlData.meta.untitled;
    }
    else if(mw()->activeChildWindow() == MainWindow::WND_World)
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
    if(m_lockSettings) return;

    QTabWidget *cat = ui->TileSetsCategories;
    makeSelectedTileset(cat->currentIndex());
}

void TilesetItemBox::makeAllTilesets()
{
    if(m_lockSettings) return;

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
