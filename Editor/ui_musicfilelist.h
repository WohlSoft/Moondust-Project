/********************************************************************************
** Form generated from reading UI file 'musicfilelist.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MUSICFILELIST_H
#define UI_MUSICFILELIST_H

#include <QtCore/QLocale>
#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_MusicFileList
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *SelectedFolder;
    QListWidget *FileList;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *MusicFileList)
    {
        if (MusicFileList->objectName().isEmpty())
            MusicFileList->setObjectName(QStringLiteral("MusicFileList"));
        MusicFileList->resize(278, 366);
        QIcon icon;
        icon.addFile(QStringLiteral(":/lvl16.png"), QSize(), QIcon::Normal, QIcon::Off);
        MusicFileList->setWindowIcon(icon);
        MusicFileList->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        verticalLayout = new QVBoxLayout(MusicFileList);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setSizeConstraint(QLayout::SetMinimumSize);
        SelectedFolder = new QLabel(MusicFileList);
        SelectedFolder->setObjectName(QStringLiteral("SelectedFolder"));
        SelectedFolder->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));

        verticalLayout->addWidget(SelectedFolder);

        FileList = new QListWidget(MusicFileList);
        FileList->setObjectName(QStringLiteral("FileList"));
        FileList->setMinimumSize(QSize(260, 300));
        FileList->setEditTriggers(QAbstractItemView::DoubleClicked|QAbstractItemView::EditKeyPressed);

        verticalLayout->addWidget(FileList);

        buttonBox = new QDialogButtonBox(MusicFileList);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(MusicFileList);

        QMetaObject::connectSlotsByName(MusicFileList);
    } // setupUi

    void retranslateUi(QDialog *MusicFileList)
    {
        MusicFileList->setWindowTitle(QApplication::translate("MusicFileList", "Select Custom music", 0));
        SelectedFolder->setText(QApplication::translate("MusicFileList", "Please, select music file for use as custom", 0));
    } // retranslateUi

};

namespace Ui {
    class MusicFileList: public Ui_MusicFileList {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MUSICFILELIST_H
