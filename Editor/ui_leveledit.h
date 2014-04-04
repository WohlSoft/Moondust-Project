/********************************************************************************
** Form generated from reading UI file 'leveledit.ui'
**
** Created: Fri 4. Apr 22:13:48 2014
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LEVELEDIT_H
#define UI_LEVELEDIT_H

#include <QtCore/QLocale>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGraphicsView>
#include <QtGui/QHeaderView>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_leveledit
{
public:
    QVBoxLayout *verticalLayout;
    QGraphicsView *graphicsView;

    void setupUi(QWidget *leveledit)
    {
        if (leveledit->objectName().isEmpty())
            leveledit->setObjectName(QString::fromUtf8("leveledit"));
        leveledit->resize(800, 602);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(leveledit->sizePolicy().hasHeightForWidth());
        leveledit->setSizePolicy(sizePolicy);
        leveledit->setBaseSize(QSize(800, 602));
        leveledit->setContextMenuPolicy(Qt::NoContextMenu);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/level16.png"), QSize(), QIcon::Normal, QIcon::Off);
        leveledit->setWindowIcon(icon);
        leveledit->setStyleSheet(QString::fromUtf8(""));
        leveledit->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        verticalLayout = new QVBoxLayout(leveledit);
        verticalLayout->setSpacing(0);
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        graphicsView = new QGraphicsView(leveledit);
        graphicsView->setObjectName(QString::fromUtf8("graphicsView"));
        graphicsView->setAutoFillBackground(false);
        graphicsView->setStyleSheet(QString::fromUtf8("background-color: qlineargradient(spread:pad, x1:0.369, y1:1, x2:0, y2:0, stop:0 rgba(0, 0, 0, 255), stop:1 rgba(132, 132, 132, 255));"));
        graphicsView->setInteractive(true);

        verticalLayout->addWidget(graphicsView);


        retranslateUi(leveledit);

        QMetaObject::connectSlotsByName(leveledit);
    } // setupUi

    void retranslateUi(QWidget *leveledit)
    {
        leveledit->setWindowTitle(QApplication::translate("leveledit", "Level edit", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class leveledit: public Ui_leveledit {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LEVELEDIT_H
