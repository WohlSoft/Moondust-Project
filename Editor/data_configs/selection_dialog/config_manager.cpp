/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2024 Vitaly Novichkov <admin@wohlnet.ru>
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
#ifdef Q_OS_WIN
#include <QtWin>
#include <QSysInfo>
#endif

#include <common_features/app_path.h>
#include <common_features/util.h>
#include <common_features/graphics_funcs.h>
#include <main_window/global_settings.h>

#include "config_manager.h"
#include <ui_config_manager.h>

#include <_includes/GlobalDefines>

#include <QPainter>
#include <QAbstractItemDelegate>

#include <js_engine/pge_jsengine.h>
#include <js_engine/proxies/js_common.h>
#include <js_engine/proxies/js_file.h>
#include <js_engine/proxies/js_ini.h>
#include <js_engine/proxies/js_system.h>

#include "../data_configs.h"
#include "version.h"

#define CONFIGURE_TOOL_NAME "configure.js"

#define CP_NAME_ROLE         (Qt::ToolTipRole)
#define CP_DESC_ROLE         (Qt::UserRole + 1)
#define CP_SMBX64FLAG_ROLE   (Qt::UserRole + 2)
#define CP_FULLDIR_ROLE      (Qt::UserRole + 3)
#define CP_INCOMPATIBLE_ROLE (Qt::UserRole + 4)
#define CP_API_VERSION_ROLE  (Qt::UserRole + 5)
#define CP_HOMEPAGE_ROLE     (Qt::UserRole + 6)

enum CPIncompatibleLevel
{
    CPIncompatible_Compatible = 0,
    CPIncompatible_OutdatedApi,
    CPIncompatible_IncompatibleApi,
    CPIncompatible_IncompatibleIntegrat,
    CPIncompatible_IncompatibleX2,
};

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

    //Color: #f00
    QPen fontPenNotice(QColor::fromRgb(0xFF, 0, 0), 1, Qt::SolidLine);

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
    CPIncompatibleLevel incompat = static_cast<CPIncompatibleLevel>(index.data(CP_INCOMPATIBLE_ROLE).toInt());
    QString incompatString;

    switch(incompat)
    {
    case CPIncompatible_OutdatedApi:
        incompatString = ConfigManager::tr("Outdated", "Level of config pack incompatibility. Outdated means it MAY work, but some minor errors are possible.");
        break;
    case CPIncompatible_IncompatibleApi:
        incompatString = ConfigManager::tr("Legacy, incompatible", "Level of config pack incompatibility. Legacy means this config pack is too old to work on this version of Editor.");
        break;
    case CPIncompatible_IncompatibleIntegrat:
        incompatString = ConfigManager::tr("Incompatible integrational", "Level of config pack incompatibility. An obsolete and incompatible integrational config package.");
        break;
    case CPIncompatible_IncompatibleX2:
        incompatString = ConfigManager::tr("Incompatible", "Level of config pack incompatibility. Incompatible, means, it's a totally incompatible config pack, probably, created for a modified version of the Moondust.");
        break;
    default:
        break;
    }

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

    if(incompat > CPIncompatible_Compatible)
    {
        //Incompatibility notice
        painter->save();
        painter->setPen(fontPenNotice);
        painter->setBrush(QColor(252, 0, 0));
        r = option.rect.adjusted(imageSpace_l, 0, -imageSpace_r, -30);
        painter->setFont(QFont("Lucida Grande", PGEDefaultFontSize, QFont::Normal));
        painter->drawText(r.left(), r.top(), r.width(), r.height(), Qt::AlignRight, incompatString, &r);
        painter->restore();
    }
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

    setGeometry(util::alignToScreenCenter(size()));

#ifdef Q_OS_MAC
    this->setWindowIcon(QIcon(":/appicon/app.icns"));
    ui->frame->setAutoFillBackground(false);
    ui->frame->setFrameShape(QFrame::NoFrame);
    ui->frame->setLineWidth(0);
