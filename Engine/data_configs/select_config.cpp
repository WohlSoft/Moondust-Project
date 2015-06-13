/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2015 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "select_config.h"
#include <ui_select_config.h>

#include <QDir>
#include <QFileInfo>

#include "../common_features/app_path.h"

#include "../common_features/util.h"
#include "../common_features/graphics_funcs.h"

SelectConfig::SelectConfig(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectConfig)
{

    ui->setupUi(this);
    QListWidgetItem * item;

    currentConfig = "";
    themePack = "";

    typedef QPair<QString, QString > configPackPair;
    QList<configPackPair > config_paths;

    QString configPath(ApplicationPath+"/configs/");
    QDir configDir(configPath);

    QStringList configs = configDir.entryList(QDir::AllDirs);

    foreach(QString c, configs)
    {
        QString config_dir = configPath+c+"/";
        configPackPair path;
        path.first = c;//Full path
        path.second = config_dir;//name of config dir
        config_paths<<path;
    }

    if(AppPathManager::userDirIsAvailable())
    {
        QString configPath_user = AppPathManager::userAppDir()+"/configs/";//!< User additional folder
        QDir configUserDir(configPath_user);
        configs=configUserDir.entryList(QDir::AllDirs);
        foreach(QString c, configs)
        {
            QString config_dir = configPath_user+c+"/";
            configPackPair path;
            path.first = c;//Full path
            path.second = config_dir;//name of config dir
            config_paths<<path;
        }
    }

    foreach(configPackPair confD, config_paths)
    {
        QString c=confD.first;
        QString config_dir=confD.second;
        QString configName;
        QString data_dir;
        QString splash_logo;

        QString description;

        QString gui_ini = config_dir + "main.ini";

        if(!QFileInfo(gui_ini).exists()) continue; //Skip if it is not a config
        QSettings guiset(gui_ini, QSettings::IniFormat);
        guiset.setIniCodec("UTF-8");

        guiset.beginGroup("gui");
            splash_logo = guiset.value("editor-splash", "").toString();
            themePack = guiset.value("default-theme", "").toString();
        guiset.endGroup();

        guiset.beginGroup("main");
            data_dir = (guiset.value("application-dir", "0").toBool() ?
                            ApplicationPath + "/" : config_dir + "data/" );
            configName = guiset.value("config_name", QDir(config_dir).dirName()).toString();
            description = guiset.value("config_desc", config_dir).toString();
        guiset.endGroup();

        //Default splash image
        if(splash_logo.isEmpty())
            splash_logo = ":/images/splash_editor.png";
        else
        {
            splash_logo = data_dir + splash_logo;
            if(QPixmap(splash_logo).isNull())
            {
                splash_logo = ":/images/splash_editor.png";
            }
        }

        item = new QListWidgetItem( configName );
        QPixmap iconImg(200,114);
        iconImg.fill(QColor(Qt::white));
        iconImg = GraphicsHelps::squareImage(QPixmap(splash_logo), QSize(200,114));
        item->setIcon( QIcon( iconImg ) );
        item->setData(Qt::ToolTipRole, description);
        item->setData(Qt::UserRole, c);
        item->setData(Qt::UserRole+4, config_dir);
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );

        ui->configList->addItem( item );
    }

}

SelectConfig::~SelectConfig()
{
    util::memclear(ui->configList);
    delete ui;
}

///
/// \brief isPreLoaded
/// \return defined working config. If is empty, show config selecting dialog
///
QString SelectConfig::isPreLoaded(QString openConfig)
{
    QString configPath = openConfig;

    if(QFileInfo(openConfig+"/main.ini").exists())
        currentConfig=configPath;
//    //check exists of config in list
//    foreach(QListWidgetItem * it, ui->configList->findItems(QString("*"), Qt::MatchWrap | Qt::MatchWildcard))
//    {
//        if(configPath.isEmpty())
//        {
//            currentConfig = ""; break;
//        }
//        if(it->data(Qt::UserRole+4).toString()==configPath)
//        {
//            currentConfig = it->data(Qt::UserRole).toString();
//            currentConfigPath = it->data(Qt::UserRole+4).toString();
//            break;
//        }
//    }

    return currentConfig;
}

void SelectConfig::on_configList_itemDoubleClicked(QListWidgetItem *item)
{
    currentConfig = item->data(Qt::UserRole).toString();
    currentConfigPath = item->data(Qt::UserRole+4).toString();
    this->accept();
}

void SelectConfig::on_buttonBox_accepted()
{
    if(ui->configList->selectedItems().isEmpty()) return;
    currentConfig = ui->configList->selectedItems().first()->data(Qt::UserRole).toString();
    currentConfigPath = ui->configList->selectedItems().first()->data(Qt::UserRole+4).toString();
    this->accept();
}

