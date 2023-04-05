#ifndef TRANSLATE_FIELD_H
#define TRANSLATE_FIELD_H

#include <QFrame>
#include "textdata/textdata.h"
#include "textdata/text_types.h"


namespace Ui {
class TranslateField;
}

class TranslateField : public QFrame
{
    Q_OBJECT

    TranslateProject *m_project = nullptr;
    QString m_lang;
    QString m_level;
    QString *m_string = nullptr;
    QString *m_note = nullptr;

public:
    explicit TranslateField(QWidget *parent = nullptr);
    ~TranslateField();

    void setItem(int group, const QString &level, int type, int key);

    void setText(const QString &text, const QString &note);
    void setLang(const QString &lang);

signals:
    void textChanged(const QString &newText);

protected:
    void changeEvent(QEvent *e);
    bool eventFilter(QObject *object, QEvent *event);

private:
    Ui::TranslateField *ui;
};

#endif // TRANSLATE_FIELD_H
