/********************************************************************************
** Form generated from reading UI file 'levelprops.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LEVELPROPS_H
#define UI_LEVELPROPS_H

#include <QtCore/QLocale>
#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>

QT_BEGIN_NAMESPACE

class Ui_LevelProps
{
public:
    QGridLayout *gridLayout;
    QFrame *frame;
    QGridLayout *gridLayout_2;
    QLabel *label;
    QLineEdit *LVLPropLevelTitle;
    QGroupBox *EditingProps;
    QCheckBox *setAutoplayMusic;
    QDialogButtonBox *LVLPropButtonBox;

    void setupUi(QDialog *LevelProps)
    {
        if (LevelProps->objectName().isEmpty())
            LevelProps->setObjectName(QStringLiteral("LevelProps"));
        LevelProps->setWindowModality(Qt::WindowModal);
        LevelProps->resize(318, 197);
        LevelProps->setContextMenuPolicy(Qt::NoContextMenu);
        QIcon icon;
        icon.addFile(QStringLiteral(":/lvl16.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon.addFile(QStringLiteral(":/lvl16.png"), QSize(), QIcon::Normal, QIcon::On);
        icon.addFile(QStringLiteral(":/lvl16.png"), QSize(), QIcon::Active, QIcon::Off);
        icon.addFile(QStringLiteral(":/lvl16.png"), QSize(), QIcon::Active, QIcon::On);
        icon.addFile(QStringLiteral(":/lvl16.png"), QSize(), QIcon::Selected, QIcon::On);
        LevelProps->setWindowIcon(icon);
        LevelProps->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        LevelProps->setModal(false);
        gridLayout = new QGridLayout(LevelProps);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setSizeConstraint(QLayout::SetFixedSize);
        gridLayout->setContentsMargins(-1, 9, -1, -1);
        frame = new QFrame(LevelProps);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        gridLayout_2 = new QGridLayout(frame);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        label = new QLabel(frame);
        label->setObjectName(QStringLiteral("label"));

        gridLayout_2->addWidget(label, 0, 0, 1, 1);

        LVLPropLevelTitle = new QLineEdit(frame);
        LVLPropLevelTitle->setObjectName(QStringLiteral("LVLPropLevelTitle"));

        gridLayout_2->addWidget(LVLPropLevelTitle, 0, 1, 1, 2);

        EditingProps = new QGroupBox(frame);
        EditingProps->setObjectName(QStringLiteral("EditingProps"));
        setAutoplayMusic = new QCheckBox(EditingProps);
        setAutoplayMusic->setObjectName(QStringLiteral("setAutoplayMusic"));
        setAutoplayMusic->setGeometry(QRect(10, 20, 147, 17));

        gridLayout_2->addWidget(EditingProps, 1, 1, 1, 2);


        gridLayout->addWidget(frame, 0, 0, 1, 1);

        LVLPropButtonBox = new QDialogButtonBox(LevelProps);
        LVLPropButtonBox->setObjectName(QStringLiteral("LVLPropButtonBox"));
        LVLPropButtonBox->setOrientation(Qt::Horizontal);
        LVLPropButtonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        gridLayout->addWidget(LVLPropButtonBox, 1, 0, 1, 1);

        gridLayout->setColumnMinimumWidth(0, 300);
        gridLayout->setRowMinimumHeight(0, 150);

        retranslateUi(LevelProps);
        QObject::connect(LVLPropButtonBox, SIGNAL(accepted()), LevelProps, SLOT(accept()));
        QObject::connect(LVLPropButtonBox, SIGNAL(rejected()), LevelProps, SLOT(reject()));

        QMetaObject::connectSlotsByName(LevelProps);
    } // setupUi

    void retranslateUi(QDialog *LevelProps)
    {
        LevelProps->setWindowTitle(QApplication::translate("LevelProps", "Level Properties", 0));
        label->setText(QApplication::translate("LevelProps", "Level name", 0));
        EditingProps->setTitle(QApplication::translate("LevelProps", "Editing", 0));
        setAutoplayMusic->setText(QApplication::translate("LevelProps", "Autoplay music", 0));
    } // retranslateUi

};

namespace Ui {
    class LevelProps: public Ui_LevelProps {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LEVELPROPS_H
