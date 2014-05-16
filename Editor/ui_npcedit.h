/********************************************************************************
** Form generated from reading UI file 'npcedit.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NPCEDIT_H
#define UI_NPCEDIT_H

#include <QtCore/QLocale>
#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_npcedit
{
public:
    QGroupBox *Graphics;
    QLabel *label_2;
    QCheckBox *en_GFXOffsetY;
    QLabel *label;
    QCheckBox *en_GFXOffsetX;
    QSpinBox *GFXOffSetX;
    QSpinBox *GFXOffSetY;
    QCheckBox *En_GFXw;
    QCheckBox *En_GFXh;
    QLabel *label_10;
    QLabel *label_11;
    QSpinBox *GFXw;
    QSpinBox *GFXh;
    QLabel *label_9;
    QLabel *label_7;
    QLabel *label_8;
    QCheckBox *En_Framespeed;
    QSpinBox *Framespeed;
    QComboBox *FrameStyle;
    QCheckBox *En_Framestyle;
    QSpinBox *Frames;
    QCheckBox *En_Frames;
    QCheckBox *En_IsForeground;
    QCheckBox *IsForeground;
    QGroupBox *Physics;
    QCheckBox *En_Width;
    QLabel *label_3;
    QSpinBox *Width;
    QLabel *label_4;
    QSpinBox *Height;
    QCheckBox *En_Height;
    QCheckBox *En_Speed;
    QLabel *label_6;
    QDoubleSpinBox *Speed;
    QCheckBox *PlayerBlockTop;
    QCheckBox *PlayerBlock;
    QCheckBox *NPCBlockTop;
    QCheckBox *En_PlayerBlock;
    QCheckBox *NPCBlock;
    QCheckBox *En_PlayerBlockTop;
    QCheckBox *En_NPCBlockTop;
    QCheckBox *En_NPCBlock;
    QCheckBox *En_NoGravity;
    QCheckBox *NoGravity;
    QCheckBox *En_TurnCliff;
    QCheckBox *TurnCliff;
    QCheckBox *En_NoBlockCollision;
    QCheckBox *NoBlockCollision;
    QGroupBox *InGame;
    QCheckBox *GrabSide;
    QCheckBox *NoFireball;
    QCheckBox *En_JumpHurt;
    QLabel *label_5;
    QCheckBox *En_NoFireball;
    QCheckBox *En_Score;
    QCheckBox *NoEat;
    QCheckBox *En_NoEat;
    QCheckBox *JumpHurt;
    QCheckBox *En_GrabTop;
    QCheckBox *DontHurt;
    QCheckBox *En_DontHurt;
    QComboBox *Score;
    QCheckBox *En_GrabSide;
    QCheckBox *NoIceball;
    QCheckBox *En_NoIceball;
    QCheckBox *GrabTop;
    QCheckBox *En_NoHammer;
    QCheckBox *NoHammer;
    QGroupBox *groupBox_4;
    QGraphicsView *PreviewBox;
    QPushButton *ResetNPCData;
    QGroupBox *NPC_ID;
    QLabel *CurrentNPCID;
    QGroupBox *isModBox;
    QLabel *isModyfiedL;

    void setupUi(QWidget *npcedit)
    {
        if (npcedit->objectName().isEmpty())
            npcedit->setObjectName(QStringLiteral("npcedit"));
        npcedit->setWindowModality(Qt::NonModal);
        npcedit->resize(500, 600);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(npcedit->sizePolicy().hasHeightForWidth());
        npcedit->setSizePolicy(sizePolicy);
        npcedit->setMinimumSize(QSize(500, 600));
        npcedit->setBaseSize(QSize(500, 600));
        QFont font;
        font.setFamily(QStringLiteral("Liberation Sans"));
        npcedit->setFont(font);
        npcedit->setContextMenuPolicy(Qt::NoContextMenu);
        QIcon icon;
        icon.addFile(QStringLiteral(":/images/coin.png"), QSize(), QIcon::Normal, QIcon::Off);
        npcedit->setWindowIcon(icon);
        npcedit->setWindowOpacity(0);
        npcedit->setLayoutDirection(Qt::LeftToRight);
        npcedit->setAutoFillBackground(false);
        npcedit->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        Graphics = new QGroupBox(npcedit);
        Graphics->setObjectName(QStringLiteral("Graphics"));
        Graphics->setGeometry(QRect(10, 40, 241, 261));
        label_2 = new QLabel(Graphics);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(110, 50, 121, 21));
        en_GFXOffsetY = new QCheckBox(Graphics);
        en_GFXOffsetY->setObjectName(QStringLiteral("en_GFXOffsetY"));
        en_GFXOffsetY->setGeometry(QRect(10, 50, 16, 21));
        label = new QLabel(Graphics);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(110, 20, 121, 21));
        en_GFXOffsetX = new QCheckBox(Graphics);
        en_GFXOffsetX->setObjectName(QStringLiteral("en_GFXOffsetX"));
        en_GFXOffsetX->setGeometry(QRect(10, 20, 16, 21));
        GFXOffSetX = new QSpinBox(Graphics);
        GFXOffSetX->setObjectName(QStringLiteral("GFXOffSetX"));
        GFXOffSetX->setGeometry(QRect(30, 20, 71, 22));
        GFXOffSetX->setButtonSymbols(QAbstractSpinBox::PlusMinus);
        GFXOffSetX->setMinimum(-10000);
        GFXOffSetX->setMaximum(10000);
        GFXOffSetY = new QSpinBox(Graphics);
        GFXOffSetY->setObjectName(QStringLiteral("GFXOffSetY"));
        GFXOffSetY->setGeometry(QRect(30, 50, 71, 22));
        GFXOffSetY->setButtonSymbols(QAbstractSpinBox::PlusMinus);
        GFXOffSetY->setMinimum(-10000);
        GFXOffSetY->setMaximum(10000);
        En_GFXw = new QCheckBox(Graphics);
        En_GFXw->setObjectName(QStringLiteral("En_GFXw"));
        En_GFXw->setGeometry(QRect(10, 80, 16, 21));
        En_GFXh = new QCheckBox(Graphics);
        En_GFXh->setObjectName(QStringLiteral("En_GFXh"));
        En_GFXh->setGeometry(QRect(10, 110, 16, 21));
        label_10 = new QLabel(Graphics);
        label_10->setObjectName(QStringLiteral("label_10"));
        label_10->setGeometry(QRect(110, 110, 121, 21));
        label_11 = new QLabel(Graphics);
        label_11->setObjectName(QStringLiteral("label_11"));
        label_11->setGeometry(QRect(110, 80, 121, 21));
        GFXw = new QSpinBox(Graphics);
        GFXw->setObjectName(QStringLiteral("GFXw"));
        GFXw->setGeometry(QRect(30, 80, 71, 22));
        GFXw->setButtonSymbols(QAbstractSpinBox::PlusMinus);
        GFXw->setMinimum(0);
        GFXw->setMaximum(10000);
        GFXh = new QSpinBox(Graphics);
        GFXh->setObjectName(QStringLiteral("GFXh"));
        GFXh->setEnabled(true);
        GFXh->setGeometry(QRect(30, 110, 71, 22));
        GFXh->setButtonSymbols(QAbstractSpinBox::PlusMinus);
        GFXh->setMinimum(0);
        GFXh->setMaximum(10000);
        label_9 = new QLabel(Graphics);
        label_9->setObjectName(QStringLiteral("label_9"));
        label_9->setGeometry(QRect(150, 200, 81, 21));
        label_7 = new QLabel(Graphics);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setGeometry(QRect(110, 140, 121, 21));
        label_8 = new QLabel(Graphics);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setGeometry(QRect(110, 170, 121, 21));
        En_Framespeed = new QCheckBox(Graphics);
        En_Framespeed->setObjectName(QStringLiteral("En_Framespeed"));
        En_Framespeed->setGeometry(QRect(10, 170, 16, 21));
        Framespeed = new QSpinBox(Graphics);
        Framespeed->setObjectName(QStringLiteral("Framespeed"));
        Framespeed->setGeometry(QRect(30, 170, 71, 22));
        Framespeed->setMinimum(1);
        Framespeed->setMaximum(16);
        FrameStyle = new QComboBox(Graphics);
        FrameStyle->setObjectName(QStringLiteral("FrameStyle"));
        FrameStyle->setGeometry(QRect(30, 200, 111, 22));
        En_Framestyle = new QCheckBox(Graphics);
        En_Framestyle->setObjectName(QStringLiteral("En_Framestyle"));
        En_Framestyle->setGeometry(QRect(10, 200, 16, 21));
        Frames = new QSpinBox(Graphics);
        Frames->setObjectName(QStringLiteral("Frames"));
        Frames->setGeometry(QRect(30, 140, 71, 22));
        Frames->setMinimum(1);
        Frames->setMaximum(10000);
        En_Frames = new QCheckBox(Graphics);
        En_Frames->setObjectName(QStringLiteral("En_Frames"));
        En_Frames->setGeometry(QRect(10, 140, 16, 21));
        En_IsForeground = new QCheckBox(Graphics);
        En_IsForeground->setObjectName(QStringLiteral("En_IsForeground"));
        En_IsForeground->setGeometry(QRect(10, 230, 16, 21));
        IsForeground = new QCheckBox(Graphics);
        IsForeground->setObjectName(QStringLiteral("IsForeground"));
        IsForeground->setGeometry(QRect(30, 230, 201, 21));
        Physics = new QGroupBox(npcedit);
        Physics->setObjectName(QStringLiteral("Physics"));
        Physics->setGeometry(QRect(260, 0, 231, 320));
        En_Width = new QCheckBox(Physics);
        En_Width->setObjectName(QStringLiteral("En_Width"));
        En_Width->setGeometry(QRect(10, 20, 16, 21));
        label_3 = new QLabel(Physics);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(110, 20, 121, 21));
        Width = new QSpinBox(Physics);
        Width->setObjectName(QStringLiteral("Width"));
        Width->setGeometry(QRect(30, 20, 71, 22));
        Width->setMaximum(10000);
        label_4 = new QLabel(Physics);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(110, 50, 121, 21));
        Height = new QSpinBox(Physics);
        Height->setObjectName(QStringLiteral("Height"));
        Height->setGeometry(QRect(30, 50, 71, 22));
        Height->setMaximum(10000);
        En_Height = new QCheckBox(Physics);
        En_Height->setObjectName(QStringLiteral("En_Height"));
        En_Height->setGeometry(QRect(10, 50, 16, 21));
        En_Speed = new QCheckBox(Physics);
        En_Speed->setObjectName(QStringLiteral("En_Speed"));
        En_Speed->setGeometry(QRect(10, 80, 16, 21));
        label_6 = new QLabel(Physics);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(110, 80, 121, 21));
        Speed = new QDoubleSpinBox(Physics);
        Speed->setObjectName(QStringLiteral("Speed"));
        Speed->setGeometry(QRect(30, 80, 71, 22));
        Speed->setFrame(true);
        Speed->setDecimals(2);
        Speed->setMinimum(-100);
        Speed->setMaximum(100);
        Speed->setSingleStep(0.1);
        PlayerBlockTop = new QCheckBox(Physics);
        PlayerBlockTop->setObjectName(QStringLiteral("PlayerBlockTop"));
        PlayerBlockTop->setGeometry(QRect(30, 140, 201, 21));
        PlayerBlock = new QCheckBox(Physics);
        PlayerBlock->setObjectName(QStringLiteral("PlayerBlock"));
        PlayerBlock->setGeometry(QRect(30, 110, 201, 21));
        NPCBlockTop = new QCheckBox(Physics);
        NPCBlockTop->setObjectName(QStringLiteral("NPCBlockTop"));
        NPCBlockTop->setGeometry(QRect(30, 200, 201, 21));
        En_PlayerBlock = new QCheckBox(Physics);
        En_PlayerBlock->setObjectName(QStringLiteral("En_PlayerBlock"));
        En_PlayerBlock->setGeometry(QRect(10, 110, 16, 21));
        NPCBlock = new QCheckBox(Physics);
        NPCBlock->setObjectName(QStringLiteral("NPCBlock"));
        NPCBlock->setGeometry(QRect(30, 170, 201, 21));
        En_PlayerBlockTop = new QCheckBox(Physics);
        En_PlayerBlockTop->setObjectName(QStringLiteral("En_PlayerBlockTop"));
        En_PlayerBlockTop->setGeometry(QRect(10, 140, 16, 21));
        En_NPCBlockTop = new QCheckBox(Physics);
        En_NPCBlockTop->setObjectName(QStringLiteral("En_NPCBlockTop"));
        En_NPCBlockTop->setGeometry(QRect(10, 200, 16, 21));
        En_NPCBlock = new QCheckBox(Physics);
        En_NPCBlock->setObjectName(QStringLiteral("En_NPCBlock"));
        En_NPCBlock->setGeometry(QRect(10, 170, 16, 21));
        En_NoGravity = new QCheckBox(Physics);
        En_NoGravity->setObjectName(QStringLiteral("En_NoGravity"));
        En_NoGravity->setGeometry(QRect(10, 260, 16, 21));
        NoGravity = new QCheckBox(Physics);
        NoGravity->setObjectName(QStringLiteral("NoGravity"));
        NoGravity->setGeometry(QRect(30, 260, 201, 21));
        En_TurnCliff = new QCheckBox(Physics);
        En_TurnCliff->setObjectName(QStringLiteral("En_TurnCliff"));
        En_TurnCliff->setGeometry(QRect(10, 290, 16, 21));
        TurnCliff = new QCheckBox(Physics);
        TurnCliff->setObjectName(QStringLiteral("TurnCliff"));
        TurnCliff->setGeometry(QRect(30, 290, 201, 21));
        En_NoBlockCollision = new QCheckBox(Physics);
        En_NoBlockCollision->setObjectName(QStringLiteral("En_NoBlockCollision"));
        En_NoBlockCollision->setGeometry(QRect(10, 230, 16, 21));
        NoBlockCollision = new QCheckBox(Physics);
        NoBlockCollision->setObjectName(QStringLiteral("NoBlockCollision"));
        NoBlockCollision->setGeometry(QRect(30, 230, 201, 21));
        InGame = new QGroupBox(npcedit);
        InGame->setObjectName(QStringLiteral("InGame"));
        InGame->setGeometry(QRect(10, 300, 241, 291));
        GrabSide = new QCheckBox(InGame);
        GrabSide->setObjectName(QStringLiteral("GrabSide"));
        GrabSide->setGeometry(QRect(30, 20, 201, 21));
        NoFireball = new QCheckBox(InGame);
        NoFireball->setObjectName(QStringLiteral("NoFireball"));
        NoFireball->setGeometry(QRect(30, 200, 201, 21));
        En_JumpHurt = new QCheckBox(InGame);
        En_JumpHurt->setObjectName(QStringLiteral("En_JumpHurt"));
        En_JumpHurt->setGeometry(QRect(10, 80, 16, 21));
        label_5 = new QLabel(InGame);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(120, 140, 111, 21));
        En_NoFireball = new QCheckBox(InGame);
        En_NoFireball->setObjectName(QStringLiteral("En_NoFireball"));
        En_NoFireball->setGeometry(QRect(10, 200, 16, 21));
        En_Score = new QCheckBox(InGame);
        En_Score->setObjectName(QStringLiteral("En_Score"));
        En_Score->setGeometry(QRect(10, 140, 16, 21));
        NoEat = new QCheckBox(InGame);
        NoEat->setObjectName(QStringLiteral("NoEat"));
        NoEat->setGeometry(QRect(30, 170, 201, 21));
        En_NoEat = new QCheckBox(InGame);
        En_NoEat->setObjectName(QStringLiteral("En_NoEat"));
        En_NoEat->setGeometry(QRect(10, 170, 16, 21));
        JumpHurt = new QCheckBox(InGame);
        JumpHurt->setObjectName(QStringLiteral("JumpHurt"));
        JumpHurt->setGeometry(QRect(30, 80, 201, 21));
        En_GrabTop = new QCheckBox(InGame);
        En_GrabTop->setObjectName(QStringLiteral("En_GrabTop"));
        En_GrabTop->setGeometry(QRect(10, 50, 16, 21));
        DontHurt = new QCheckBox(InGame);
        DontHurt->setObjectName(QStringLiteral("DontHurt"));
        DontHurt->setGeometry(QRect(30, 110, 201, 21));
        En_DontHurt = new QCheckBox(InGame);
        En_DontHurt->setObjectName(QStringLiteral("En_DontHurt"));
        En_DontHurt->setGeometry(QRect(10, 110, 16, 21));
        Score = new QComboBox(InGame);
        Score->setObjectName(QStringLiteral("Score"));
        Score->setGeometry(QRect(30, 140, 81, 22));
        En_GrabSide = new QCheckBox(InGame);
        En_GrabSide->setObjectName(QStringLiteral("En_GrabSide"));
        En_GrabSide->setGeometry(QRect(10, 20, 16, 21));
        NoIceball = new QCheckBox(InGame);
        NoIceball->setObjectName(QStringLiteral("NoIceball"));
        NoIceball->setGeometry(QRect(30, 230, 201, 21));
        En_NoIceball = new QCheckBox(InGame);
        En_NoIceball->setObjectName(QStringLiteral("En_NoIceball"));
        En_NoIceball->setGeometry(QRect(10, 230, 16, 21));
        GrabTop = new QCheckBox(InGame);
        GrabTop->setObjectName(QStringLiteral("GrabTop"));
        GrabTop->setGeometry(QRect(30, 50, 201, 21));
        En_NoHammer = new QCheckBox(InGame);
        En_NoHammer->setObjectName(QStringLiteral("En_NoHammer"));
        En_NoHammer->setGeometry(QRect(10, 260, 16, 21));
        NoHammer = new QCheckBox(InGame);
        NoHammer->setObjectName(QStringLiteral("NoHammer"));
        NoHammer->setGeometry(QRect(30, 260, 201, 21));
        groupBox_4 = new QGroupBox(npcedit);
        groupBox_4->setObjectName(QStringLiteral("groupBox_4"));
        groupBox_4->setGeometry(QRect(260, 330, 231, 261));
        PreviewBox = new QGraphicsView(groupBox_4);
        PreviewBox->setObjectName(QStringLiteral("PreviewBox"));
        PreviewBox->setEnabled(false);
        PreviewBox->setGeometry(QRect(10, 20, 211, 231));
        PreviewBox->setAutoFillBackground(true);
        PreviewBox->setStyleSheet(QLatin1String("background-color: rgb(0, 0, 0);\n"
"border-color: rgb(132, 132, 132);"));
        ResetNPCData = new QPushButton(npcedit);
        ResetNPCData->setObjectName(QStringLiteral("ResetNPCData"));
        ResetNPCData->setGeometry(QRect(20, 10, 81, 23));
        NPC_ID = new QGroupBox(npcedit);
        NPC_ID->setObjectName(QStringLiteral("NPC_ID"));
        NPC_ID->setGeometry(QRect(120, 0, 61, 41));
        NPC_ID->setAlignment(Qt::AlignCenter);
        CurrentNPCID = new QLabel(NPC_ID);
        CurrentNPCID->setObjectName(QStringLiteral("CurrentNPCID"));
        CurrentNPCID->setGeometry(QRect(10, 15, 41, 21));
        CurrentNPCID->setAlignment(Qt::AlignCenter);
        isModBox = new QGroupBox(npcedit);
        isModBox->setObjectName(QStringLiteral("isModBox"));
        isModBox->setGeometry(QRect(180, 0, 71, 41));
        isModBox->setAlignment(Qt::AlignCenter);
        isModyfiedL = new QLabel(isModBox);
        isModyfiedL->setObjectName(QStringLiteral("isModyfiedL"));
        isModyfiedL->setGeometry(QRect(5, 9, 61, 31));
        isModyfiedL->setAlignment(Qt::AlignCenter);

        retranslateUi(npcedit);

        FrameStyle->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(npcedit);
    } // setupUi

    void retranslateUi(QWidget *npcedit)
    {
        npcedit->setWindowTitle(QApplication::translate("npcedit", "NPC Configuration edit", 0));
        Graphics->setTitle(QApplication::translate("npcedit", "Graphics", 0));
        label_2->setText(QApplication::translate("npcedit", "offset y", 0));
        en_GFXOffsetY->setText(QString());
        label->setText(QApplication::translate("npcedit", "offset x", 0));
        en_GFXOffsetX->setText(QString());
        GFXOffSetX->setSuffix(QApplication::translate("npcedit", " px", 0));
        GFXOffSetX->setPrefix(QString());
        GFXOffSetY->setSuffix(QApplication::translate("npcedit", " px", 0));
        En_GFXw->setText(QString());
        En_GFXh->setText(QString());
        label_10->setText(QApplication::translate("npcedit", "Height", 0));
        label_11->setText(QApplication::translate("npcedit", "Width", 0));
        GFXw->setSuffix(QApplication::translate("npcedit", " px", 0));
        GFXw->setPrefix(QString());
        GFXh->setSuffix(QApplication::translate("npcedit", " px", 0));
        label_9->setText(QApplication::translate("npcedit", "Frame style", 0));
        label_7->setText(QApplication::translate("npcedit", "Frames", 0));
        label_8->setText(QApplication::translate("npcedit", "Frame speed multiplier", 0));
        En_Framespeed->setText(QString());
        Framespeed->setSuffix(QString());
        FrameStyle->clear();
        FrameStyle->insertItems(0, QStringList()
         << QApplication::translate("npcedit", "Single sprite", 0)
         << QApplication::translate("npcedit", "Left-Right direction", 0)
         << QApplication::translate("npcedit", "Left-Right-Grabbed", 0)
        );
        En_Framestyle->setText(QString());
        Frames->setSuffix(QString());
        En_Frames->setText(QString());
        En_IsForeground->setText(QString());
        IsForeground->setText(QApplication::translate("npcedit", "Foreground", 0));
        Physics->setTitle(QApplication::translate("npcedit", "Physics", 0));
        En_Width->setText(QString());
        label_3->setText(QApplication::translate("npcedit", "Width", 0));
        Width->setSuffix(QApplication::translate("npcedit", " px", 0));
        label_4->setText(QApplication::translate("npcedit", "Height", 0));
        Height->setSuffix(QApplication::translate("npcedit", " px", 0));
        En_Height->setText(QString());
        En_Speed->setText(QString());
        label_6->setText(QApplication::translate("npcedit", "Speed multiplier", 0));
        Speed->setPrefix(QString());
        Speed->setSuffix(QString());
        PlayerBlockTop->setText(QApplication::translate("npcedit", "Player collision top", 0));
        PlayerBlock->setText(QApplication::translate("npcedit", "Player collision", 0));
        NPCBlockTop->setText(QApplication::translate("npcedit", "NPC collision top", 0));
        En_PlayerBlock->setText(QString());
        NPCBlock->setText(QApplication::translate("npcedit", "NPC collision", 0));
        En_PlayerBlockTop->setText(QString());
        En_NPCBlockTop->setText(QString());
        En_NPCBlock->setText(QString());
        En_NoGravity->setText(QString());
        NoGravity->setText(QApplication::translate("npcedit", "Disable Gravity", 0));
        En_TurnCliff->setText(QString());
        TurnCliff->setText(QApplication::translate("npcedit", "Turn on cliff", 0));
        En_NoBlockCollision->setText(QApplication::translate("npcedit", "Disable Block collision", 0));
        NoBlockCollision->setText(QApplication::translate("npcedit", "Disable Block collision", 0));
        InGame->setTitle(QApplication::translate("npcedit", "In game", 0));
        GrabSide->setText(QApplication::translate("npcedit", "Grab side", 0));
        NoFireball->setText(QApplication::translate("npcedit", "Don't kill on fireball", 0));
        En_JumpHurt->setText(QString());
        label_5->setText(QApplication::translate("npcedit", "Score", 0));
        En_NoFireball->setText(QString());
        En_Score->setText(QString());
        NoEat->setText(QApplication::translate("npcedit", "Can't be eaten", 0));
        En_NoEat->setText(QString());
        JumpHurt->setText(QApplication::translate("npcedit", "Jump hurt", 0));
        En_GrabTop->setText(QString());
        DontHurt->setText(QApplication::translate("npcedit", "Don't hurt", 0));
        En_DontHurt->setText(QString());
        Score->clear();
        Score->insertItems(0, QStringList()
         << QApplication::translate("npcedit", "[none]", 0)
         << QApplication::translate("npcedit", "10", 0)
         << QApplication::translate("npcedit", "100", 0)
         << QApplication::translate("npcedit", "200", 0)
         << QApplication::translate("npcedit", "400", 0)
         << QApplication::translate("npcedit", "800", 0)
         << QApplication::translate("npcedit", "1000", 0)
         << QApplication::translate("npcedit", "2000", 0)
         << QApplication::translate("npcedit", "4000", 0)
         << QApplication::translate("npcedit", "8000", 0)
         << QApplication::translate("npcedit", "1up", 0)
         << QApplication::translate("npcedit", "2up", 0)
         << QApplication::translate("npcedit", "5up", 0)
        );
        En_GrabSide->setText(QString());
        NoIceball->setText(QApplication::translate("npcedit", "Don't freeze on fireball", 0));
        En_NoIceball->setText(QString());
        GrabTop->setText(QApplication::translate("npcedit", "Grab top", 0));
        En_NoHammer->setText(QString());
        NoHammer->setText(QApplication::translate("npcedit", "Don't kill on hammer", 0));
        groupBox_4->setTitle(QApplication::translate("npcedit", "Preview", 0));
        ResetNPCData->setText(QApplication::translate("npcedit", "Reset", 0));
        NPC_ID->setTitle(QApplication::translate("npcedit", "NPC ID", 0));
        CurrentNPCID->setText(QApplication::translate("npcedit", "0", 0));
        isModBox->setTitle(QApplication::translate("npcedit", "Modyfied", 0));
        isModyfiedL->setText(QApplication::translate("npcedit", "No", 0));
    } // retranslateUi

};

namespace Ui {
    class npcedit: public Ui_npcedit {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NPCEDIT_H
