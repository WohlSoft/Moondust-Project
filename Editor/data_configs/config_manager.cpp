#include "config_manager.h"
#include <ui_config_manager.h>

#include "../common_features/app_path.h"

#include "../common_features/util.h"
#include "../common_features/graphics_funcs.h"

#include <QDir>
#include <QMessageBox>

ConfigManager::ConfigManager(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigManager)
{
    ui->setupUi(this);

    QListWidgetItem * item;

    currentConfig = "";
    themePack = "";
    askAgain = false;

    QString configPath(ApplicationPath+"/configs/");
    QDir configDir(configPath);
    QStringList configs = configDir.entryList(QDir::AllDirs);

    foreach(QString c, configs)
    {
        QString config_dir = configPath+c+"/";
        QString configName;
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

        item->setIcon( QIcon( GraphicsHelps::squareImage(QPixmap(splash_logo), QSize(70,40)) ) );
        item->setData(3, c);
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );

        ui->configList->addItem( item );
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
                    .arg(ApplicationPath+"/configs")
                    .arg("<a href=\"http://engine.wohlnet.ru/config_packs.php\">"
                         "http://engine.wohlnet.ru/config_packs.php"
                         "</a>")
                    );
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
    QString inifile = ApplicationPath + "/" + "pge_editor.ini";
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
        if(it->data(3).toString()==configPath)
        {
            currentConfig = configPath;
            it->setSelected(true);
            ui->configList->scrollToItem(it);
            break;
        }
    }

    ui->AskAgain->setChecked(askAgain);

    return currentConfig;
}

void ConfigManager::setAskAgain(bool _x)
{
    askAgain = _x;
    ui->AskAgain->setChecked(_x);
}

void ConfigManager::on_configList_itemDoubleClicked(QListWidgetItem *item)
{
    currentConfig = item->data(3).toString();
    askAgain = ui->AskAgain->isChecked();
    this->accept();
}

void ConfigManager::on_buttonBox_accepted()
{
    if(ui->configList->selectedItems().isEmpty()) return;
    currentConfig = ui->configList->selectedItems().first()->data(3).toString();
    askAgain = ui->AskAgain->isChecked();
    this->accept();
}
