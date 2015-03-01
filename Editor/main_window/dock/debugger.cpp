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

#include <QSettings>

#include <common_features/app_path.h>
#include <common_features/util.h>
#include <main_window/global_settings.h>
#include <tools/debugger/custom_counter_gui.h>

#include "../../mainwindow.h"
#include <ui_mainwindow.h>

#include "debugger.h"
#include "ui_debugger.h"

DebuggerBox::DebuggerBox(QWidget *parent) :
    QDockWidget(parent),
    MWDock_Base(parent),
    ui(new Ui::DebuggerBox)
{
    setVisible(false);
    ui->setupUi(this);
    isLoaded=false;

    QRect mwg = mw()->geometry();
    int GOffset=240;
    mw()->addDockWidget(Qt::RightDockWidgetArea, this);
    connect(mw(), SIGNAL(languageSwitched()), this, SLOT(re_translate()));
    setFloating(true);
    setGeometry(
                mwg.x()+mwg.width()-width()-GOffset,
                mwg.y()+120,
                width(),
                height()
                );

    mw()->docks_level_and_world.
          addState(this, &GlobalSettings::DebuggerBoxVis,
            &setVisible, &isVisible);

    QFont font("Monospace");
    font.setStyleHint(QFont::TypeWriter);
    font.setWeight(8);
    ui->DEBUG_Items->setFont(font);

}

DebuggerBox::~DebuggerBox()
{
    delete ui;
}

void DebuggerBox::re_translate()
{
    ui->retranslateUi(this);
}

void DebuggerBox::on_DebuggerBox_visibilityChanged(bool visible)
{
    mw()->ui->actionDebugger->setChecked(visible);
    if(visible)
        on_DEBUG_RefreshCoutners_clicked();
}

void MainWindow::on_actionDebugger_triggered(bool checked)
{
    dock_DebuggerBox->setVisible(checked);
    if(checked) dock_DebuggerBox->raise();
}

void DebuggerBox::setMousePos(QPoint p, bool isOffScreen)
{
    if(!mw()->ui->actionDebugger->isVisible()) return; //SpeedUp

    if(isOffScreen)
    {
        ui->DEBUG_MouseX->setText("");
        ui->DEBUG_MouseY->setText("");
    }
    else
    {
        ui->DEBUG_MouseX->setText(QString::number(p.x()));
        ui->DEBUG_MouseY->setText(QString::number(p.y()));
    }
}

void DebuggerBox::setItemStat(QString list)
{
    if(!mw()->ui->actionDebugger->isVisible()) return; //SpeedUp

    ui->DEBUG_Items->setText(list);
}

void DebuggerBox::on_DEBUG_GotoPoint_clicked()
{
    if(ui->DEBUG_GotoX->text().isEmpty()) return;
    if(ui->DEBUG_GotoY->text().isEmpty()) return;

    if(mw()->activeChildWindow(mw()->LastActiveSubWindow)==1)
    {
        mw()->activeLvlEditWin(mw()->LastActiveSubWindow)->goTo(
                    ui->DEBUG_GotoX->text().toInt(),
                    ui->DEBUG_GotoY->text().toInt(), true, QPoint(0,0), true);
    }
    else if(mw()->activeChildWindow()==3)
    {
        mw()->activeWldEditWin(mw()->LastActiveSubWindow)->goTo(
                    ui->DEBUG_GotoX->text().toInt(),
                    ui->DEBUG_GotoY->text().toInt(), true, QPoint(0,0), true);

    }

    ui->DEBUG_GotoX->setText("");
    ui->DEBUG_GotoY->setText("");
}

void DebuggerBox::Debugger_loadCustomCounters()
{
    QString userDFile=AppPathManager::userAppDir()+"/"+util::filePath(ConfStatus::configName)+" counters.ini";
    QString debuggerFile = userDFile;
    if(!QFile(debuggerFile).exists())
    {
        debuggerFile = mw()->configs.config_dir+"/counters.ini";
    }
    reload:
    QSettings cCounters(debuggerFile, QSettings::IniFormat);
    cCounters.setIniCodec("UTF-8");

    {
        cCounters.beginGroup("custom-counters");
            int cntn = cCounters.value("total", 0).toInt();
        cCounters.endGroup();

        if(cntn<=0)
        {
            if(debuggerFile==userDFile)
            {
                debuggerFile = mw()->configs.config_dir+"/counters.ini";
                goto reload;
            }
            else
                goto quit;
        }

        customCounters.clear();

        for(;cntn>0;cntn--)
        {
            bool valid=true;
            CustomCounter counter;
            QStringList items;
            cCounters.beginGroup(QString("custom-counter-%1").arg(cntn));
                counter.name = cCounters.value("name", "").toString();
                    if(counter.name.isEmpty()) valid=false;
                counter.type = (ItemTypes::itemTypes)cCounters.value("type", 0).toInt();
                counter.windowType = cCounters.value("window-type", 0).toInt();
                QString tmp = cCounters.value("items", "").toString();
                    if(tmp.isEmpty()) {valid=false; goto skip;}
                    items = tmp.split(',');
                    foreach(QString x, items)
                    {
                        counter.items.push_back(x.toLong());
                    }
                    skip:
            cCounters.endGroup();
            if(valid) customCounters.push_back(counter);
        }
    }
    quit:
    isLoaded=true;
}


