/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2019 Vitaly Novichkov <admin@wohlnet.ru>
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
#include <QSysInfo>
#endif

#include <common_features/app_path.h>
#include <common_features/util.h>
#include <common_features/graphics_funcs.h>

#include "config_manager.h"
#include <ui_config_manager.h>

#include <_includes/GlobalDefines>

#include <QPainter>
#include <QAbstractItemDelegate>

#include <js_engine/pge_jsengine.h>
#include <js_engine/proxies/js_common.h>
#include <js_engine/proxies/js_file.h>
#include <js_engine/proxies/js_ini.h>

#define CONFIGURE_TOOL_NAME "configure.js"

#define CP_NAME_ROLE        (Qt::ToolTipRole)
#define CP_DESC_ROLE        (Qt::UserRole+1)
#define CP_SMBX64FLAG_ROLE  (Qt::UserRole+2)
#define CP_FULLDIR_ROLE     (Qt::UserRole+3)

class ListDelegate : public QAbstractItemDelegate
{
public:
    ListDelegate(QObject *parent = 0);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

    virtual ~ListDelegate();
};

ListDelegate::ListDelegate(QObject *parent)
    : QAbstractItemDelegate(parent) {}

ListDelegate::~ListDelegate() {}

void ListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QRect r = option.rect;

    //Color: #C4C4C4
    QPen linePen(QColor::fromRgb(211, 211, 211), 2, Qt::SolidLine);

    //Color: #005A83
    QPen lineMarkedPen(QColor::fromRgb(0, 90, 131), 1, Qt::SolidLine);

    //Color: #333
    QPen fontPen(QColor::fromRgb(51, 51, 51), 1, Qt::SolidLine);

    //Color: #fff
    QPen fontMarkedPen(Qt::white, 1, Qt::SolidLine);

    //BORDER
    painter->setPen(linePen);
    painter->drawLine(r.topLeft(), r.topRight());
    painter->drawLine(r.topRight(), r.bottomRight());
    painter->drawLine(r.bottomLeft(), r.bottomRight());
    painter->drawLine(r.topLeft(), r.bottomLeft());

    if(option.state & QStyle::State_Selected)
    {
        QLinearGradient gradientSelected(r.left(), r.top(), r.left(), r.height() + r.top());
        gradientSelected.setColorAt(0.0, QColor::fromRgb(119, 213, 247));
        gradientSelected.setColorAt(0.9, QColor::fromRgb(27, 134, 183));
        gradientSelected.setColorAt(1.0, QColor::fromRgb(0, 120, 174));
        painter->setBrush(gradientSelected);
        painter->drawRect(r);

        //BORDER
        painter->setPen(lineMarkedPen);
        painter->drawLine(r.topLeft(), r.topRight());
        painter->drawLine(r.topRight(), r.bottomRight());
        painter->drawLine(r.bottomLeft(), r.bottomRight());
        painter->drawLine(r.topLeft(), r.bottomLeft());

        painter->setPen(fontMarkedPen);
    }
    else
    {
        //BACKGROUND
        //ALTERNATING COLORS
        painter->setBrush((index.row() % 2) ? Qt::white : QColor(252, 252, 252));
        painter->drawRect(r);

        //BORDER
        painter->setPen(linePen);
        painter->drawLine(r.topLeft(), r.topRight());
        painter->drawLine(r.topRight(), r.bottomRight());
        painter->drawLine(r.bottomLeft(), r.bottomRight());
        painter->drawLine(r.topLeft(), r.bottomLeft());

        painter->setPen(fontPen);
    }

    //GET TITLE, DESCRIPTION AND ICON
    QIcon ic = QIcon(qvariant_cast<QPixmap>(index.data(Qt::DecorationRole)));
    QString title       = index.data(Qt::DisplayRole).toString();
    QString description = index.data(CP_DESC_ROLE).toString();

    int imageSpace_l = 10, imageSpace_r = 10;
    if(!ic.isNull())
    {
        //ICON
        r = option.rect.adjusted(5, 10, -10, -10);
        ic.paint(painter, r, Qt::AlignVCenter | Qt::AlignLeft);
        if(qApp->layoutDirection() == Qt::RightToLeft)
            imageSpace_r = 80;
        else
            imageSpace_l = 80;
    }

    //TITLE
    r = option.rect.adjusted(imageSpace_l, 0, -imageSpace_r, -30);
    painter->setFont(QFont("Lucida Grande", PGEDefaultFontSize + 2, QFont::Bold));
    painter->drawText(r.left(), r.top(), r.width(), r.height(), Qt::AlignBottom | Qt::AlignLeft, title, &r);

    //DESCRIPTION
    r = option.rect.adjusted(imageSpace_l, 30, -imageSpace_r, 30);
    painter->setFont(QFont("Lucida Grande", PGEDefaultFontSize, QFont::Normal));
    painter->drawText(r.left(), r.top(), r.width(), r.height(), Qt::AlignLeft, description, &r);
}

