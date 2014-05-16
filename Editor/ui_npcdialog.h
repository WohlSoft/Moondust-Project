/********************************************************************************
** Form generated from reading UI file 'npcdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NPCDIALOG_H
#define UI_NPCDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>

QT_BEGIN_NAMESPACE

class Ui_NpcDialog
{
public:
    QGridLayout *gridLayout_2;
    QListWidget *npcList;
    QGroupBox *ContentType;
    QGridLayout *gridLayout;
    QRadioButton *coinsNPC;
    QSpinBox *coinsInBlock;
    QRadioButton *emptyNPC;
    QRadioButton *NPCfromList;
    QSpacerItem *verticalSpacer;
    QDialogButtonBox *buttonBox;
    QLabel *npcSelectLabel;

    void setupUi(QDialog *NpcDialog)
    {
        if (NpcDialog->objectName().isEmpty())
            NpcDialog->setObjectName(QStringLiteral("NpcDialog"));
        NpcDialog->resize(378, 327);
        QIcon icon;
        icon.addFile(QStringLiteral(":/images/coin.png"), QSize(), QIcon::Normal, QIcon::Off);
        NpcDialog->setWindowIcon(icon);
        gridLayout_2 = new QGridLayout(NpcDialog);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        gridLayout_2->setSizeConstraint(QLayout::SetDefaultConstraint);
        npcList = new QListWidget(NpcDialog);
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/images/mushroom.png"), QSize(), QIcon::Normal, QIcon::Off);
        QListWidgetItem *__qlistwidgetitem = new QListWidgetItem(npcList);
        __qlistwidgetitem->setIcon(icon1);
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/images/world.png"), QSize(), QIcon::Normal, QIcon::Off);
        QListWidgetItem *__qlistwidgetitem1 = new QListWidgetItem(npcList);
        __qlistwidgetitem1->setIcon(icon2);
        QIcon icon3;
        icon3.addFile(QStringLiteral(":/images/reset_pos.png"), QSize(), QIcon::Normal, QIcon::Off);
        QListWidgetItem *__qlistwidgetitem2 = new QListWidgetItem(npcList);
        __qlistwidgetitem2->setIcon(icon3);
        npcList->setObjectName(QStringLiteral("npcList"));
        npcList->setMinimumSize(QSize(250, 250));
        npcList->setIconSize(QSize(24, 24));
        npcList->setModelColumn(0);
        npcList->setUniformItemSizes(true);

        gridLayout_2->addWidget(npcList, 2, 0, 2, 1);

        ContentType = new QGroupBox(NpcDialog);
        ContentType->setObjectName(QStringLiteral("ContentType"));
        gridLayout = new QGridLayout(ContentType);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        coinsNPC = new QRadioButton(ContentType);
        coinsNPC->setObjectName(QStringLiteral("coinsNPC"));

        gridLayout->addWidget(coinsNPC, 2, 0, 1, 1);

        coinsInBlock = new QSpinBox(ContentType);
        coinsInBlock->setObjectName(QStringLiteral("coinsInBlock"));
        coinsInBlock->setEnabled(false);
        coinsInBlock->setMinimum(1);

        gridLayout->addWidget(coinsInBlock, 3, 0, 1, 1);

        emptyNPC = new QRadioButton(ContentType);
        emptyNPC->setObjectName(QStringLiteral("emptyNPC"));

        gridLayout->addWidget(emptyNPC, 1, 0, 1, 1);

        NPCfromList = new QRadioButton(ContentType);
        NPCfromList->setObjectName(QStringLiteral("NPCfromList"));
        NPCfromList->setChecked(true);

        gridLayout->addWidget(NPCfromList, 0, 0, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer, 4, 0, 1, 1);


        gridLayout_2->addWidget(ContentType, 3, 1, 1, 1);

        buttonBox = new QDialogButtonBox(NpcDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Vertical);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        gridLayout_2->addWidget(buttonBox, 1, 1, 2, 1);

        npcSelectLabel = new QLabel(NpcDialog);
        npcSelectLabel->setObjectName(QStringLiteral("npcSelectLabel"));
        npcSelectLabel->setMinimumSize(QSize(0, 25));
        npcSelectLabel->setMaximumSize(QSize(16777215, 25));

        gridLayout_2->addWidget(npcSelectLabel, 0, 0, 2, 1);

        gridLayout_2->setRowStretch(1, 10);
        gridLayout_2->setRowStretch(2, 40);
        gridLayout_2->setRowStretch(3, 1000);
        gridLayout_2->setColumnStretch(0, 100);

        retranslateUi(NpcDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), NpcDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), NpcDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(NpcDialog);
    } // setupUi

    void retranslateUi(QDialog *NpcDialog)
    {
        NpcDialog->setWindowTitle(QApplication::translate("NpcDialog", "NPC Select dialog", 0));

        const bool __sortingEnabled = npcList->isSortingEnabled();
        npcList->setSortingEnabled(false);
        QListWidgetItem *___qlistwidgetitem = npcList->item(0);
        ___qlistwidgetitem->setText(QApplication::translate("NpcDialog", "NPC 1", 0));
        QListWidgetItem *___qlistwidgetitem1 = npcList->item(1);
        ___qlistwidgetitem1->setText(QApplication::translate("NpcDialog", "NPC 2", 0));
        QListWidgetItem *___qlistwidgetitem2 = npcList->item(2);
        ___qlistwidgetitem2->setText(QApplication::translate("NpcDialog", "NPC 3", 0));
        npcList->setSortingEnabled(__sortingEnabled);

        ContentType->setTitle(QApplication::translate("NpcDialog", "Content type", 0));
        coinsNPC->setText(QApplication::translate("NpcDialog", "Coins", 0));
        emptyNPC->setText(QApplication::translate("NpcDialog", "Empty", 0));
        NPCfromList->setText(QApplication::translate("NpcDialog", "NPC from list", 0));
        npcSelectLabel->setText(QApplication::translate("NpcDialog", "Please, select NPC from list", 0));
    } // retranslateUi

};

namespace Ui {
    class NpcDialog: public Ui_NpcDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NPCDIALOG_H
