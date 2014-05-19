/********************************************************************************
** Form generated from reading UI file 'tonewlayerbox.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TONEWLAYERBOX_H
#define UI_TONEWLAYERBOX_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>

QT_BEGIN_NAMESPACE

class Ui_ToNewLayerBox
{
public:
    QFormLayout *formLayout;
    QLabel *label;
    QLineEdit *layerName;
    QCheckBox *hide;
    QCheckBox *Lock;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *ToNewLayerBox)
    {
        if (ToNewLayerBox->objectName().isEmpty())
            ToNewLayerBox->setObjectName(QStringLiteral("ToNewLayerBox"));
        ToNewLayerBox->resize(174, 109);
        QIcon icon;
        icon.addFile(QStringLiteral(":/layers.png"), QSize(), QIcon::Normal, QIcon::Off);
        ToNewLayerBox->setWindowIcon(icon);
        formLayout = new QFormLayout(ToNewLayerBox);
        formLayout->setObjectName(QStringLiteral("formLayout"));
        formLayout->setSizeConstraint(QLayout::SetMinimumSize);
        label = new QLabel(ToNewLayerBox);
        label->setObjectName(QStringLiteral("label"));

        formLayout->setWidget(0, QFormLayout::SpanningRole, label);

        layerName = new QLineEdit(ToNewLayerBox);
        layerName->setObjectName(QStringLiteral("layerName"));

        formLayout->setWidget(1, QFormLayout::SpanningRole, layerName);

        hide = new QCheckBox(ToNewLayerBox);
        hide->setObjectName(QStringLiteral("hide"));

        formLayout->setWidget(2, QFormLayout::LabelRole, hide);

        Lock = new QCheckBox(ToNewLayerBox);
        Lock->setObjectName(QStringLiteral("Lock"));

        formLayout->setWidget(2, QFormLayout::FieldRole, Lock);

        buttonBox = new QDialogButtonBox(ToNewLayerBox);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        formLayout->setWidget(3, QFormLayout::SpanningRole, buttonBox);


        retranslateUi(ToNewLayerBox);
        QObject::connect(buttonBox, SIGNAL(accepted()), ToNewLayerBox, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), ToNewLayerBox, SLOT(reject()));

        QMetaObject::connectSlotsByName(ToNewLayerBox);
    } // setupUi

    void retranslateUi(QDialog *ToNewLayerBox)
    {
        ToNewLayerBox->setWindowTitle(QApplication::translate("ToNewLayerBox", "Add to new layer", 0));
        label->setText(QApplication::translate("ToNewLayerBox", "New layer name", 0));
        hide->setText(QApplication::translate("ToNewLayerBox", "Hidden", 0));
        Lock->setText(QApplication::translate("ToNewLayerBox", "Locked", 0));
    } // retranslateUi

};

namespace Ui {
    class ToNewLayerBox: public Ui_ToNewLayerBox {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TONEWLAYERBOX_H