QSize ListDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
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

    setWindowFlags(Qt::Window |
                   Qt::WindowTitleHint |
                   Qt::WindowCloseButtonHint |
                   Qt::WindowStaysOnTopHint);

    setGeometry(QStyle::alignedRect(Qt::LeftToRight,
                                    Qt::AlignCenter,
                                    size(),
                                    qApp->desktop()->availableGeometry()));

    #ifdef Q_OS_MAC
    this->setWindowIcon(QIcon(":/cat_builder.icns"));
    ui->frame->setAutoFillBackground(false);
    ui->frame->setFrameShape(QFrame::NoFrame);
    ui->frame->setLineWidth(0);
    #endif
    #ifdef Q_OS_WIN
    this->setWindowIcon(QIcon(":/cat_builder.ico"));

    if(QSysInfo::WindowsVersion >= QSysInfo::WV_VISTA)
    {
        if(QtWin::isCompositionEnabled())
        {
            this->setAttribute(Qt::WA_TranslucentBackground, true);
            QtWin::extendFrameIntoClientArea(this, -1, -1, -1, 1);
            QtWin::enableBlurBehindWindow(this);
        }
        else
        {
            QtWin::resetExtendedFrame(this);
            setAttribute(Qt::WA_TranslucentBackground, false);
        }
    }
    #endif

    connect(ui->configList, SIGNAL(clicked(QModelIndex)), ui->configList, SLOT(update()));
    connect(this, SIGNAL(accepted()), this, SLOT(saveCurrentSettings()));

    m_currentConfig = "";
    m_themePackName = "";
    m_doAskAgain    = false;
    loadConfigPackList();
}

ConfigManager::~ConfigManager()
{
    util::memclear(ui->configList);
    delete ui;
}

void ConfigManager::loadConfigPackList()
{
    typedef QPair<QString, QString > configPackPair;
    QList<configPackPair > config_paths;

    QListWidgetItem *item;

    //! Stuff PGE config dir
    QString     configPath      = ApplicationPath + "/configs/";
    //! User profile PGE config dir
    QString     configPath_user = AppPathManager::userAppDir() + "/configs/";

    //Create empty config directory if not exists
    if(!QDir(configPath).exists())
        QDir().mkdir(configPath);

    if(AppPathManager::userDirIsAvailable())
    {
        if(!QDir(configPath_user).exists())
            QDir().mkdir(configPath_user);
    }

    if(QDir(configPath).exists())
    {
        QDir configDir(configPath);
        QStringList configs = configDir.entryList(QDir::AllDirs);
        for(QString &cpName : configs)
        {
            QString config_dir = configPath + cpName + "/";
            configPackPair path;
            path.first  = cpName;       //name of config dir
            path.second = config_dir;   //Full path
            config_paths.append(path);
        }
    }

    if(AppPathManager::userDirIsAvailable())
    {
        QDir configUserDir(configPath_user);
        QStringList configs = configUserDir.entryList(QDir::AllDirs);
        for(QString &cpName : configs)
        {
            QString config_dir = configPath_user + cpName + "/";
            configPackPair path;
            path.first  = cpName;//Name of config dir
            path.second = config_dir;//Full path
            config_paths << path;
        }
    }

    ui->configList->setItemDelegate(new ListDelegate(ui->configList));
    ui->configList->setStyleSheet("* { paint-alternating-row-colors-for-empty-area: true; "
                                  "alternate-background-color: #2C2C2CFF; }");

    for(configPackPair &confD : config_paths)
    {
        QString cpName          = confD.first;
        QString cpFullDirPath   = confD.second;
        QString configName;
        QString cpDesc;
        bool    cpSmbx64Flag;
        QString data_dir;
        QString splash_logo;

        QString gui_ini = cpFullDirPath + "main.ini";

        if(!QFileInfo(gui_ini).exists())
            continue; //Skip if it is not a config pack directory

        QSettings guiset(gui_ini, QSettings::IniFormat);
        guiset.setIniCodec("UTF-8");

        guiset.beginGroup("gui");
        splash_logo = guiset.value("editor-splash", "").toString();
        splash_logo = guiset.value("editor-icon", /* show splash if alternate icon is not defined */ splash_logo).toString();
        m_themePackName   = guiset.value("default-theme", "").toString();
        guiset.endGroup();

        guiset.beginGroup("main");
        data_dir = (guiset.value("application-dir", "0").toBool() ?
                    ApplicationPath + "/" :
                    cpFullDirPath + "data/");

        configName = guiset.value("config_name", QDir(cpFullDirPath).dirName()).toString();
        cpDesc = guiset.value("config_desc", cpFullDirPath).toString();
        cpSmbx64Flag =  guiset.value("smbx-compatible", false).toBool();
        guiset.endGroup();

        QPixmap cpSplashImg;

        //Default splash image
        if(splash_logo.isEmpty())
            splash_logo = ":/images/splash_editor.png";
        else
        {
            splash_logo = data_dir + splash_logo;
            if(QPixmap(splash_logo).isNull())
                splash_logo = ":/images/splash_editor.png";
        }

        cpSplashImg.load(splash_logo);
        GraphicsHelps::squareImageR(cpSplashImg, QSize(70, 40));

        item = new QListWidgetItem(configName);
        item->setData(Qt::DecorationRole,   cpSplashImg);
        item->setData(CP_NAME_ROLE,         cpName);
        item->setData(CP_DESC_ROLE,         cpDesc);
        item->setData(CP_SMBX64FLAG_ROLE,   cpSmbx64Flag);
        item->setData(CP_FULLDIR_ROLE,      cpFullDirPath);
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui->configList->addItem(item);
    }
}

