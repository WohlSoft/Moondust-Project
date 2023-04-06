#include <QMouseEvent>

#include "dialogue_item.h"
#include "ui_dialogue_item.h"


DialogueItem::DialogueItem(TranslateProject *project,
                           const QString &lang,
                           const QString &level,
                           int type,
                           int key,
                           QWidget *parent) :
    QFrame(parent)
    , m_project(project)
    , m_lang(lang)
    , m_level(level)
    , m_type(type)
    , m_key(key)
    , ui(new Ui::DialogueItem)
{
    ui->setupUi(this);
    ui->label->setFont(QFont("Press Start 2P", 8));
    updateLabel();
}

DialogueItem::~DialogueItem()
{
    delete ui;
}

void DialogueItem::setLang(const QString &lang)
{
    m_lang = lang;
    updateLabel();
}

QString DialogueItem::getText() const
{
    return m_text;
}

void DialogueItem::updateLabel()
{
    const auto &l = (*m_project)[m_lang].levels[m_level];
    const auto &orig = (*m_project)["origin"].levels[m_level];
    const bool isOrigin = m_lang == "origin";

    switch(m_type)
    {
    case T_NPC:
    {
        Q_ASSERT(l.npc.contains(m_key));
        auto &n = l.npc[m_key];
        auto &on = orig.npc[m_key];
        m_text = n.talk;
        ui->itemType->setText(QString("NPC-%1 [idx-%2]").arg(on.npc_id).arg(on.npc_index));
        if(!isOrigin && m_text.isEmpty())
            m_text = on.talk;
        break;
    }

    case T_EVENT:
    {
        Q_ASSERT(l.events.contains(m_key));
        auto &e = l.events[m_key];
        auto &oe = orig.events[m_key];
        m_text = e.message;
        ui->itemType->setText(QString("Event-%1").arg(oe.event_index));
        if(!isOrigin && m_text.isEmpty())
            m_text = oe.message;
        break;
    }
    }

    int B = 0;
    int A = 0;
    auto tmpText = m_text;
    m_display.clear();

    do
    {
        B = 0;

        for(A = 1; A <= int(tmpText.size()) && A <= 27; A++)
        {
            auto c = tmpText[A - 1];

            if(B == 0 && A >= 27)
                break;

            if(A == tmpText.size())
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

        if(B == 0)
            B = A;

        if(!m_display.isEmpty())
            m_display.push_back('\n');
        m_display.append(tmpText.mid(0, size_t(B)));
        tmpText.remove(0, size_t(B));
    } while(!tmpText.isEmpty());

    ui->label->setText(m_display);
}

void DialogueItem::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type())
    {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void DialogueItem::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton)
        emit clicked();
}
