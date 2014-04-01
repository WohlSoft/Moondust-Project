/********************************************************************************
** Form generated from reading UI file 'saveimage.ui'
**
** Created: Tue 1. Apr 22:51:58 2014
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SAVEIMAGE_H
#define UI_SAVEIMAGE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>

QT_BEGIN_NAMESPACE

class Ui_ExportToImage
{
public:

    void setupUi(QDialog *ExportToImage)
    {
        if (ExportToImage->objectName().isEmpty())
            ExportToImage->setObjectName(QString::fromUtf8("ExportToImage"));
        ExportToImage->resize(390, 319);

        retranslateUi(ExportToImage);

        QMetaObject::connectSlotsByName(ExportToImage);
    } // setupUi

    void retranslateUi(QDialog *ExportToImage)
    {
        ExportToImage->setWindowTitle(QApplication::translate("ExportToImage", "Export to image...", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ExportToImage: public Ui_ExportToImage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SAVEIMAGE_H
