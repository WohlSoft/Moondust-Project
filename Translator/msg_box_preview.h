#ifndef MSGBOXPREVIEW_H
#define MSGBOXPREVIEW_H

#include <QWidget>
#include <QString>
#include <QFont>

class MsgBoxPreview : public QWidget
{
    Q_OBJECT

    QFont m_font;
    int m_letter_w = 0;
    int m_letter_h = 0;
    QString m_currentText;
    QStringList m_shownLines;
    bool doVanilla = true;

public:
    explicit MsgBoxPreview(QWidget *parent = nullptr);
    virtual ~MsgBoxPreview();
    void setText(const QString &text);
    void clearText();
    void setFontSize(int size);
    int fontSize() const;
    void setVanillaMode(bool vanilla);

    QSize sizeHint() const;

protected:
    void paintEvent(QPaintEvent *e);

private:
    void updateLines();
    void printLine(QPainter &p, int x, int y, const QString &text);
};

#endif // MSGBOXPREVIEW_H