void ConfigManager::saveCurrentSettings()
{
    QString inifile = AppPathManager::settingsFile();
    QSettings settings(inifile, QSettings::IniFormat);
    if(!m_currentConfigPath.endsWith('/'))
        m_currentConfigPath.append('/');
    settings.beginGroup("Main");
    settings.setValue("current-config",     m_currentConfigPath);
    settings.setValue("ask-config-again",   m_doAskAgain);
    settings.endGroup();
}

bool ConfigManager::hasConfigPacks()
{
    //Warning message: if no installed config packs
    if(ui->configList->findItems(QString("*"), Qt::MatchWrap | Qt::MatchWildcard).isEmpty())
    {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle(tr("No config packs were found"));
        msgBox.setTextFormat(Qt::RichText); //this is what makes the links clickable
        #if (QT_VERSION >= 0x050100)
        msgBox.setTextInteractionFlags(Qt::TextBrowserInteraction);
        #endif
        msgBox.setText(
            tr("No configuration packages were found!<br>\n"
               "Please download and install them into this directory<br>\n<br>\n%1<br>\n<br>\n"
               "You can use any configuration package here:<br>%2")
            .arg(AppPathManager::userAppDir() + "/configs")
            .arg("<a href=\"http://wohlsoft.ru/config_packs/\">"
                 "http://wohlsoft.ru/config_packs/"
                 "</a>")
        );
        QSize mSize = msgBox.sizeHint();
        QRect screenRect = QDesktopWidget().screen()->rect();
        msgBox.move(QPoint(screenRect.width() / 2 - mSize.width() / 2,
                           screenRect.height() / 2 - mSize.height() / 2));
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();
        return false;
    }
    return true;
}

///
/// \brief isPreLoaded
/// \return defined working config. If is empty, show config selecting dialog
///
QString ConfigManager::loadConfigs()
{
    QString inifile = AppPathManager::settingsFile();

    QSettings settings(inifile, QSettings::IniFormat);

    settings.beginGroup("Main");
    QString configPath  = settings.value("current-config", "").toString();
    QString saved_theme = settings.value("current-theme", "").toString();
    m_doAskAgain        = settings.value("ask-config-again", false).toBool();
    settings.endGroup();

    if(!configPath.endsWith('/'))
        configPath.append('/');

    if(!saved_theme.isEmpty())
        m_themePackName = saved_theme;

    QList<QListWidgetItem *> AvailableConfigs = ui->configList->findItems(QString("*"), Qt::MatchWrap | Qt::MatchWildcard);

    //Automatically choice config pack if alone detected
    if(AvailableConfigs.size() == 1)
    {
        m_currentConfig       = AvailableConfigs[0]->data(CP_NAME_ROLE).toString();
        m_currentConfigPath   = AvailableConfigs[0]->data(CP_FULLDIR_ROLE).toString();
        AvailableConfigs[0]->setSelected(true);
        ui->configList->scrollToItem(AvailableConfigs[0]);
    }
    else

        //check exists of config in list
        for(QListWidgetItem *it : AvailableConfigs)
        {
            if(configPath.isEmpty())
            {
                m_currentConfig = "";
                break;
            }

            //If full config pack path is same
            if(it->data(CP_FULLDIR_ROLE).toString() == configPath)
            {
                m_currentConfig     = it->data(CP_NAME_ROLE).toString();
                m_currentConfigPath = it->data(CP_FULLDIR_ROLE).toString();
                it->setSelected(true);
                ui->configList->scrollToItem(it);
                break;
            }
        }
    ui->AskAgain->setChecked(m_doAskAgain);

    //Don't spawn dialog on load if alone config pack detected
    if(AvailableConfigs.size() == 1)
        m_doAskAgain = false;

    if((!m_currentConfigPath.isEmpty()) && (!m_doAskAgain))
    {
        if(!checkForConfigureTool())
            return QString();
    }

    return m_currentConfigPath;
}

