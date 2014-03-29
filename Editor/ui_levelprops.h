/********************************************************************************
** Form generated from reading UI file 'levelprops.ui'
**
** Created: Fri 28. Mar 13:52:06 2014
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LEVELPROPS_H
#define UI_LEVELPROPS_H

#include <QtCore/QLocale>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>

QT_BEGIN_NAMESPACE

class Ui_LevelProps
{
public:
    QGridLayout *gridLayout;
    QFrame *frame;
    QGridLayout *gridLayout_2;
    QLabel *label;
    QLineEdit *LVLPropLevelTitle;
    QDialogButtonBox *LVLPropButtonBox;
    QGroupBox *groupBox;
    QCheckBox *checkBox;

    void setupUi(QDialog *LevelProps)
    {
        if (LevelProps->objectName().isEmpty())
            LevelProps->setObjectName(QString::fromUtf8("LevelProps"));
        LevelProps->setWindowModality(Qt::WindowModal);
        LevelProps->resize(318, 168);
        LevelProps->setContextMenuPolicy(Qt::NoContextMenu);
        LevelProps->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        LevelProps->setModal(false);
        gridLayout = new QGridLayout(LevelProps);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setSizeConstraint(QLayout::SetFixedSize);
        gridLayout->setContentsMargins(-1, 9, -1, -1);
        frame = new QFrame(LevelProps);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        gridLayout_2 = new QGridLayout(frame);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        label = new QLabel(frame);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout_2->addWidget(label, 0, 0, 1, 1);

        LVLPropLevelTitle = new QLineEdit(frame);
        LVLPropLevelTitle->setObjectName(QString::fromUtf8("LVLPropLevelTitle"));

        gridLayout_2->addWidget(LVLPropLevelTitle, 0, 1, 1, 2);

        LVLPropButtonBox = new QDialogButtonBox(frame);
        LVLPropButtonBox->setObjectName(QString::fromUtf8("LVLPropButtonBox"));
        LVLPropButtonBox->setOrientation(Qt::Horizontal);
        LVLPropButtonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        gridLayout_2->addWidget(LVLPropButtonBox, 2, 2, 1, 1);

        groupBox = new QGroupBox(frame);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        checkBox = new QCheckBox(groupBox);
        checkBox->setObjectName(QString::fromUtf8("checkBox"));
        checkBox->setGeometry(QRect(10, 20, 147, 17));

        gridLayout_2->addWidget(groupBox, 1, 1, 1, 2);


        gridLayout->addWidget(frame, 0, 0, 1, 1);

        gridLayout->setColumnMinimumWidth(0, 300);
        gridLayout->setRowMinimumHeight(0, 150);

        retranslateUi(LevelProps);
        QObject::connect(LVLPropButtonBox, SIGNAL(accepted()), LevelProps, SLOT(accept()));
        QObject::connect(LVLPropButtonBox, SIGNAL(rejected()), LevelProps, SLOT(reject()));

        QMetaObject::connectSlotsByName(LevelProps);
    } // setupUi

    void retranslateUi(QDialog *LevelProps)
    {
        LevelProps->setWindowTitle(QApplication::translate("LevelProps", "Level Properties", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("LevelProps", "Level name", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("LevelProps", "Editing", 0, QApplication::UnicodeUTF8));
        checkBox->setText(QApplication::translate("LevelProps", "Autoplay music", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class LevelProps: public Ui_LevelProps {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LEVELPROPS_H
