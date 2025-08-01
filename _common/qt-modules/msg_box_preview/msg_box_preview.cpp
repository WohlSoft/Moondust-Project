#include <QFontMetrics>
#include <QPaintEvent>
#include <QFontDatabase>
#include <QPainter>
#include <QStack>

#include "msg_box_preview.h"
#include "msg_macros.h"


QString MsgBoxPreview::runPreProcessor()
{
    std::string ret;
    MsgMacroErrors err;
    m_errorText.clear();
    msgMacroProcess(m_currentText.toStdString(), ret, m_macro.player, m_macro.state, &err);

    switch(err)
    {
    case MSG_MACRO_ERROR_BAD_CMD_SYNTAX:
        m_errorText = tr("Bad #command syntax");
        break;
    case MSG_MACRO_ERROR_BAD_FUNC_SYNTAX:
        m_errorText = tr("Bad function() syntax");
        break;
    case MSG_MACRO_ERROR_UNKNOWN_CMD:
        m_errorText = tr("Unknown #command specified");
        break;
    case MSG_MACRO_ERROR_UNKNOWN_FUNC:
        m_errorText = tr("Unknown function() specified");
        break;
    case MSG_MACRO_ERROR_BAD_FUNC_ARGS:
        m_errorText = tr("Bad function() arguments specified");
        break;
    case MSG_MACRO_ERROR_EXTRA_SYMBOLS_AT_END:
        m_errorText = tr("Extra characters at end of the code line");
        break;
    case MSG_MACRO_ERROR_ILLEGAL_CMD:
        m_errorText = tr("Illegal usage of a #command");
        break;
    default:
        break;
    }

    return QString::fromStdString(ret);
}

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
    updateGeometry();
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

void MsgBoxPreview::setPreviewType(PreviewType pt)
{
    m_previewType = (int)pt;
    updateLines();
    update();
}

QSize MsgBoxPreview::sizeHint() const
{
    return QSize((28 * m_letter_w) + 20, 200);
}

void MsgBoxPreview::setEnableMacros(bool en)
{
    m_macroEnable = en;
    updateLines();
    update();
}

void MsgBoxPreview::setMacroPlayerNum(int plr)
{
    m_macro.player = plr;
    updateLines();
    update();
}

void MsgBoxPreview::setMacroPlayerState(int state)
{
    m_macro.state = state;
    updateLines();
    update();
}

