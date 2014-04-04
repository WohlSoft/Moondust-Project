/********************************************************************************
** Form generated from reading UI file 'saveimage.ui'
**
** Created: Fri 4. Apr 20:35:34 2014
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
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QSpinBox>

QT_BEGIN_NAMESPACE

class Ui_ExportToImage
{
public:
    QGridLayout *gridLayout;
    QLabel *label;
    QLabel *label_2;
    QSpinBox *imgHeight;
    QLabel *label_3;
    QSpinBox *imgWidth;
    QCheckBox *SaveProportion;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *ExportToImage)
    {
        if (ExportToImage->objectName().isEmpty())
            ExportToImage->setObjectName(QString::fromUtf8("ExportToImage"));
        ExportToImage->resize(279, 137);
        QFont font;
        font.setFamily(QString::fromUtf8("Liberation Sans"));
        ExportToImage->setFont(font);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/mushroom16.png"), QSize(), QIcon::Normal, QIcon::Off);
        ExportToImage->setWindowIcon(icon);
        gridLayout = new QGridLayout(ExportToImage);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setSizeConstraint(QLayout::SetFixedSize);
        label = new QLabel(ExportToImage);
        label->setObjectName(QString::fromUtf8("label"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy);

        gridLayout->addWidget(label, 0, 0, 1, 2);

        label_2 = new QLabel(ExportToImage);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        imgHeight = new QSpinBox(ExportToImage);
        imgHeight->setObjectName(QString::fromUtf8("imgHeight"));
        imgHeight->setMaximum(999999999);

        gridLayout->addWidget(imgHeight, 1, 1, 1, 1);

        label_3 = new QLabel(ExportToImage);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout->addWidget(label_3, 2, 0, 1, 1);

        imgWidth = new QSpinBox(ExportToImage);
        imgWidth->setObjectName(QString::fromUtf8("imgWidth"));
        imgWidth->setMaximum(999999999);

        gridLayout->addWidget(imgWidth, 2, 1, 1, 1);

        SaveProportion = new QCheckBox(ExportToImage);
        SaveProportion->setObjectName(QString::fromUtf8("SaveProportion"));

        gridLayout->addWidget(SaveProportion, 3, 0, 1, 2);

        buttonBox = new QDialogButtonBox(ExportToImage);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        buttonBox->setCenterButtons(true);

        gridLayout->addWidget(buttonBox, 4, 0, 1, 2);


        retranslateUi(ExportToImage);

        QMetaObject::connectSlotsByName(ExportToImage);
    } // setupUi

    void retranslateUi(QDialog *ExportToImage)
    {
        ExportToImage->setWindowTitle(QApplication::translate("ExportToImage", "Export to image", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("ExportToImage", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">Export current section to image</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("ExportToImage", "Height", 0, QApplication::UnicodeUTF8));
        imgHeight->setSuffix(QApplication::translate("ExportToImage", " px", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("ExportToImage", "Width", 0, QApplication::UnicodeUTF8));
        imgWidth->setSuffix(QApplication::translate("ExportToImage", " px", 0, QApplication::UnicodeUTF8));
        SaveProportion->setText(QApplication::translate("ExportToImage", "Save proportion", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ExportToImage: public Ui_ExportToImage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SAVEIMAGE_H