void DebuggerBox::Debugger_saveCustomCounters()
{
    if(!isLoaded) return;

    QString debuggerFile = AppPathManager::userAppDir()+"/"+util::filePath(ConfStatus::configName)+" counters.ini";

    QSettings cCounters(debuggerFile, QSettings::IniFormat);
    cCounters.setIniCodec("UTF-8");
    cCounters.clear();

    cCounters.beginGroup("custom-counters");
        cCounters.setValue("total", customCounters.size());
    cCounters.endGroup();
    for(int i=0; i<customCounters.size();i++)
    {
        cCounters.beginGroup(QString("custom-counter-%1").arg(i+1));
            cCounters.setValue("name", customCounters[i].name);
            cCounters.setValue("type", (int)customCounters[i].type);
            cCounters.setValue("window-type", customCounters[i].windowType);
            QString items;
            for(int j=0;j<customCounters[i].items.size();j++)
            {
                if(j==customCounters[i].items.size()-1)
                    items.append(QString::number(customCounters[i].items[j]));
                else
                    items.append(QString::number(customCounters[i].items[j])+",");
            }
            cCounters.setValue("items", items);
        cCounters.endGroup();
    }
}


void DebuggerBox::on_DEBUG_AddCustomCounter_clicked()
{
    CustomCounterGUI customCounterGui;
    customCounterGui.setWindowFlags (Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    customCounterGui.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, customCounterGui.size(), qApp->desktop()->availableGeometry()));

    if(customCounterGui.exec()==QDialog::Accepted)
    {
        customCounters.push_back(customCounterGui.counterData);
        on_DEBUG_RefreshCoutners_clicked();
    }
}


