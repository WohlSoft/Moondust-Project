/********************************************************************************
** Form generated from reading UI file 'aboutdialog.ui'
**
** Created: Sat 29. Mar 23:25:22 2014
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ABOUTDIALOG_H
#define UI_ABOUTDIALOG_H

#include <QtCore/QLocale>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QTextEdit>

QT_BEGIN_NAMESPACE

class Ui_aboutDialog
{
public:
    QAction *actionCloseDialog;
    QGridLayout *gridLayout;
    QPushButton *pushButton;
    QLabel *label;
    QTextEdit *textEdit;

    void setupUi(QDialog *aboutDialog)
    {
        if (aboutDialog->objectName().isEmpty())
            aboutDialog->setObjectName(QString::fromUtf8("aboutDialog"));
        aboutDialog->setWindowModality(Qt::ApplicationModal);
        aboutDialog->resize(500, 443);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(aboutDialog->sizePolicy().hasHeightForWidth());
        aboutDialog->setSizePolicy(sizePolicy);
        aboutDialog->setMinimumSize(QSize(500, 0));
        aboutDialog->setBaseSize(QSize(500, 300));
        QFont font;
        font.setFamily(QString::fromUtf8("Liberation Sans"));
        aboutDialog->setFont(font);
        aboutDialog->setContextMenuPolicy(Qt::NoContextMenu);
        aboutDialog->setAutoFillBackground(true);
        aboutDialog->setStyleSheet(QString::fromUtf8(""));
        aboutDialog->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        aboutDialog->setModal(true);
        actionCloseDialog = new QAction(aboutDialog);
        actionCloseDialog->setObjectName(QString::fromUtf8("actionCloseDialog"));
        gridLayout = new QGridLayout(aboutDialog);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setSizeConstraint(QLayout::SetFixedSize);
        gridLayout->setContentsMargins(-1, -1, 9, -1);
        pushButton = new QPushButton(aboutDialog);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setAutoRepeat(true);

        gridLayout->addWidget(pushButton, 2, 0, 1, 1);

        label = new QLabel(aboutDialog);
        label->setObjectName(QString::fromUtf8("label"));
        label->setMinimumSize(QSize(0, 0));
        label->setStyleSheet(QString::fromUtf8("background-color: rgba(255, 255, 255, 0);"));
        label->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        label->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        label->setOpenExternalLinks(true);
        label->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);

        gridLayout->addWidget(label, 0, 0, 1, 1);

        textEdit = new QTextEdit(aboutDialog);
        textEdit->setObjectName(QString::fromUtf8("textEdit"));
        textEdit->setMinimumSize(QSize(480, 0));
        textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        textEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        textEdit->setTextInteractionFlags(Qt::LinksAccessibleByKeyboard|Qt::LinksAccessibleByMouse|Qt::TextBrowserInteraction|Qt::TextSelectableByKeyboard|Qt::TextSelectableByMouse);

        gridLayout->addWidget(textEdit, 1, 0, 1, 1);

        gridLayout->setRowMinimumHeight(1, 150);

        retranslateUi(aboutDialog);

        QMetaObject::connectSlotsByName(aboutDialog);
    } // setupUi

    void retranslateUi(QDialog *aboutDialog)
    {
        aboutDialog->setWindowTitle(QApplication::translate("aboutDialog", "About", 0, QApplication::UnicodeUTF8));
        actionCloseDialog->setText(QApplication::translate("aboutDialog", "CloseDialog", 0, QApplication::UnicodeUTF8));
        actionCloseDialog->setShortcut(QApplication::translate("aboutDialog", "Esc", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("aboutDialog", "Close", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("aboutDialog", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Liberation Sans'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><img src=\":/images/mushroom.png\" /></p>\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'MS Shell Dlg 2'; font-size:8pt; font-weight:600;\">Platformer Game Engine</span></p>\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'MS Shell Dlg 2'; font-size:8pt; text-decoration: underline;\">By Wohlstand<"
                        "/span><span style=\" font-family:'MS Shell Dlg 2'; font-size:8pt; font-weight:600;\"><br /></span><span style=\" font-family:'MS Shell Dlg 2'; font-size:8pt; font-style:italic;\">Pre-Alpha Version 0.0.3</span></p>\n"
"<p align=\"center\" style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-family:'MS Shell Dlg 2'; font-size:8pt; font-style:italic;\"></p>\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'MS Shell Dlg 2'; font-size:8pt; font-style:italic; color:#aa0000;\">Project is Under Construction</span><span style=\" font-family:'MS Shell Dlg 2'; font-size:8pt;\"><br /></span></p>\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'MS Shell Dlg 2'; font-size:8pt;\">Our project site:"
                        "</span></p>\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><a href=\"http://engine.wohlnet.ru\"><span style=\" font-size:8pt; text-decoration: underline; color:#0000ff;\">http://engine.wohlnet.ru</span></a></p>\n"
"<p align=\"center\" style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:8pt; text-decoration: underline; color:#0000ff;\"></p>\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt;\">This programm is distributed under </span><a href=\"http://www.gnu.org/licenses/gpl-2.0.html\"><span style=\" font-size:8pt; text-decoration: underline; color:#0000ff;\">GNU GNLv2</span></a><span style=\" font-size:8pt; text-decoration: underline;\">.</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        textEdit->setHtml(QApplication::translate("aboutDialog", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Liberation Sans'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'MS Shell Dlg 2'; font-size:8pt; font-weight:600; text-decoration: underline;\">Main developers:<br /></span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'MS Shell Dlg 2'; font-size:8pt; text-decoration: underline;\">Engine, Editor, Graphics, Documents, Logo:</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-fami"
                        "ly:'MS Shell Dlg 2'; font-size:8pt; font-style:italic;\">Wohlstand</span><span style=\" font-family:'MS Shell Dlg 2'; font-size:8pt;\"> &lt;</span><a href=\"mailto:admin@wohlnet.ru\"><span style=\" font-family:'MS Shell Dlg 2'; font-size:8pt; text-decoration: underline; color:#0000ff;\">admin@wohlnet.ru</span></a><span style=\" font-family:'MS Shell Dlg 2'; font-size:8pt;\">&gt;<br /></span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'MS Shell Dlg 2'; font-size:8pt; text-decoration: underline;\">Logo:</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'MS Shell Dlg 2'; font-size:8pt; font-style:italic;\">Natsu<br /></span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'MS Shell Dlg"
                        " 2'; font-size:8pt; text-decoration: underline;\">Graphics, Documents:</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'MS Shell Dlg 2'; font-size:8pt; font-style:italic;\">Veudekato<br /></span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'MS Shell Dlg 2'; font-size:8pt; font-weight:600; text-decoration: underline;\">Big thanks to:<br /></span><span style=\" font-family:'MS Shell Dlg 2'; font-size:8pt; font-style:italic;\">h2643</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'MS Shell Dlg 2'; font-size:8pt; font-style:italic;\">FanofSMBX</span></p></body></html>", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class aboutDialog: public Ui_aboutDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ABOUTDIALOG_H
