/********************************************************************************
** Form generated from reading UI file 'saveimage.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SAVEIMAGE_H
#define UI_SAVEIMAGE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpinBox>

QT_BEGIN_NAMESPACE

class Ui_ExportToImage
{
public:
    QGridLayout *gridLayout;
    QSpinBox *imgHeight;
    QLabel *label_4;
    QCheckBox *SaveProportion;
    QSpinBox *imgWidth;
    QLabel *label_3;
    QLabel *label;
    QLabel *label_2;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *ExportToImage)
    {
        if (ExportToImage->objectName().isEmpty())
            ExportToImage->setObjectName(QStringLiteral("ExportToImage"));
        ExportToImage->resize(300, 156);
        QFont font;
        font.setFamily(QStringLiteral("Liberation Sans"));
        ExportToImage->setFont(font);
        QIcon icon;
        icon.addFile(QStringLiteral(":/lvl16.png"), QSize(), QIcon::Normal, QIcon::Off);
        ExportToImage->setWindowIcon(icon);
        gridLayout = new QGridLayout(ExportToImage);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setSizeConstraint(QLayout::SetFixedSize);
        imgHeight = new QSpinBox(ExportToImage);
        imgHeight->setObjectName(QStringLiteral("imgHeight"));
        imgHeight->setMaximum(999999999);

        gridLayout->addWidget(imgHeight, 2, 1, 1, 1);

        label_4 = new QLabel(ExportToImage);
        label_4->setObjectName(QStringLiteral("label_4"));
        QFont font1;
        font1.setItalic(true);
        label_4->setFont(font1);
        label_4->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_4, 1, 0, 1, 2);

        SaveProportion = new QCheckBox(ExportToImage);
        SaveProportion->setObjectName(QStringLiteral("SaveProportion"));

        gridLayout->addWidget(SaveProportion, 5, 0, 1, 2);

        imgWidth = new QSpinBox(ExportToImage);
        imgWidth->setObjectName(QStringLiteral("imgWidth"));
        imgWidth->setMaximum(999999999);

        gridLayout->addWidget(imgWidth, 3, 1, 1, 1);

        label_3 = new QLabel(ExportToImage);
        label_3->setObjectName(QStringLiteral("label_3"));

        gridLayout->addWidget(label_3, 3, 0, 1, 1);

        label = new QLabel(ExportToImage);
        label->setObjectName(QStringLiteral("label"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy);
        QFont font2;
        font2.setPointSize(11);
        font2.setBold(true);
        font2.setWeight(75);
        label->setFont(font2);
        label->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label, 0, 0, 1, 2);

        label_2 = new QLabel(ExportToImage);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout->addWidget(label_2, 2, 0, 1, 1);

        buttonBox = new QDialogButtonBox(ExportToImage);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        buttonBox->setCenterButtons(true);

        gridLayout->addWidget(buttonBox, 6, 0, 1, 2);


        retranslateUi(ExportToImage);

        QMetaObject::connectSlotsByName(ExportToImage);
    } // setupUi

    void retranslateUi(QDialog *ExportToImage)
    {
        ExportToImage->setWindowTitle(QApplication::translate("ExportToImage", "Export to image", 0));
        imgHeight->setSuffix(QApplication::translate("ExportToImage", " px", 0));
        label_4->setText(QApplication::translate("ExportToImage", "Please, select target image size:", 0));
        SaveProportion->setText(QApplication::translate("ExportToImage", "Save proportion", 0));
        imgWidth->setSuffix(QApplication::translate("ExportToImage", " px", 0));
        label_3->setText(QApplication::translate("ExportToImage", "Width", 0));
        label->setText(QApplication::translate("ExportToImage", "Export current section to image", 0));
        label_2->setText(QApplication::translate("ExportToImage", "Height", 0));
    } // retranslateUi

};

namespace Ui {
    class ExportToImage: public Ui_ExportToImage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SAVEIMAGE_H
