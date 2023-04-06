#ifndef DIALOGUE_ITEM_H
#define DIALOGUE_ITEM_H

#include <QFrame>
#include "textdata/textdata.h"

namespace Ui {
class DialogueItem;
}

class DialogueItem : public QFrame
{
    Q_OBJECT

    TranslateProject *m_project = nullptr;
    QString m_lang;
    QString m_level;
    QString m_text;
    QString m_display;
    int m_type = -1;
    int m_key = -1;
    TranslationData_NPC *m_npc = nullptr;
    TranslationData_EVENT *m_evt = nullptr;

public:
    enum Type
    {
        T_NPC,
        T_EVENT
    };

    explicit DialogueItem(TranslateProject *project,
                          const QString &lang,
                          const QString &level,
                          int type,
                          int key,
                          QWidget *parent = nullptr);
    ~DialogueItem();

    void setLang(const QString &lang);
    QString getText() const;

signals:
    void clicked();

protected:
    void changeEvent(QEvent *e);
    void mousePressEvent(QMouseEvent *e);

private:
    void updateLabel();
    Ui::DialogueItem *ui;
};

#endif // DIALOGUE_ITEM_H
