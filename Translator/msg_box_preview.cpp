#include "msg_box_preview.h"
#include <QFontMetrics>
#include <QPaintEvent>
#include <QFontDatabase>
#include <QPainter>

MsgBoxPreview::MsgBoxPreview(QWidget *parent) :
    QWidget(parent)
{
    QFontDatabase::addApplicationFont(":/PressStart2P.ttf");
    m_font = QFont("Press Start 2P", 12);
    QFontMetrics meter(m_font);
    m_letter_w = int(double(meter.maxWidth()) * 1.1);
    m_letter_h = int(double(meter.height()) * 1.5);
}

MsgBoxPreview::~MsgBoxPreview()
{}

void MsgBoxPreview::setText(const QString &text)
{
    m_currentText = text;
    updateLines();
    update();
}

void MsgBoxPreview::clearText()
{
    m_currentText.clear();
    updateLines();
    update();
}

void MsgBoxPreview::setFontSize(int size)
{
    if(size < 4)
        return;
    m_font.setPointSize(size);
    QFontMetrics meter(m_font);
    m_letter_w = int(double(meter.maxWidth()) * 1.1);
    m_letter_h = int(double(meter.height()) * 1.5);
    update();
}

int MsgBoxPreview::fontSize() const
{
    return m_font.pointSize();
}

void MsgBoxPreview::setVanillaMode(bool vanilla)
{
    doVanilla = vanilla;
    updateLines();
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

    if(m_shownLines.isEmpty())
        return; // Don't draw anything when field is empty

    int boxY = 10;
    int boxX = 10;
    int paddingL = m_letter_w / 2;
    int paddingR = m_letter_w / 2;
    int paddingT = m_letter_h / 4;
    int paddingB = m_letter_h / 4;
    int boxW = paddingL + (28 * m_letter_w) + paddingR;

    boxX = (rect().width() / 2) - (boxW / 2);

    p.fillRect(boxX, boxY, boxW, paddingT, Qt::darkBlue);

    boxY += paddingT;

    p.setFont(m_font);
    p.setBrush(Qt::white);
    p.setPen(Qt::white);

    for(const auto &tempText : m_shownLines)
    {
        p.fillRect(boxX, boxY, boxW, m_letter_h, Qt::darkBlue);

        if(m_shownLines.size() == 1)
            printLine(p, boxX + paddingL + ((27 * m_letter_w) / 2) - ((tempText.length() * m_letter_w) / 2), boxY, tempText);
        else
            printLine(p, boxX + paddingL, boxY, tempText);

        boxY += m_letter_h;
    }

    p.fillRect(boxX, boxY, boxW, paddingB, Qt::darkBlue);
}

void MsgBoxPreview::updateLines()
{
    int B = 0;
    int A = 0;

    m_shownLines.clear();
    if(m_currentText.isEmpty())
        return;

    QString SuperText = m_currentText;

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

        m_shownLines.append(SuperText.mid(0, size_t(B)).trimmed());
        SuperText.remove(0, size_t(B));
    } while(!SuperText.isEmpty());
}

void MsgBoxPreview::printLine(QPainter &p, int x, int y, const QString &text)
{
    int xPos = x;
    for(int i = 0; i < text.size(); ++i)
    {
        p.drawText(QRect(xPos, y, m_letter_w, m_letter_h), QString(text[i]), Qt::AlignLeft|Qt::AlignTop);
        xPos += m_letter_w;
    }
}
