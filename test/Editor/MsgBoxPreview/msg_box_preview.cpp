#include "msg_box_preview.h"
#include <QFontMetrics>
#include <QPaintEvent>
#include <QFontDatabase>
#include <QPainter>

MsgBoxPreview::MsgBoxPreview(QWidget *parent) :
    QWidget(parent)
{
    QFontDatabase::addApplicationFont(":/PressStart2P.ttf");
    m_font = QFont("Press Start 2P", 14);
    QFontMetrics meter(m_font);
    m_letter_w = int(double(meter.maxWidth()) * 1.1);
    m_letter_h = int(double(meter.height()) * 1.5);
}

MsgBoxPreview::~MsgBoxPreview()
{}

void MsgBoxPreview::setText(const QString &text)
{
    m_currentText = text;
    update();
}

void MsgBoxPreview::setVanillaMode(bool vanilla)
{
    doVanilla = vanilla;
    update();
}

QSize MsgBoxPreview::sizeHint() const
{
    return QSize((28 * m_letter_w) + 20, 200);
}

void MsgBoxPreview::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e)
    QPainter p(this);
    p.fillRect(rect(), Qt::black);

    int X = 0;
    int Y = 0;
//    if((DScreenType == 1 && Z == 2) || (DScreenType == 2 && Z == 1))
//        X = -400;
//    else if((DScreenType == 6 && Z == 2) || (DScreenType == 4 && Z == 2) || (DScreenType == 3 && Z == 1))
//        Y = -300;

    QString SuperText = m_currentText;
    int BoxY = 10;
    int BoxX = 10;
    int TextX = 10;
    int boxW = (28 * m_letter_w);
//    frmMain.renderTexture(400 - GFX.TextBox.w / 2 + X,
//                          BoxY + Y + Y,
//                          GFX.TextBox.w, 20, GFX.TextBox, 0, 0);

    p.fillRect(BoxX, BoxY + Y + Y, boxW, m_letter_h, Qt::darkBlue);

    BoxY += 10;
    bool tempBool = false;
    int B = 0;
    int A = 0;
    QString tempText;

    p.setFont(m_font);
    p.setBrush(Qt::white);
    p.setPen(Qt::white);

    do
    {
        B = 0;

        if(doVanilla)
        {
            for(A = 1; A <= int(SuperText.size()); A++)
            {
                if(SuperText[A - 1] == ' ' || A == SuperText.size())
                {
                    if(A < 28)
                        B = A;
                    else
                        break;
                }
            }
        }
        else
        {
            for(A = 1; A <= int(SuperText.size()) && A <= 27; A++)
            {
                auto c = SuperText[A - 1];

                if(B == 0 && A >= 27)
                    break;

                if(A == SuperText.size())
                {
                    if(A < 28)
                        B = A;
                }
                else if(c == ' ')
                {
                    B = A;
                }
                else if(c == '\n')
                {
                    B = A;
                    break;
                }
            }
        }

        if(B == 0)
            B = A;

        tempText = SuperText.mid(0, size_t(B));
        SuperText.remove(0, size_t(B));
//        frmMain.renderTexture(400 - GFX.TextBox.w / 2 + X, BoxY + Y + Y,
//                              GFX.TextBox.w, 20, GFX.TextBox, 0, 20);

        p.fillRect(BoxX, BoxY + Y + Y, boxW, m_letter_h, Qt::darkBlue);

        if(SuperText.length() == 0 && !tempBool)
        {
            printLine(p, BoxX + TextX + X + (27 * 9) - (tempText.length() * 9), Y + BoxY, tempText);
//            SuperPrint(tempText,
//                       4,
//                       162 + X + (27 * 9) - (tempText.length() * 9),
//                       Y + BoxY);
        }
        else
        {
            printLine(p, BoxX + TextX + X, Y + BoxY, tempText);
//            SuperPrint(tempText, 4, 162 + X, Y + BoxY);
        }
        BoxY += m_letter_h;
        tempBool = true;
    } while(!SuperText.isEmpty());

    p.fillRect(BoxX, BoxY + Y + Y, boxW, m_letter_h, Qt::darkBlue);

    //    frmMain.renderTexture(400 - GFX.TextBox.w / 2 + X, BoxY + Y + Y, GFX.TextBox.w, 10, GFX.TextBox, 0, GFX.TextBox.h - 10);
}

void MsgBoxPreview::printLine(QPainter &p, int x, int y, const QString &text)
{
    int xPos = x;
    for(int i = 0; i < text.size(); ++i)
    {
        p.drawText(QRect(xPos, y, m_letter_w * 2, m_letter_h * 2), QString(text[i]));
        xPos += m_letter_w;
    }
}