#endif
#ifdef Q_OS_WIN
    this->setWindowIcon(QIcon(":/appicon/app.ico"));

    if(QSysInfo::WindowsVersion >= QSysInfo::WV_VISTA &&
       QSysInfo::WindowsVersion <= QSysInfo::WV_WINDOWS7)
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
        QString homePage;
        bool    cpSmbx64Flag;
        bool    cpIsX2;
        bool    cpIsIntegrat;
        int     apiVersion;
        CPIncompatibleLevel cpIncompatible = CPIncompatible_Compatible;
        QString data_dir;
        QString splash_logo;

        QString gui_ini = cpFullDirPath + "main.ini";

        cpIsIntegrat = QFile::exists(cpFullDirPath + CONFIGURE_TOOL_NAME);

        if(!QFileInfo(gui_ini).exists())
            continue; //Skip if it is not a config pack directory

        QSettings guiset(gui_ini, QSettings::IniFormat);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        guiset.setIniCodec("UTF-8");
#endif

        guiset.beginGroup("gui");
        splash_logo = guiset.value("editor-splash", "").toString();
        splash_logo = guiset.value("editor-icon", /* show splash if alternate icon is not defined */ splash_logo).toString();
        m_themePackName   = guiset.value("default-theme", "").toString();
        guiset.endGroup();

        guiset.beginGroup("main");
        data_dir = (guiset.value("application-dir", "0").toBool() ?
                    ApplicationPath + "/" :
                    cpFullDirPath + "data/");

        configName =    guiset.value("config_name", QDir(cpFullDirPath).dirName()).toString();
        cpDesc =        guiset.value("config_desc", cpFullDirPath).toString();
        cpSmbx64Flag =  guiset.value("smbx-compatible", false).toBool();
        apiVersion =    guiset.value("api-version", -1).toInt();
        cpIsX2 =        guiset.value("is-smbx2", false).toBool();
        homePage =      guiset.value("home-page", "https://wohlsoft.ru/config_packs/").toString();
        guiset.endGroup();

        QPixmap cpSplashImg;
        QString splashData = cpFullDirPath + "data/";

        //Default splash image
        if(splash_logo.isEmpty())
            splash_logo = ":/images/splash_editor.png";
        else
        {
            splash_logo = splashData + splash_logo;
            if(QPixmap(splash_logo).isNull())
                splash_logo = ":/images/splash_editor.png";
        }

        cpSplashImg.load(splash_logo);
        GraphicsHelps::squareImageR(cpSplashImg, QSize(70, 40));

        //Incompatibility level
        if(cpIsX2)
            cpIncompatible = CPIncompatible_IncompatibleX2;
        else if(cpIsIntegrat && apiVersion < 42)
            cpIncompatible = CPIncompatible_IncompatibleIntegrat;
        else if(apiVersion < 41)
            cpIncompatible = CPIncompatible_IncompatibleApi;
        else if(apiVersion < V_CP_API)
            cpIncompatible = CPIncompatible_OutdatedApi;

        item = new QListWidgetItem(configName);
        item->setData(Qt::DecorationRole,   cpSplashImg);
        item->setData(CP_NAME_ROLE,         cpName);
        item->setData(CP_DESC_ROLE,         cpDesc);
        item->setData(CP_SMBX64FLAG_ROLE,   cpSmbx64Flag);
        item->setData(CP_FULLDIR_ROLE,      cpFullDirPath);
        item->setData(CP_INCOMPATIBLE_ROLE, static_cast<int>(cpIncompatible));
        item->setData(CP_API_VERSION_ROLE,  apiVersion);
        item->setData(CP_HOMEPAGE_ROLE,     homePage);
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui->configList->addItem(item);
    }
}

