#include "pge_editorplugininfo.h"
#include "ui_pge_editorplugininfo.h"

PGE_EditorPluginInfo::PGE_EditorPluginInfo(PGE_EditorPluginManager *pluginManager, QWidget *parent) :
    QDialog(parent),
    m_pluginManager(pluginManager),
    ui(new Ui::PGE_EditorPluginInfo)
{
    ui->setupUi(this);

    for(unsigned int i = 0; i < pluginManager->getPluginCount(); ++i){
        const PGE_EditorPluginItem& nextItem = pluginManager->getPluginInfo(i);
        QListWidgetItem* nextDisplayItem = new QListWidgetItem(nextItem.pluginName());
        nextDisplayItem->setData(UserRolePluginIndex, QVariant(i));
        nextDisplayItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
        nextDisplayItem->setCheckState(Qt::Checked);
        ui->pluginList->addItem(nextDisplayItem);
    }

    connect(ui->pluginButtonBox, &QDialogButtonBox::accepted, this, &PGE_EditorPluginInfo::accept);
    connect(ui->pluginButtonBox, &QDialogButtonBox::rejected, this, &PGE_EditorPluginInfo::reject);


    connect(ui->pluginList, &QListWidget::currentItemChanged,
            [this](QListWidgetItem* current, QListWidgetItem*){
        const PGE_EditorPluginItem& pluginDetails = m_pluginManager->getPluginInfo(current->data(UserRolePluginIndex).toInt());
        ui->textInfo->setText(tr("Name: ") + pluginDetails.pluginName() + "\n"
                              + tr("Author: ") + pluginDetails.authorName() + "\n"
                              + tr("Version: ") + QString::number(pluginDetails.pluginVersionNum()) + "\n"
                              + tr("Description: ") + pluginDetails.description());
    });
}

PGE_EditorPluginInfo::~PGE_EditorPluginInfo()
{
    delete ui;
}
