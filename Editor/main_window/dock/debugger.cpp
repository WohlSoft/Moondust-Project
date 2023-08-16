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
    setAttribute(Qt::WA_ShowWithoutActivating);
    ui->setupUi(this);
    m_isLoaded = false;

    QRect mwg = mw()->geometry();
    int GOffset = 10;
    mw()->addDockWidget(Qt::RightDockWidgetArea, this);
    connect(mw(), SIGNAL(languageSwitched()), this, SLOT(re_translate()));
    connect(this, SIGNAL(visibilityChanged(bool)), mw()->ui->actionDebugger, SLOT(setChecked(bool)));
    connect(this, SIGNAL(visibilityChanged(bool)), this, SLOT(on_DEBUG_RefreshCoutners_clicked()));
    setFloating(true);
    setGeometry(
        mwg.right() - width() - GOffset,
        mwg.y() + 120,
        width(),
        height()
    );

    m_lastVisibilityState = isVisible();
    mw()->docks_level_and_world.
    addState(this, &m_lastVisibilityState);

    QFont font("Monospace");
    font.setStyleHint(QFont::TypeWriter);
    font.setWeight(QFont::Normal);
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

    if(mw()->activeChildWindow(mw()->LastActiveSubWindow) == MainWindow::WND_Level)
    {
        mw()->activeLvlEditWin(mw()->LastActiveSubWindow)->goTo(
            ui->DEBUG_GotoX->text().toInt(),
            ui->DEBUG_GotoY->text().toInt(), true, QPoint(0, 0), true);
    }
    else if(mw()->activeChildWindow() == MainWindow::WND_World)
    {
        mw()->activeWldEditWin(mw()->LastActiveSubWindow)->goTo(
            ui->DEBUG_GotoX->text().toInt(),
            ui->DEBUG_GotoY->text().toInt(), true, QPoint(0, 0), true);

    }

    ui->DEBUG_GotoX->setText("");
    ui->DEBUG_GotoY->setText("");
}

void DebuggerBox::Debugger_loadCustomCounters()
{
    QString userDFile = AppPathManager::settingsPath() + "/" + util::filePath(ConfStatus::configName) + " counters.ini";
    QString debuggerFile = userDFile;
    if(!QFile::exists(debuggerFile))
    {
        debuggerFile = mw()->configs.config_dir + "/counters.ini";
    }

reload:
    QSettings cCounters(debuggerFile, QSettings::IniFormat);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    cCounters.setIniCodec("UTF-8");
#endif

    {
        cCounters.beginGroup("custom-counters");
        int cntn = cCounters.value("total", 0).toInt();
        cCounters.endGroup();

        if(cntn <= 0)
        {
            if(debuggerFile == userDFile)
            {
                debuggerFile = mw()->configs.config_dir + "/counters.ini";
                goto reload;
            }
            else
                goto quit;
        }

        m_customCounters.clear();

        for(; cntn > 0; cntn--)
        {
            bool valid = true;
            CustomCounter counter;
            QStringList items;
            cCounters.beginGroup(QString("custom-counter-%1").arg(cntn));
            counter.name = cCounters.value("name", "").toString();
            if(counter.name.isEmpty()) valid = false;
            counter.type = (ItemTypes::itemTypes)cCounters.value("type", 0).toInt();
            counter.windowType = cCounters.value("window-type", 0).toInt();
            QString tmp = cCounters.value("items", "").toString();
            if(tmp.isEmpty())
            {
                valid = false;
                goto skip;
            }
            items = tmp.split(',');
            foreach(QString x, items)
            {
                counter.items.push_back(x.toLong());
            }
skip:
            cCounters.endGroup();
            if(valid) m_customCounters.push_back(counter);
        }
    }
quit:
    m_isLoaded = true;
}


void DebuggerBox::Debugger_saveCustomCounters()
{
    if(!m_isLoaded) return;

    QString debuggerFile = AppPathManager::settingsPath() + "/" + util::filePath(ConfStatus::configName) + " counters.ini";

    QSettings cCounters(debuggerFile, QSettings::IniFormat);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    cCounters.setIniCodec("UTF-8");
#endif
    cCounters.clear();

    cCounters.beginGroup("custom-counters");
    cCounters.setValue("total", m_customCounters.size());
    cCounters.endGroup();
    for(int i = 0; i < m_customCounters.size(); i++)
    {
        cCounters.beginGroup(QString("custom-counter-%1").arg(i + 1));
        cCounters.setValue("name", m_customCounters[i].name);
        cCounters.setValue("type", (int)m_customCounters[i].type);
        cCounters.setValue("window-type", m_customCounters[i].windowType);
        QString items;
        for(int j = 0; j < m_customCounters[i].items.size(); j++)
        {
            if(j == m_customCounters[i].items.size() - 1)
                items.append(QString::number(m_customCounters[i].items[j]));
            else
                items.append(QString::number(m_customCounters[i].items[j]) + ",");
        }
        cCounters.setValue("items", items);
        cCounters.endGroup();
    }
}


