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

    struct MacroCond
    {
        int player = 1;
        int state = 1;
        int coins = 0;
        int lives = 0;
    } m_macro;

    bool m_macroEnable = false;

    QString runPreProcessor();

public:
    explicit MsgBoxPreview(QWidget *parent = nullptr);
    virtual ~MsgBoxPreview();
    void setText(const QString &text);
    void clearText();
    void setFontSize(int size);
    int fontSize() const;
    void setVanillaMode(bool vanilla);

    QSize sizeHint() const;

    void setEnableMacros(bool en);

    void setMacroPlayerNum(int plr);
    void setMacroPlayerState(int state);

protected:
    void paintEvent(QPaintEvent *e);

private:
    void updateLines();
    void printLine(QPainter &p, int x, int y, const QString &text);
};

#endif // MSGBOXPREVIEW_H
