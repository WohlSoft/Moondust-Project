#include "pge_editorplugininfo.h"
#include "ui_pge_editorplugininfo.h"

PGE_EditorPluginInfo::PGE_EditorPluginInfo(PGE_EditorPluginManager *pluginManager, QWidget *parent) :
    QDialog(parent),
    m_pluginManager(pluginManager),
    ui(new Ui::PGE_EditorPluginInfo)
{
    ui->setupUi(this);

    for(unsigned int i = 0; i < pluginManager->getPluginCount(); ++i) {
        const PGE_EditorPluginItem* nextItem = pluginManager->getPluginInfo(i);
        QString nextDisplayName = (nextItem->isValid() ? nextItem->pluginName() : QString("Package: ") + nextItem->packageName());
        QListWidgetItem* nextDisplayItem = new QListWidgetItem(nextDisplayName);
        nextDisplayItem->setData(UserRolePluginIndex, QVariant(i));
        nextDisplayItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
        nextDisplayItem->setCheckState(Qt::Checked);
        if(nextItem->isValid())
            nextDisplayItem->setForeground(Qt::blue);
        else
            nextDisplayItem->setForeground(Qt::red);
        ui->pluginList->addItem(nextDisplayItem);
    }

    connect(ui->pluginButtonBox, &QDialogButtonBox::accepted, this, &PGE_EditorPluginInfo::accept);
    connect(ui->pluginButtonBox, &QDialogButtonBox::rejected, this, &PGE_EditorPluginInfo::reject);


    connect(ui->pluginList, &QListWidget::currentItemChanged,
            [this](QListWidgetItem* current, QListWidgetItem*) {
        const PGE_EditorPluginItem* pluginDetails = m_pluginManager->getPluginInfo(current->data(UserRolePluginIndex).toInt());
        if(pluginDetails->isValid())
        {
            ui->textInfo->setText(tr("Name: ") + pluginDetails->pluginName() + "\n"
                                 +tr("Author: ") + pluginDetails->authorName() + "\n"
                                 +tr("Version: ") + QString::number(pluginDetails->pluginVersionNum()) + "\n"
                                 +tr("Description: ") + pluginDetails->description());
        }
        else
        {
            ui->textInfo->setText(tr("Package ") + pluginDetails->packageName() + tr(" failed to load!\n")
                                 +tr("Error description: \n") + pluginDetails->errorMessage());
        }
    });
}

PGE_EditorPluginInfo::~PGE_EditorPluginInfo()
{
    delete ui;
}