void DebuggerBox::on_DEBUG_AddCustomCounter_clicked()
{
    CustomCounterGUI customCounterGui(mw());
    customCounterGui.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    customCounterGui.setGeometry(util::alignToScreenCenter(customCounterGui.size()));

    if(customCounterGui.exec() == QDialog::Accepted)
    {
        m_customCounters.push_back(customCounterGui.counterData);
        on_DEBUG_RefreshCoutners_clicked();
    }
}


void DebuggerBox::on_DEBUG_RefreshCoutners_clicked()
{
    if(!isVisible()) return;
    if(!m_isLoaded)
        Debugger_loadCustomCounters();

    int win = mw()->activeChildWindow(mw()->LastActiveSubWindow);

    qDebug() << "Size of custom conters array:" << m_customCounters.size();
    util::memclear(ui->DEBUG_CustomCountersList);
    for(int i = 0; i < m_customCounters.size(); i++)
    {
        if(m_customCounters[i].windowType != win) continue;
        QListWidgetItem *item;
        item = new QListWidgetItem(ui->DEBUG_CustomCountersList);
        item->setData(Qt::UserRole, QVariant(i));

        int countItems = 0;
        if(mw()->activeChildWindow() == MainWindow::WND_Level)
        {
            if(m_customCounters[i].type == ItemTypes::LVL_NPC)
            {
                LevelEdit *e = mw()->activeLvlEditWin(mw()->LastActiveSubWindow);
                if(e)
                {
                    if(!m_customCounters[i].items.isEmpty())
                    {
                        //Deep search of NPC's
                        for(int j = 0; j < e->LvlData.npc.size(); j++)
                        {
                            foreach(long q, m_customCounters[i].items)
                            {
                                //check as regular NPC
                                if(e->LvlData.npc[j].id == (unsigned)q)
                                {
                                    countItems++;
                                    break;
                                }
                                //check as NPC-Container
                                if((e->LvlData.npc[j].id > 0) && e->scene->m_localConfigNPCs.contains(e->LvlData.npc[j].id))
                                {
                                    obj_npc &t_npc = e->scene->m_localConfigNPCs[ e->LvlData.npc[j].id ];
                                    if(t_npc.setup.container)
                                    {
                                        if(e->LvlData.npc[j].contents == q)
                                        {
                                            countItems++;
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                        //find all NPC's in blocks
                        for(int j = 0; j < e->LvlData.blocks.size(); j++)
                        {
                            foreach(long q, m_customCounters[i].items)
                            {
                                if((unsigned)q == mw()->configs.marker_npc.coin_in_block)
                                {
                                    if(e->LvlData.blocks[j].npc_id < 0)
                                    {
                                        countItems += e->LvlData.blocks[j].npc_id * -1;
                                        break;
                                    }
                                }
                                else if(e->LvlData.blocks[j].npc_id == q)
                                {
                                    countItems++;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            else if(m_customCounters[i].type == ItemTypes::LVL_Block)
            {
                LevelEdit *e = mw()->activeLvlEditWin(mw()->LastActiveSubWindow);
                if(e)
                    if(!m_customCounters[i].items.isEmpty())
                    {
                        for(int j = 0; j < e->LvlData.blocks.size(); j++)
                        {
                            foreach(long q, m_customCounters[i].items)
                            {
                                if(e->LvlData.blocks[j].id == (unsigned)q)
                                {
                                    countItems++;
                                    break;
                                }
                            }
                        }
                    }
            }
            else if(m_customCounters[i].type == ItemTypes::LVL_BGO)
            {
                LevelEdit *e = mw()->activeLvlEditWin(mw()->LastActiveSubWindow);
                if(e)
                    if(!m_customCounters[i].items.isEmpty())
                    {
                        for(int j = 0; j < e->LvlData.bgo.size(); j++)
                        {
                            foreach(long q, m_customCounters[i].items)
                            {
                                if(e->LvlData.bgo[j].id == (unsigned)q)
                                {
                                    countItems++;
                                    break;
                                }
                            }
                        }
                    }
            }
        }
        else if(mw()->activeChildWindow() == MainWindow::WND_World)
        {
            if(m_customCounters[i].type == ItemTypes::WLD_Tile)
            {
                WorldEdit *e = mw()->activeWldEditWin(mw()->LastActiveSubWindow);
                if(!e) return;
                if(!m_customCounters[i].items.isEmpty())
                {
                    for(int j = 0; j < e->WldData.tiles.size(); j++)
                    {
                        foreach(long q, m_customCounters[i].items)
                        {
                            if(e->WldData.tiles[j].id == (unsigned)q)
                            {
                                countItems++;
                                break;
                            }
                        }
                    }
                }
            }
            else if(m_customCounters[i].type == ItemTypes::WLD_Scenery)
            {
                WorldEdit *e = mw()->activeWldEditWin(mw()->LastActiveSubWindow);
                if(e)
                    if(!m_customCounters[i].items.isEmpty())
                    {
                        for(int j = 0; j < e->WldData.scenery.size(); j++)
                        {
                            foreach(long q, m_customCounters[i].items)
                            {
                                if(e->WldData.scenery[j].id == (unsigned)q)
                                {
                                    countItems++;
                                    break;
                                }
                            }
                        }
                    }
            }
            else if(m_customCounters[i].type == ItemTypes::WLD_Path)
            {
                WorldEdit *e = mw()->activeWldEditWin(mw()->LastActiveSubWindow);
                if(e)
                    if(!m_customCounters[i].items.isEmpty())
                    {
                        for(int j = 0; j < e->WldData.paths.size(); j++)
                        {
                            foreach(long q, m_customCounters[i].items)
                            {
                                if(e->WldData.paths[j].id == (unsigned)q)
                                {
                                    countItems++;
                                    break;
                                }
                            }
                        }
                    }
            }
            else if(m_customCounters[i].type == ItemTypes::WLD_Level)
            {
                WorldEdit *e = mw()->activeWldEditWin(mw()->LastActiveSubWindow);
                if(e)
                    if(!m_customCounters[i].items.isEmpty())
                    {
                        for(int j = 0; j < e->WldData.levels.size(); j++)
                        {
                            foreach(long q, m_customCounters[i].items)
                            {
                                if(e->WldData.levels[j].id == (unsigned)q)
                                {
                                    countItems++;
                                    break;
                                }
                            }
                        }
                    }
            }
            else if(m_customCounters[i].type == ItemTypes::WLD_MusicBox)
            {
                WorldEdit *e = mw()->activeWldEditWin(mw()->LastActiveSubWindow);
                if(e)
                    if(!m_customCounters[i].items.isEmpty())
                    {
                        for(int j = 0; j < e->WldData.music.size(); j++)
                        {
                            foreach(long q, m_customCounters[i].items)
                            {
                                if(e->WldData.music[j].id == (unsigned)q)
                                {
                                    countItems++;
                                    break;
                                }
                            }
                        }
                    }
            }

        }
        item->setText(QString("%1\t%2").arg(countItems).arg(m_customCounters[i].name));
        ui->DEBUG_CustomCountersList->addItem(item);
    }

}


void DebuggerBox::on_DEBUG_CustomCountersList_itemClicked(QListWidgetItem *item)
{
    Q_UNUSED(item);
    int itemID = item->data(Qt::UserRole).toInt();
    on_DEBUG_RefreshCoutners_clicked();
    foreach(QListWidgetItem *x, ui->DEBUG_CustomCountersList->findItems("*", Qt::MatchWildcard))
    {
        if(x->data(Qt::UserRole).toInt() == itemID)
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
    QAction *edit = menu.addAction(tr("Edit"));
    QAction *remove = menu.addAction(tr("Remove"));
    QAction *selected = menu.exec(ui->DEBUG_CustomCountersList->mapToGlobal(pos));
    if(selected == NULL) return;
    if(selected == remove)
    {
        m_customCounters.remove(itemID);
        on_DEBUG_RefreshCoutners_clicked();
    }
    else if(selected == edit)
    {
        CustomCounterGUI customCounterGui(mw());
        customCounterGui.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
        customCounterGui.setGeometry(util::alignToScreenCenter(customCounterGui.size()));
        customCounterGui.setCounterData(m_customCounters[itemID]);

        if(customCounterGui.exec() == QDialog::Accepted)
        {
            m_customCounters[itemID] = customCounterGui.counterData;
            on_DEBUG_RefreshCoutners_clicked();
        }
    }

}