void MsgBoxPreview::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e)
    QPainter p(this);
    p.fillRect(rect(), Qt::black);

    if(m_shownLines.isEmpty())
        return; // Don't draw anything when field is empty

    const int boxYtop = 10;
    const int paddingL = m_letter_w / 2;
    const int paddingR = m_letter_w / 2;
    const int paddingT = m_letter_h / 4;
    const int paddingB = m_letter_h / 4;
    const int boxW = paddingL + (27 * m_letter_w) + paddingR;

    switch(m_previewType)
    {
    default:
    case PREVIEW_MESSAGEBOX:
    {
        int boxY = boxYtop;
        int boxX = 10;
        int boxH = 0;

        boxX = (rect().width() / 2) - (boxW / 2);

        p.fillRect(boxX, boxY, boxW, paddingT, Qt::darkBlue);

        boxY += paddingT;
        boxH += paddingT  + paddingB - 2;

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
            boxH += m_letter_h;
        }

        p.fillRect(boxX, boxY, boxW, paddingB, Qt::darkBlue);
        p.setPen(QPen(Qt::white, 2));
        p.setBrush(Qt::transparent);
        p.drawRect(boxX, boxYtop, boxW, boxH);
        break;
    }

    case PREVIEW_FILE_TITLE:
    {
        QRect r = rect();
        r.setLeft(r.left() + 10);
        r.setTop(r.top() + 10);
        r.setRight(r.right() - 10);
        r.setBottom(r.bottom() - 10);
        p.setFont(m_font);
        p.setBrush(Qt::white);
        p.setPen(Qt::white);
        p.drawText(r, Qt::AlignHCenter|Qt::AlignTop, m_currentText);
        break;
    }

    case PREVIEW_LEVEL_TITLE:
    {
        QRect r = rect();
        r.setLeft(r.left() + 10);
        r.setTop(r.top() + 10);
        r.setRight(r.right() - 10);
        r.setBottom(r.bottom() - 10);
        p.setFont(m_font);
        p.setBrush(Qt::white);
        p.setPen(Qt::white);
        p.drawText(r, Qt::AlignLeft|Qt::AlignTop|Qt::TextWordWrap, m_currentText);
        break;
    }

    case PREVIEW_CREDITS:
        if(!m_currentText.isEmpty())
        {
            p.setFont(m_font);
            p.setBrush(Qt::white);
            p.setPen(Qt::white);
            p.drawText(rect(), Qt::AlignCenter, m_currentText);
        }
        break;

    case PREVIEW_SCRIPT_PRINT:
    {
        QRect r = rect();
        r.setLeft(r.left() + 10);
        r.setTop(r.top() + 10);
        r.setRight(r.right() - 10);
        r.setBottom(r.bottom() - 10);
        p.setFont(m_font);
        p.setBrush(Qt::white);
        p.setPen(Qt::white);
        p.drawText(r, Qt::AlignLeft|Qt::AlignTop, m_currentText);
        break;
    }
    }


    if(m_macroEnable && !m_errorText.isEmpty())
    {
        int errW = rect().width() - 20;
        int errH = 0;
        int errYtop = 30;
        int errY = errYtop;
        int maxChars = (errW - (paddingL + paddingR)) / m_letter_w;

        QString toPrint = tr("Macro error: ") + m_errorText;

        p.fillRect(10, errY, errW, paddingT, Qt::darkRed);

        errY += paddingT;
        errH += paddingT  + paddingB - 2;

        for(int i = 0, line = 0, printAt = 0, breakAt = 0; ; ++i, ++line)
        {
            if(i < toPrint.size() && toPrint[i] == ' ')
                breakAt = i;

            if(toPrint.size() - printAt <= maxChars || i >= toPrint.size())
            {
                p.fillRect(10, errY, errW, m_letter_h, Qt::darkRed);
                printLine(p, 10 + paddingL, errY, toPrint.mid(printAt, -1));
                errY += m_letter_h;
                errH += m_letter_h;
                break;
            }

            if(line >= maxChars)
            {
                int skip = 1;

                p.fillRect(10, errY, errW, m_letter_h, Qt::darkRed);

                if(breakAt == printAt)
                {
                    breakAt = printAt + line;
                    skip = 0;
                }

                printLine(p, 10 + paddingL, errY, toPrint.mid(printAt, breakAt - printAt));
                errY += m_letter_h;
                errH += m_letter_h;
                printAt = breakAt + skip;
                breakAt = printAt;
                line = i - printAt;
            }
        }

        p.fillRect(10, errY, errW, paddingB, Qt::darkRed);
        p.setPen(QPen(Qt::white, 2));
        p.setBrush(Qt::transparent);
        p.drawRect(10, errYtop, errW, errH);
    }
}

void MsgBoxPreview::updateLines()
{
    int B = 0;
    int A = 0;

    m_shownLines.clear();
    if(m_currentText.isEmpty())
        return;

    QString SuperText = m_macroEnable ? runPreProcessor() : m_currentText;

    if(doVanilla)
    {
        // Simulate vanilla behaviour: print all non-ASCII characters as spaces
        std::string text = SuperText.toStdString();

        for(char &c : text)
        {
            if(c < 0 || c == '\r' || c == '\n' || c == '\t')
                c = ' ';
        }

        SuperText = QString::fromStdString(text);
    }


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

        auto lin = SuperText.mid(0, size_t(B));

        if(lin.endsWith('\n'))
            lin.remove(lin.size()-1, 1);

        m_shownLines.append(lin);

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
