/********************************************************************************
** Form generated from reading UI file 'leveledit.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LEVELEDIT_H
#define UI_LEVELEDIT_H

#include <QtCore/QLocale>
#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_leveledit
{
public:
    QVBoxLayout *verticalLayout;
    QGraphicsView *graphicsView;

    void setupUi(QWidget *leveledit)
    {
        if (leveledit->objectName().isEmpty())
            leveledit->setObjectName(QStringLiteral("leveledit"));
        leveledit->resize(800, 602);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(leveledit->sizePolicy().hasHeightForWidth());
        leveledit->setSizePolicy(sizePolicy);
        leveledit->setBaseSize(QSize(800, 602));
        leveledit->setContextMenuPolicy(Qt::NoContextMenu);
        QIcon icon;
        icon.addFile(QStringLiteral(":/lvl16.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon.addFile(QStringLiteral(":/lvl16.png"), QSize(), QIcon::Normal, QIcon::On);
        icon.addFile(QStringLiteral(":/lvl16.png"), QSize(), QIcon::Active, QIcon::Off);
        icon.addFile(QStringLiteral(":/lvl16.png"), QSize(), QIcon::Active, QIcon::On);
        leveledit->setWindowIcon(icon);
        leveledit->setStyleSheet(QStringLiteral(""));
        leveledit->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        verticalLayout = new QVBoxLayout(leveledit);
        verticalLayout->setSpacing(0);
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        graphicsView = new QGraphicsView(leveledit);
        graphicsView->setObjectName(QStringLiteral("graphicsView"));
        graphicsView->setAutoFillBackground(false);
        graphicsView->setStyleSheet(QStringLiteral("background-color: qlineargradient(spread:pad, x1:0.369, y1:1, x2:0, y2:0, stop:0 rgba(0, 0, 0, 255), stop:1 rgba(132, 132, 132, 255));"));
        graphicsView->setInteractive(true);

        verticalLayout->addWidget(graphicsView);


        retranslateUi(leveledit);

        QMetaObject::connectSlotsByName(leveledit);
    } // setupUi

    void retranslateUi(QWidget *leveledit)
    {
        leveledit->setWindowTitle(QApplication::translate("leveledit", "Level edit", 0));
    } // retranslateUi

};

namespace Ui {
    class leveledit: public Ui_leveledit {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LEVELEDIT_H
