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
    QString m_root;
    int m_group = -1;
    int m_type = -1;
    int m_key = -1;
    TrLine *m_tr = nullptr;

public:
    explicit TranslateField(TranslateProject *project, QWidget *parent = nullptr);
    ~TranslateField();

    void setItem(int group, const QString &root, int type, int key);
    void clearItem();

    void setText(const QString &text, const QString &note, bool finished);
    void setLang(const QString &lang);

    const QString &getLang() const;
    const QString &getText() const;

    void reloadEntry();

signals:
    void textChanged(const QString &newText);
    void itemActivated(const QString &lang);

protected:
    void changeEvent(QEvent *e);
    bool eventFilter(QObject *object, QEvent *event);

private:
    Ui::TranslateField *ui;
};

#endif // TRANSLATE_FIELD_H
