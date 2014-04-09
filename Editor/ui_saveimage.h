/********************************************************************************
** Form generated from reading UI file 'saveimage.ui'
**
** Created: Wed 9. Apr 14:39:50 2014
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
    QLabel *label_4;

    void setupUi(QDialog *ExportToImage)
    {
        if (ExportToImage->objectName().isEmpty())
            ExportToImage->setObjectName(QString::fromUtf8("ExportToImage"));
        ExportToImage->resize(300, 156);
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
        QFont font1;
        font1.setPointSize(11);
        font1.setBold(true);
        font1.setWeight(75);
        label->setFont(font1);
        label->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label, 0, 0, 1, 2);

        label_2 = new QLabel(ExportToImage);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 2, 0, 1, 1);

        imgHeight = new QSpinBox(ExportToImage);
        imgHeight->setObjectName(QString::fromUtf8("imgHeight"));
        imgHeight->setMaximum(999999999);

        gridLayout->addWidget(imgHeight, 2, 1, 1, 1);

        label_3 = new QLabel(ExportToImage);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout->addWidget(label_3, 3, 0, 1, 1);

        imgWidth = new QSpinBox(ExportToImage);
        imgWidth->setObjectName(QString::fromUtf8("imgWidth"));
        imgWidth->setMaximum(999999999);

        gridLayout->addWidget(imgWidth, 3, 1, 1, 1);

        SaveProportion = new QCheckBox(ExportToImage);
        SaveProportion->setObjectName(QString::fromUtf8("SaveProportion"));

        gridLayout->addWidget(SaveProportion, 4, 0, 1, 2);

        buttonBox = new QDialogButtonBox(ExportToImage);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        buttonBox->setCenterButtons(true);

        gridLayout->addWidget(buttonBox, 5, 0, 1, 2);

        label_4 = new QLabel(ExportToImage);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        QFont font2;
        font2.setItalic(true);
        label_4->setFont(font2);
        label_4->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_4, 1, 0, 1, 2);


        retranslateUi(ExportToImage);

        QMetaObject::connectSlotsByName(ExportToImage);
    } // setupUi

    void retranslateUi(QDialog *ExportToImage)
    {
        ExportToImage->setWindowTitle(QApplication::translate("ExportToImage", "Export to image", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("ExportToImage", "Export current section to image", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("ExportToImage", "Height", 0, QApplication::UnicodeUTF8));
        imgHeight->setSuffix(QApplication::translate("ExportToImage", " px", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("ExportToImage", "Width", 0, QApplication::UnicodeUTF8));
        imgWidth->setSuffix(QApplication::translate("ExportToImage", " px", 0, QApplication::UnicodeUTF8));
        SaveProportion->setText(QApplication::translate("ExportToImage", "Save proportion", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("ExportToImage", "Please, select target image size:", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ExportToImage: public Ui_ExportToImage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SAVEIMAGE_H
