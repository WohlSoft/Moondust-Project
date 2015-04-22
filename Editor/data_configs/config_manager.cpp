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

#include <QDir>
#include <QMessageBox>
#include <QDesktopWidget>
#ifdef Q_OS_WIN
#include <QtWin>
#endif

#include <common_features/app_path.h>
#include <common_features/util.h>
#include <common_features/graphics_funcs.h>

#include "config_manager.h"
#include <ui_config_manager.h>

#include <_includes/GlobalDefines>

#include <QPainter>
#include <QAbstractItemDelegate>

class ListDelegate : public QAbstractItemDelegate
{
public:
    ListDelegate(QObject *parent = 0);

    void paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
    QSize sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const;

    virtual ~ListDelegate();
};

ListDelegate::ListDelegate(QObject *parent)
    : QAbstractItemDelegate(parent) {}

ListDelegate::~ListDelegate() {}

void ListDelegate::paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    QRect r = option.rect;

    //Color: #C4C4C4
    QPen linePen(QColor::fromRgb(211,211,211), 2, Qt::SolidLine);

    //Color: #005A83
    QPen lineMarkedPen(QColor::fromRgb(0,90,131), 1, Qt::SolidLine);

    //Color: #333
    QPen fontPen(QColor::fromRgb(51,51,51), 1, Qt::SolidLine);

    //Color: #fff
    QPen fontMarkedPen(Qt::white, 1, Qt::SolidLine);

    //BORDER
    painter->setPen(linePen);
    painter->drawLine(r.topLeft(),r.topRight());
    painter->drawLine(r.topRight(),r.bottomRight());
    painter->drawLine(r.bottomLeft(),r.bottomRight());
    painter->drawLine(r.topLeft(),r.bottomLeft());

    if(option.state & QStyle::State_Selected)
    {
        QLinearGradient gradientSelected(r.left(),r.top(),r.left(),r.height()+r.top());
        gradientSelected.setColorAt(0.0, QColor::fromRgb(119,213,247));
        gradientSelected.setColorAt(0.9, QColor::fromRgb(27,134,183));
        gradientSelected.setColorAt(1.0, QColor::fromRgb(0,120,174));
        painter->setBrush(gradientSelected);
        painter->drawRect(r);

        //BORDER
        painter->setPen(lineMarkedPen);
        painter->drawLine(r.topLeft(),r.topRight());
        painter->drawLine(r.topRight(),r.bottomRight());
        painter->drawLine(r.bottomLeft(),r.bottomRight());
        painter->drawLine(r.topLeft(),r.bottomLeft());

        painter->setPen(fontMarkedPen);
    }
    else
    {
        //BACKGROUND
        //ALTERNATING COLORS
        painter->setBrush( (index.row() % 2) ? Qt::white : QColor(252,252,252) );
        painter->drawRect(r);

        //BORDER
        painter->setPen(linePen);
        painter->drawLine(r.topLeft(),r.topRight());
        painter->drawLine(r.topRight(),r.bottomRight());
        painter->drawLine(r.bottomLeft(),r.bottomRight());
        painter->drawLine(r.topLeft(),r.bottomLeft());

        painter->setPen(fontPen);
    }

    //GET TITLE, DESCRIPTION AND ICON
    QIcon ic = QIcon(qvariant_cast<QPixmap>(index.data(Qt::DecorationRole)));
    QString title = index.data(Qt::DisplayRole).toString();
    QString description = index.data(Qt::UserRole + 1).toString();

    int imageSpace = 10;
    if (!ic.isNull())
    {
        //ICON
        r = option.rect.adjusted(5, 10, -10, -10);
        ic.paint(painter, r, Qt::AlignVCenter|Qt::AlignLeft);
        imageSpace = 80;
    }

    //TITLE
    r = option.rect.adjusted(imageSpace, 0, -10, -30);
    painter->setFont( QFont( "Lucida Grande", PGEDefaultFontSize+2, QFont::Bold ) );
    painter->drawText(r.left(), r.top(), r.width(), r.height(), Qt::AlignBottom|Qt::AlignLeft, title, &r);

    //DESCRIPTION
    r = option.rect.adjusted(imageSpace, 30, -10, 30);
    painter->setFont( QFont( "Lucida Grande", PGEDefaultFontSize, QFont::Normal ) );
    painter->drawText(r.left(), r.top(), r.width(), r.height(), Qt::AlignLeft, description, &r);
}

QSize ListDelegate::sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    Q_UNUSED(index);
    Q_UNUSED(option);
    return QSize(200, 60); // very dumb value
}