void ConfigManager::saveCurrentSettings()
{
    QString inifile = AppPathManager::settingsFile();
    QSettings settings(inifile, QSettings::IniFormat);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    settings.setIniCodec("UTF-8");
#endif

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
            .arg("<a href=\"https://wohlsoft.ru/config_packs/\">"
                 "https://wohlsoft.ru/config_packs/"
                 "</a>")
        );
        QSize mSize = msgBox.sizeHint();
        QRect screenRect = util::getScreenGeometry();
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
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    settings.setIniCodec("UTF-8");
#endif

    settings.beginGroup("Main");
    QString configPath  = settings.value("current-config", QString()).toString();
    QString saved_theme = settings.value("current-theme", QString()).toString();
    m_doAskAgain        = settings.value("ask-config-again", false).toBool();
    settings.endGroup();

    if(!configPath.endsWith('/'))
        configPath.append('/');

    if(!saved_theme.isEmpty())
        m_themePackName = saved_theme;

    auto availableConfigs = ui->configList->findItems(QString("*"), Qt::MatchWrap | Qt::MatchWildcard);

    // Automatically choice a config pack if only one detected
    if(availableConfigs.size() == 1)
    {
        m_currentConfig       = availableConfigs[0]->data(CP_NAME_ROLE).toString();
        m_currentConfigPath   = availableConfigs[0]->data(CP_FULLDIR_ROLE).toString();
        availableConfigs[0]->setSelected(true);
        ui->configList->scrollToItem(availableConfigs[0]);
    }
    else for(QListWidgetItem *it : availableConfigs) //check exists of config in list
    {
        if(configPath.isEmpty())
        {
            m_currentConfig.clear();
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
    if(availableConfigs.size() == 1)
        m_doAskAgain = false;

    if(!verifyCompatibility())
        return QString();

    checkIsIntegrational();

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
    m_currentConfig         = item->data(CP_NAME_ROLE).toString();
    m_currentConfigPath     = item->data(CP_FULLDIR_ROLE).toString();
    m_currentIncompatLevel  = item->data(CP_INCOMPATIBLE_ROLE).toInt();
    m_currentAPIVersion     = item->data(CP_API_VERSION_ROLE).toInt();
    m_currentConfigHomeUrl  = item->data(CP_HOMEPAGE_ROLE).toString();
    m_doAskAgain            = ui->AskAgain->isChecked();

    if(verifyCompatibility())
        this->accept();
}

void ConfigManager::on_buttonBox_accepted()
{
    if(ui->configList->selectedItems().isEmpty())
        return;

    on_configList_itemDoubleClicked(ui->configList->selectedItems().first());
}

bool ConfigManager::isConfigured()
{
    QString settingsFile = DataConfig::buildLocalConfigPath(m_currentConfigPath);
    if(!QFile::exists(settingsFile))
        return false;

    QSettings settings(settingsFile, QSettings::IniFormat);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    settings.setIniCodec("UTF-8");
#endif

    settings.beginGroup("main");
    bool ret = settings.value("application-path-configured", false).toBool();
    QString path = settings.value("application-path", QString()).toString();
    settings.endGroup();

    // When directory got moved or deleted, config pack should be marked as not configured because got broken
    ret &= !path.isEmpty() && QFileInfo(path).isDir() && QDir(path).exists();

    return ret;
}

bool ConfigManager::checkForConfigureTool()
{
    //If configure tool has been detected
    if(ConfStatus::configIsIntegrational && (!isConfigured()))
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

void ConfigManager::checkIsIntegrational()
{
    ConfStatus::configConfigureTool = m_currentConfigPath + CONFIGURE_TOOL_NAME;
    ConfStatus::configIsIntegrational = QFile::exists(ConfStatus::configConfigureTool);
}

bool ConfigManager::verifyCompatibility()
{
    checkIsIntegrational();

    switch(static_cast<CPIncompatibleLevel>(m_currentIncompatLevel))
    {
    default:
        break;

    case CPIncompatible_OutdatedApi:
    {
        LogWarning(QString("Config pack version is outdated: "
                           "has API version: %1, wanted: %2").arg(m_currentAPIVersion).arg(V_CP_API));
        QMessageBox box;
        box.setWindowTitle(tr("Legacy configuration package", "Warning message box title"));
        box.setTextFormat(Qt::RichText);
#if (QT_VERSION >= 0x050100)
        box.setTextInteractionFlags(Qt::TextBrowserInteraction);
#endif
        box.setText(tr("You have a legacy configuration package.\n<br>"
                       "Editor will be started, but you may have a some problems with items or settings.\n<br>\n<br>"
                       "Please download and install latest version of a configuration package:\n<br>\n<br>Download: %1\n<br>"
                       "Note: most of config packs gets being updates with Moondust Project togeter. "
                       "Therefore, you can use the same link to download the updated version.")
                        .arg("<a href=\"%1\">%1</a>").arg(m_currentConfigHomeUrl));
        box.setStandardButtons(QMessageBox::Ok);
        box.setIcon(QMessageBox::Warning);
        box.exec();
        break;
    }

    case CPIncompatible_IncompatibleApi:
    {
        LogWarning(QString("Config pack version is incompatible: "
                           "has API version: %1, wanted: %2").arg(m_currentAPIVersion).arg(V_CP_API));
        QMessageBox box;
        box.setWindowTitle(tr("Incompatible configuration package", "Warning message box title"));
        box.setTextFormat(Qt::RichText);
#if (QT_VERSION >= 0x050100)
        box.setTextInteractionFlags(Qt::TextBrowserInteraction);
#endif
        box.setText(tr("You have a legacy and incompatible configuration package.\n<br>"
                       "This configuration package has the API version older than the minimum supported, "
                       "and therefore, it can't be used in this version of the Editor.<br>\n<br>\n"
                       "You can download the compatible configuration package at here:<br>\n"
                       "%1\n<br>\n<br>"
                       "Note: most of config packs gets being updates with Moondust Project togeter.\n"
                       "Therefore, you can use the same link to download the updated version.")
                        .arg("<a href=\"%1\">%1</a>").arg(m_currentConfigHomeUrl));
        box.setStandardButtons(QMessageBox::Ok);
        box.setIcon(QMessageBox::Warning);
        box.exec();
        return false;
    }

    case CPIncompatible_IncompatibleIntegrat:
        QMessageBox::warning(this,
                             tr("Integrational configuration package is incompatible"),
                             tr("This integrational configuration package is older than API version 42 and it "
                                "is no longer compatible with this version of Moondust Devkit. "
                                "Since API 42, integrational configuration packages must use the local settings "
                                "file instead the main.ini overriding."),
                             QMessageBox::Ok);
        return false;

    case CPIncompatible_IncompatibleX2:
    {
        QMessageBox box;
        box.setWindowTitle(tr("Incompatible configuration package", "Warning message box title"));
        box.setTextFormat(Qt::RichText);
#if (QT_VERSION >= 0x050100)
        box.setTextInteractionFlags(Qt::TextBrowserInteraction);
#endif
        box.setText(tr("You have an incompatible configuration package designed for the "
                       "SMBX2 project to use it with the modified version of the DevKit "
                       "which is incompatible to the mainstream.\n"
                       "To use this configuration package, you should use the compatible "
                       "Devkit maintained by developers of the SMBX2 project.<br>\n<br>\n"
                       "You can obtain the compatible Devkit by downloading the SMBX2 Beta5 or newer:<br>\n"
                       "%1\n<br>\n<br>\n"
                       "The Official Moondust Devkit has a support of the SMBX2 Beta4 and older only. "
                       "However, you still can run any other configuration package and run the level "
                       "tests using an SMBX2 engine from the outside.")
                        .arg("<a href=\"%1\">%1</a>").arg(m_currentConfigHomeUrl));
        box.setStandardButtons(QMessageBox::Ok);
        box.setIcon(QMessageBox::Warning);
        box.exec();
        return false;
    }
    }

    return checkForConfigureTool();
}

bool ConfigManager::runConfigureTool()
{
    QWidget *parentW = qobject_cast<QWidget *>(parent());
    if(!parentW || isVisible())
        parentW = this;

    if(ConfStatus::configIsIntegrational)
    {
        PGE_JsEngine js;
        // QString cpDirName = QDir(m_currentConfigPath).dirName();
        QString cpSetupFile = DataConfig::buildLocalConfigPath(m_currentConfigPath);

        js.bindProxy(new PGE_JS_Common(parentW), "PGE");
        js.bindProxy(new PGE_JS_File(m_currentConfigPath, cpSetupFile, parentW), "FileIO");
        js.bindProxy(new PGE_JS_INI(parentW), "INI");
        js.bindProxy(new PGE_JS_System(parentW), "System");

        bool successfulLoaded = false;
        js.loadFileByExpcetedResult<void>(ConfStatus::configConfigureTool, &successfulLoaded);

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
                                  .arg(ConfStatus::configConfigureTool),
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
