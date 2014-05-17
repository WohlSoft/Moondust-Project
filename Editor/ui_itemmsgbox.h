/********************************************************************************
** Form generated from reading UI file 'itemmsgbox.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ITEMMSGBOX_H
#define UI_ITEMMSGBOX_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPlainTextEdit>

QT_BEGIN_NAMESPACE

class Ui_ItemMsgBox
{
public:
    QGridLayout *gridLayout;
    QLabel *NotesLabel;
    QPlainTextEdit *msgTextBox;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *ItemMsgBox)
    {
        if (ItemMsgBox->objectName().isEmpty())
            ItemMsgBox->setObjectName(QStringLiteral("ItemMsgBox"));
        ItemMsgBox->resize(268, 177);
        QFont font;
        font.setFamily(QStringLiteral("Liberation Sans"));
        font.setPointSize(8);
        font.setBold(false);
        font.setWeight(50);
        ItemMsgBox->setFont(font);
        QIcon icon;
        icon.addFile(QStringLiteral(":/images/coin.png"), QSize(), QIcon::Normal, QIcon::Off);
        ItemMsgBox->setWindowIcon(icon);
        gridLayout = new QGridLayout(ItemMsgBox);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setSizeConstraint(QLayout::SetFixedSize);
        NotesLabel = new QLabel(ItemMsgBox);
        NotesLabel->setObjectName(QStringLiteral("NotesLabel"));

        gridLayout->addWidget(NotesLabel, 0, 0, 1, 1);

        msgTextBox = new QPlainTextEdit(ItemMsgBox);
        msgTextBox->setObjectName(QStringLiteral("msgTextBox"));
        msgTextBox->setMinimumSize(QSize(250, 100));
        QFont font1;
        font1.setFamily(QStringLiteral("Liberation Mono"));
        font1.setPointSize(11);
        font1.setBold(true);
        font1.setWeight(75);
        msgTextBox->setFont(font1);
        msgTextBox->setUndoRedoEnabled(false);
        msgTextBox->setPlainText(QStringLiteral(""));
        msgTextBox->setTabStopWidth(28);
        msgTextBox->setMaximumBlockCount(28);

        gridLayout->addWidget(msgTextBox, 1, 0, 1, 1);

        buttonBox = new QDialogButtonBox(ItemMsgBox);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        gridLayout->addWidget(buttonBox, 2, 0, 1, 1);


        retranslateUi(ItemMsgBox);
        QObject::connect(buttonBox, SIGNAL(accepted()), ItemMsgBox, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), ItemMsgBox, SLOT(reject()));

        QMetaObject::connectSlotsByName(ItemMsgBox);
    } // setupUi

    void retranslateUi(QDialog *ItemMsgBox)
    {
        ItemMsgBox->setWindowTitle(QApplication::translate("ItemMsgBox", "Set message box", 0));
        NotesLabel->setText(QApplication::translate("ItemMsgBox", "Please, enter NPC's talkative message\n"
"Message limits: max line lenth is 28 characters", 0));
    } // retranslateUi

};

namespace Ui {
    class ItemMsgBox: public Ui_ItemMsgBox {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ITEMMSGBOX_H