void ConfigManager::setAskAgain(bool _x)
{
    m_doAskAgain = _x;
    ui->AskAgain->setChecked(_x);
}

void ConfigManager::on_configList_itemDoubleClicked(QListWidgetItem *item)
{
    m_currentConfig       = item->data(CP_NAME_ROLE).toString();
    m_currentConfigPath   = item->data(CP_FULLDIR_ROLE).toString();
    m_doAskAgain            = ui->AskAgain->isChecked();

    if(checkForConfigureTool())
        this->accept();
}

void ConfigManager::on_buttonBox_accepted()
{
    if(ui->configList->selectedItems().isEmpty()) return;
    m_currentConfig = ui->configList->selectedItems().first()->data(CP_NAME_ROLE).toString();
    m_currentConfigPath = ui->configList->selectedItems().first()->data(CP_FULLDIR_ROLE).toString();
    m_doAskAgain = ui->AskAgain->isChecked();

    if(checkForConfigureTool())
        this->accept();
}

bool ConfigManager::isConfigured()
{
    bool isConfigured = false;
    QSettings settings(m_currentConfigPath + "main.ini", QSettings::IniFormat);
    settings.beginGroup("main");
    isConfigured = settings.value("application-path-configured", false).toBool();
    settings.endGroup();
    return isConfigured;
}

bool ConfigManager::checkForConfigureTool()
{
    QString configureToolApp = m_currentConfigPath + CONFIGURE_TOOL_NAME;

    //If configure tool has been detected
    if(QFile::exists(configureToolApp) && (!isConfigured()))
    {
        QMessageBox::StandardButton reply =
            QMessageBox::information(this,
                                     tr("Configuration package is not configured!"),
                                     tr("\"%1\" configuration package is not configured yet.\n"
                                        "Do you want to configure it?")
                                     .arg(m_currentConfig),
                                     QMessageBox::Yes | QMessageBox::No);
        if(reply == QMessageBox::Yes)
            return runConfigureTool();
        else
            return false;
    }

    return true;
}


bool ConfigManager::runConfigureTool()
{
    QString configureToolApp = m_currentConfigPath + CONFIGURE_TOOL_NAME;
    QWidget *parentW = qobject_cast<QWidget *>(parent());
    if(!parentW || isVisible())
        parentW = this;

    if(QFile::exists(configureToolApp))
    {
        PGE_JsEngine js;
        js.bindProxy(new PGE_JS_Common(parentW), "PGE");
        js.bindProxy(new PGE_JS_File(m_currentConfigPath, parentW), "FileIO");
        js.bindProxy(new PGE_JS_INI(parentW), "INI");

        bool successfulLoaded = false;
        js.loadFileByExpcetedResult<void>(configureToolApp, &successfulLoaded);

        if(successfulLoaded)
        {
            setEnabled(false);
            if(!js.call<bool>("onConfigure", nullptr))
            {
                setEnabled(true);
                return false;
            }
            setEnabled(true);
            if(!isConfigured())
                return false;
            return true;
        }
        else
        {
            QMessageBox::critical(parentW,
                                  tr("Configuration script failed"),
                                  tr("Configuring tool encountered an error: %1 at line %2.\n"
                                     "File path: %3")
                                  .arg(js.getLastError())
                                  .arg(js.getLastErrorLine())
                                  .arg(configureToolApp),
                                  QMessageBox::Ok);
            return false;
        }
    }
    else
    {
        QMessageBox::information(parentW,
                                 tr("No configuration needed"),
                                 tr("This config pack has no configuring tool."),
                                 QMessageBox::Ok);
        return false;
    }
}
