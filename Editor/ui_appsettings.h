/********************************************************************************
** Form generated from reading UI file 'appsettings.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_APPSETTINGS_H
#define UI_APPSETTINGS_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_AppSettings
{
public:
    QGridLayout *gridLayout;
    QTabWidget *mainSettings;
    QWidget *tabSet;
    QGridLayout *gridLayout_3;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout;
    QCheckBox *checkBox;
    QSpacerItem *verticalSpacer_2;
    QWidget *tabLogging;
    QGridLayout *gridLayout_2;
    QLabel *label;
    QLineEdit *logFileName;
    QPushButton *setLogFile;
    QLabel *label_2;
    QComboBox *logLevel;
    QSpacerItem *verticalSpacer;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *AppSettings)
    {
        if (AppSettings->objectName().isEmpty())
            AppSettings->setObjectName(QStringLiteral("AppSettings"));
        AppSettings->resize(350, 400);
        AppSettings->setMinimumSize(QSize(350, 400));
        QIcon icon;
        icon.addFile(QStringLiteral(":/mushroom.ico"), QSize(), QIcon::Normal, QIcon::Off);
        AppSettings->setWindowIcon(icon);
        gridLayout = new QGridLayout(AppSettings);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        mainSettings = new QTabWidget(AppSettings);
        mainSettings->setObjectName(QStringLiteral("mainSettings"));
        tabSet = new QWidget();
        tabSet->setObjectName(QStringLiteral("tabSet"));
        gridLayout_3 = new QGridLayout(tabSet);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        groupBox = new QGroupBox(tabSet);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        verticalLayout = new QVBoxLayout(groupBox);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        checkBox = new QCheckBox(groupBox);
        checkBox->setObjectName(QStringLiteral("checkBox"));

        verticalLayout->addWidget(checkBox);


        gridLayout_3->addWidget(groupBox, 0, 0, 1, 1);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_3->addItem(verticalSpacer_2, 1, 0, 1, 1);

        mainSettings->addTab(tabSet, QString());
        tabLogging = new QWidget();
        tabLogging->setObjectName(QStringLiteral("tabLogging"));
        gridLayout_2 = new QGridLayout(tabLogging);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        label = new QLabel(tabLogging);
        label->setObjectName(QStringLiteral("label"));

        gridLayout_2->addWidget(label, 0, 0, 1, 1);

        logFileName = new QLineEdit(tabLogging);
        logFileName->setObjectName(QStringLiteral("logFileName"));

        gridLayout_2->addWidget(logFileName, 1, 1, 1, 1);

        setLogFile = new QPushButton(tabLogging);
        setLogFile->setObjectName(QStringLiteral("setLogFile"));

        gridLayout_2->addWidget(setLogFile, 1, 2, 1, 1);

        label_2 = new QLabel(tabLogging);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout_2->addWidget(label_2, 1, 0, 1, 1);

        logLevel = new QComboBox(tabLogging);
        logLevel->setObjectName(QStringLiteral("logLevel"));

        gridLayout_2->addWidget(logLevel, 0, 1, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_2->addItem(verticalSpacer, 2, 0, 1, 3);

        mainSettings->addTab(tabLogging, QString());

        gridLayout->addWidget(mainSettings, 0, 0, 1, 1);

        buttonBox = new QDialogButtonBox(AppSettings);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        gridLayout->addWidget(buttonBox, 1, 0, 1, 1);


        retranslateUi(AppSettings);
        QObject::connect(buttonBox, SIGNAL(accepted()), AppSettings, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), AppSettings, SLOT(reject()));

        mainSettings->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(AppSettings);
    } // setupUi

    void retranslateUi(QDialog *AppSettings)
    {
        AppSettings->setWindowTitle(QApplication::translate("AppSettings", "Settings", 0));
        groupBox->setTitle(QApplication::translate("AppSettings", "Music player", 0));
        checkBox->setText(QApplication::translate("AppSettings", "Autoplay music on file open", 0));
        mainSettings->setTabText(mainSettings->indexOf(tabSet), QApplication::translate("AppSettings", "Main", 0));
        label->setText(QApplication::translate("AppSettings", "Log level", 0));
        setLogFile->setText(QApplication::translate("AppSettings", "Browse", 0));
        label_2->setText(QApplication::translate("AppSettings", "Log file", 0));
        logLevel->clear();
        logLevel->insertItems(0, QStringList()
         << QApplication::translate("AppSettings", "Disable logging", 0)
         << QApplication::translate("AppSettings", "Fatal", 0)
         << QApplication::translate("AppSettings", "Crytical", 0)
         << QApplication::translate("AppSettings", "Warning", 0)
         << QApplication::translate("AppSettings", "Debug", 0)
        );
        mainSettings->setTabText(mainSettings->indexOf(tabLogging), QApplication::translate("AppSettings", "Loging", 0));
    } // retranslateUi

};

namespace Ui {
    class AppSettings: public Ui_AppSettings {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_APPSETTINGS_H