void DebuggerBox::on_DEBUG_RefreshCoutners_clicked()
{
    if(!isVisible()) return;
    if(!isLoaded)
        Debugger_loadCustomCounters();

    int win = mw()->activeChildWindow(mw()->LastActiveSubWindow);

    qDebug() << "Size of custom conters array:" << customCounters.size();
    util::memclear(ui->DEBUG_CustomCountersList);
    for(int i=0;i<customCounters.size();i++)
    {
        if(customCounters[i].windowType!=win) continue;
        QListWidgetItem *item;
        item = new QListWidgetItem(ui->DEBUG_CustomCountersList);
        item->setData(Qt::UserRole, QVariant(i));

        int countItems=0;
        if(mw()->activeChildWindow()==1)
        {
            if(customCounters[i].type==ItemTypes::LVL_NPC)
            {
                LevelEdit * e = mw()->activeLvlEditWin(mw()->LastActiveSubWindow);
                if(e)
                if(!customCounters[i].items.isEmpty())
                {
                    //Deep search of NPC's
                    for(int j=0;j<e->LvlData.npc.size();j++)
                    {
                        foreach(long q, customCounters[i].items)
                        {
                            //check as regular NPC
                            if(e->LvlData.npc[j].id==(unsigned)q) {countItems++; break;}
                            //check as NPC-Container
                            int nI = mw()->configs.getNpcI(e->LvlData.npc[j].id);
                            if(nI>=0)
                            {
                                if(mw()->configs.main_npc[nI].container)
                                    if(e->LvlData.npc[j].special_data==q) {countItems++; break;}
                            }
                        }
                    }
                    //find all NPC's in blocks
                    for(int j=0;j<e->LvlData.blocks.size();j++)
                    {
                        foreach(long q, customCounters[i].items)
                        {
                            if((unsigned)q==mw()->configs.marker_npc.coin_in_block) {
                                if(e->LvlData.blocks[j].npc_id<0)
                                    { countItems+=e->LvlData.blocks[j].npc_id*-1; break;} }
                            else if(e->LvlData.blocks[j].npc_id==q) {countItems++; break;}
                        }
                    }
                }
            }
            else
            if(customCounters[i].type==ItemTypes::LVL_Block)
            {
                LevelEdit * e = mw()->activeLvlEditWin(mw()->LastActiveSubWindow);
                if(e)
                if(!customCounters[i].items.isEmpty())
                {
                    for(int j=0;j<e->LvlData.blocks.size();j++)
                    {
                        foreach(long q, customCounters[i].items)
                        { if(e->LvlData.blocks[j].id==(unsigned)q) {countItems++; break;} }
                    }
                }
            }
            else
            if(customCounters[i].type==ItemTypes::LVL_BGO)
            {
                LevelEdit * e = mw()->activeLvlEditWin(mw()->LastActiveSubWindow);
                if(e)
                if(!customCounters[i].items.isEmpty())
                {
                    for(int j=0;j<e->LvlData.bgo.size();j++)
                    {
                        foreach(long q, customCounters[i].items)
                        { if(e->LvlData.bgo[j].id==(unsigned)q) {countItems++; break;} }
                    }
                }
            }
        }
        else
        if(mw()->activeChildWindow()==3)
        {
            if(customCounters[i].type==ItemTypes::WLD_Tile)
            {
                WorldEdit * e = mw()->activeWldEditWin(mw()->LastActiveSubWindow);
                if(!e) return;
                if(!customCounters[i].items.isEmpty())
                {
                    for(int j=0;j<e->WldData.tiles.size();j++)
                    {
                        foreach(long q, customCounters[i].items)
                        { if(e->WldData.tiles[j].id==(unsigned)q) {countItems++; break;} }
                    }
                }
            }
            else
            if(customCounters[i].type==ItemTypes::WLD_Scenery)
            {
                WorldEdit * e = mw()->activeWldEditWin(mw()->LastActiveSubWindow);
                if(e)
                if(!customCounters[i].items.isEmpty())
                {
                    for(int j=0;j<e->WldData.scenery.size();j++)
                    {
                        foreach(long q, customCounters[i].items)
                        { if(e->WldData.scenery[j].id==(unsigned)q) {countItems++; break;} }
                    }
                }
            }
            else
            if(customCounters[i].type==ItemTypes::WLD_Path)
            {
                WorldEdit * e = mw()->activeWldEditWin(mw()->LastActiveSubWindow);
                if(e)
                if(!customCounters[i].items.isEmpty())
                {
                    for(int j=0;j<e->WldData.paths.size();j++)
                    {
                        foreach(long q, customCounters[i].items)
                        { if(e->WldData.paths[j].id==(unsigned)q) {countItems++; break;} }
                    }
                }
            }
            else
            if(customCounters[i].type==ItemTypes::WLD_Level)
            {
                WorldEdit * e = mw()->activeWldEditWin(mw()->LastActiveSubWindow);
                if(e)
                if(!customCounters[i].items.isEmpty())
                {
                    for(int j=0;j<e->WldData.levels.size();j++)
                    {
                        foreach(long q, customCounters[i].items)
                        { if(e->WldData.levels[j].id==(unsigned)q) {countItems++; break;} }
                    }
                }
            }
            else
            if(customCounters[i].type==ItemTypes::WLD_MusicBox)
            {
                WorldEdit * e = mw()->activeWldEditWin(mw()->LastActiveSubWindow);
                if(e)
                if(!customCounters[i].items.isEmpty())
                {
                    for(int j=0;j<e->WldData.music.size();j++)
                    {
                        foreach(long q, customCounters[i].items)
                        { if(e->WldData.music[j].id==(unsigned)q) {countItems++; break;} }
                    }
                }
            }

        }
        item->setText(QString("%1\t%2").arg(countItems).arg(customCounters[i].name));
        ui->DEBUG_CustomCountersList->addItem(item);
    }

}


void DebuggerBox::on_DEBUG_CustomCountersList_itemClicked(QListWidgetItem *item)
{
    Q_UNUSED(item);
    int itemID = item->data(Qt::UserRole).toInt();
    on_DEBUG_RefreshCoutners_clicked();
    foreach(QListWidgetItem* x, ui->DEBUG_CustomCountersList->findItems("*", Qt::MatchWildcard))
    {
        if(x->data(Qt::UserRole).toInt()==itemID)
        {
            ui->DEBUG_CustomCountersList->scrollToItem(x);
            x->setSelected(true);
            break;
        }
    }
}


void DebuggerBox::on_DEBUG_CustomCountersList_customContextMenuRequested(const QPoint &pos)
{
    if(ui->DEBUG_CustomCountersList->selectedItems().isEmpty()) return;

    QListWidgetItem *item = ui->DEBUG_CustomCountersList->selectedItems().first();
    int itemID = item->data(Qt::UserRole).toInt();

    QMenu menu;
    QAction* edit = menu.addAction(tr("Edit"));
    QAction* remove = menu.addAction(tr("Remove"));
    QAction* selected = menu.exec( ui->DEBUG_CustomCountersList->mapToGlobal(pos) );
    if(selected==NULL) return;
    if(selected==remove)
    {
        customCounters.remove(itemID);
        on_DEBUG_RefreshCoutners_clicked();
    }
    else
    if(selected==edit)
    {
        CustomCounterGUI customCounterGui;
        customCounterGui.setWindowFlags (Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
        customCounterGui.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, customCounterGui.size(), qApp->desktop()->availableGeometry()));
        customCounterGui.setCounterData(customCounters[itemID]);

        if(customCounterGui.exec()==QDialog::Accepted)
        {
            customCounters[itemID] = customCounterGui.counterData;
            on_DEBUG_RefreshCoutners_clicked();
        }
    }

}

