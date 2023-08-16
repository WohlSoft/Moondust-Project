#include <regex>
#include <string>
#include <QFontMetrics>
#include <QPaintEvent>
#include <QFontDatabase>
#include <QPainter>
#include <QStack>

#include "msg_box_preview.h"


static void s_splitString(std::vector<std::string>& out, const std::string& str, char delimiter)
{
    std::string::size_type beg = 0;
    std::string::size_type end = 0;
    do
    {
        end = str.find(delimiter, beg);
        if(end == std::string::npos)
            end = str.size();
        out.emplace_back(str.substr(beg, end-beg));
        beg = end + 1;
    }
    while(end < str.size() - 1);
}


QString MsgBoxPreview::runPreProcessor()
{
    std::regex cond_if = std::regex("^#if *(\\w+\\(.*\\))$");
    std::regex cond_elif = std::regex("^#elif *(\\w+\\(.*\\))$");
    const std::string cond_endif = "#endif";
    const std::string cond_else = "#else";

    std::regex reg_op_player = std::regex("^player\\((.*)\\)$");

    struct State
    {
        bool open = false;
        bool cond_true = false;
        bool skip_to_endif = false;
    };

    State st;
    std::string ret;
    std::string tmpS = m_currentText.toStdString();
    std::vector<std::string> tmp;
    s_splitString(tmp, tmpS, '\n');

    for(const auto &t : tmp)
    {
        std::smatch m_if;
        std::smatch m_elif;

        if((!st.open && std::regex_search(t, m_if, cond_if)) ||
            (st.open && std::regex_search(t, m_elif, cond_elif)))
        {
            st.cond_true = false;
            if(st.open && st.skip_to_endif)
                continue;

            std::string cond = st.open ? m_elif[1].str() : m_if[1].str();
            std::smatch m_op_player;

            if(std::regex_search(cond, m_op_player, reg_op_player)) // check whaever player
            {
                st.open = true;
                std::string players = m_op_player[1].str();
                std::vector<std::string> nums;
                s_splitString(nums, players, ',');

                for(auto &i : nums)
                {
                    if(std::atoi(i.c_str()) == m_macro.player)
                    {
                        st.cond_true = true;
                        st.skip_to_endif = true;
                        break;
                    }
                }
            }
            else // invalid line
            {
                if(!ret.empty())
                    ret.push_back('\n');
                ret.append(t);
            }
        }
        else if(st.open && t == cond_endif)
        {
            st.open = false;
            st.cond_true = false;
            st.skip_to_endif = false;
        }
        else if(st.open && t == cond_else)
        {
            st.open = true;
            st.cond_true = true;
            if(st.open && st.skip_to_endif)
                continue;

            st.skip_to_endif = false;
        }
        else if(!st.open || st.cond_true) // ordinary line
        {
            if(!ret.empty())
                ret.push_back('\n');
            ret.append(t);
        }
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

    int boxY = 10;
    int boxX = 10;
    int paddingL = m_letter_w / 2;
    int paddingR = m_letter_w / 2;
    int paddingT = m_letter_h / 4;
    int paddingB = m_letter_h / 4;
    int boxW = paddingL + (27 * m_letter_w) + paddingR;

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

    QString SuperText = m_macroEnable ? runPreProcessor() : m_currentText;

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