ConfigManager::ConfigManager(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigManager)
{
    ui->setupUi(this);

    QListWidgetItem * item;


    #ifdef Q_OS_MAC
    this->setWindowIcon(QIcon(":/cat_builder.icns"));
    #endif
    #ifdef Q_OS_WIN
    this->setWindowIcon(QIcon(":/cat_builder.ico"));

    if(QtWin::isCompositionEnabled())
    {
        this->setAttribute(Qt::WA_TranslucentBackground, true);
        QtWin::extendFrameIntoClientArea(this, -1,-1,-1,1);
        QtWin::enableBlurBehindWindow(this);
    }
    else
    {
        QtWin::resetExtendedFrame(this);
        setAttribute(Qt::WA_TranslucentBackground, false);
    }
    #endif

    connect(ui->configList, SIGNAL(clicked(QModelIndex)), ui->configList, SLOT(update()));

    currentConfig = "";
    themePack = "";
    askAgain = false;

    typedef QPair<QString, QString > configPackPair;
    QList<configPackPair > config_paths;

    QString configPath;
    QString configPath_user;

    QStringList configs;

    configPath=(ApplicationPath+"/configs/");//!< Stuff PGE config dir
    QDir configDir(configPath);
    configs=configDir.entryList(QDir::AllDirs);
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
        configPath_user = AppPathManager::userAppDir()+"/configs/";//!< User additional folder
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

    ui->configList->setItemDelegate(new ListDelegate(ui->configList));
    ui->configList->setStyleSheet("{paint-alternating-row-colors-for-empty-area:1;"
                                  "alternate-background-color: #2C2C2C;}");
    foreach(configPackPair confD, config_paths)
    {
        QString c=confD.first;
        QString config_dir=confD.second;
        QString configName;
        QString configDesc;
        bool smbx_compatible;
        QString data_dir;
        QString splash_logo;

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
            configDesc = guiset.value("config_desc", config_dir).toString();
            smbx_compatible =  guiset.value("smbx-compatible", false).toBool();
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
        item->setData(Qt::DecorationRole, GraphicsHelps::squareImage(QPixmap(splash_logo), QSize(70,40)));
        item->setData(3, c);
        item->setData(Qt::UserRole + 1, configDesc);
        item->setData(Qt::UserRole + 2, smbx_compatible);
        item->setData(Qt::UserRole + 4, config_dir);
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );
        ui->configList->addItem( item);
    }

    //Warning message: if no installed config packs
    if(ui->configList->findItems(QString("*"), Qt::MatchWrap | Qt::MatchWildcard).isEmpty())
    {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle(tr("Config packs are not found"));
        msgBox.setTextFormat(Qt::RichText); //this is what makes the links clickable
        msgBox.setText(
                    tr("Available configuration packages are not found!<br>\n"
                       "Please download and install them into directory<br>\n<br>\n%1<br>\n<br>\n"
                       "You can take any configuration package here:<br>%2")
                    .arg(AppPathManager::userAppDir()+"/configs")
                    .arg("<a href=\"http://engine.wohlnet.ru/config_packs.php\">"
                         "http://engine.wohlnet.ru/config_packs.php"
                         "</a>")
                    );
        QSize mSize = msgBox.sizeHint();
        QRect screenRect = QDesktopWidget().screen()->rect();
        msgBox.move( QPoint( screenRect.width()/2 - mSize.width()/2,
            screenRect.height()/2 - mSize.height()/2 ) );
        msgBox.setIcon(QMessageBox::Warning);

        msgBox.exec();
    }

}

ConfigManager::~ConfigManager()
{
    util::memclear(ui->configList);
    delete ui;
}

///
/// \brief isPreLoaded
/// \return defined working config. If is empty, show config selecting dialog
///
QString ConfigManager::isPreLoaded()
{
    QString inifile = AppPathManager::settingsFile();
    QSettings settings(inifile, QSettings::IniFormat);

    settings.beginGroup("Main");
    QString configPath = settings.value("current-config", "").toString();
    QString saved_theme = settings.value("current-theme", "").toString();;
    askAgain = settings.value("ask-config-again", false).toBool();
    settings.endGroup();

    if(!saved_theme.isEmpty())
        themePack = saved_theme;
    //check exists of config in list
    foreach(QListWidgetItem * it, ui->configList->findItems(QString("*"), Qt::MatchWrap | Qt::MatchWildcard))
    {
        if(configPath.isEmpty())
        {
            currentConfig = ""; break;
        }

        //If full config pack path is same
        if(it->data(Qt::UserRole+4).toString()==configPath)
        {
            currentConfig = it->data(3).toString();
            currentConfigPath = it->data(Qt::UserRole+4).toString();
            it->setSelected(true);
            ui->configList->scrollToItem(it);
            break;
        }
    }

    ui->AskAgain->setChecked(askAgain);

    return currentConfigPath;
}

void ConfigManager::setAskAgain(bool _x)
{
    askAgain = _x;
    ui->AskAgain->setChecked(_x);
}

void ConfigManager::on_configList_itemDoubleClicked(QListWidgetItem *item)
{
    currentConfig = item->data(3).toString();
    currentConfigPath = item->data(Qt::UserRole+4).toString();
    askAgain = ui->AskAgain->isChecked();
    this->accept();
}

void ConfigManager::on_buttonBox_accepted()
{
    if(ui->configList->selectedItems().isEmpty()) return;
    currentConfig = ui->configList->selectedItems().first()->data(3).toString();
    currentConfigPath = ui->configList->selectedItems().first()->data(Qt::UserRole+4).toString();
    askAgain = ui->AskAgain->isChecked();
    this->accept();
}
