#ifndef TRANSLATE_FIELD_H
#define TRANSLATE_FIELD_H

#include <QFrame>

namespace Ui {
class TranslateField;
}

class TranslateField : public QFrame
{
    Q_OBJECT

public:
    explicit TranslateField(QWidget *parent = nullptr);
    ~TranslateField();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::TranslateField *ui;
};

#endif // TRANSLATE_FIELD_H
