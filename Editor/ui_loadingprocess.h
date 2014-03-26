/********************************************************************************
** Form generated from reading UI file 'loadingprocess.ui'
**
** Created: Wed 26. Mar 09:00:03 2014
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOADINGPROCESS_H
#define UI_LOADINGPROCESS_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QProgressBar>

QT_BEGIN_NAMESPACE

class Ui_LoadingProcess
{
public:
    QHBoxLayout *horizontalLayout;
    QProgressBar *progressBar;

    void setupUi(QDialog *LoadingProcess)
    {
        if (LoadingProcess->objectName().isEmpty())
            LoadingProcess->setObjectName(QString::fromUtf8("LoadingProcess"));
        LoadingProcess->setWindowModality(Qt::WindowModal);
        LoadingProcess->resize(347, 39);
        horizontalLayout = new QHBoxLayout(LoadingProcess);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        progressBar = new QProgressBar(LoadingProcess);
        progressBar->setObjectName(QString::fromUtf8("progressBar"));
        progressBar->setValue(0);

        horizontalLayout->addWidget(progressBar);


        retranslateUi(LoadingProcess);

        QMetaObject::connectSlotsByName(LoadingProcess);
    } // setupUi

    void retranslateUi(QDialog *LoadingProcess)
    {
        LoadingProcess->setWindowTitle(QApplication::translate("LoadingProcess", "Loading...", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class LoadingProcess: public Ui_LoadingProcess {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOADINGPROCESS_H
