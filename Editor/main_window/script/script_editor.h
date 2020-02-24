#pragma once
#ifndef SCRIPT_EDITOR_H
#define SCRIPT_EDITOR_H

#include <QMainWindow>
#include <QMdiSubWindow>
#include <QSyntaxHighlighter>
#include <QTextEdit>

// TODO: Rework this thing completely and start to use Scintilla!

class Highlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    Highlighter(QTextDocument *parent = 0);

protected:
    void highlightBlock(const QString &text) Q_DECL_OVERRIDE;

protected:
    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    QRegExp commentStartExpression;
    QRegExp commentEndExpression;

    QTextCharFormat keywordFormat;
    QTextCharFormat classFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat multiLineCommentFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat functionFormat;
};

class Highlighter_CPP : public Highlighter
{
    Q_OBJECT

public:
    Highlighter_CPP(QTextDocument *parent = 0);
};


class ScriptEditorWindow : public QTextEdit
{
    Q_OBJECT
public:
    enum ScriptLangs {
        LANG_CPP  = -2,
        LANG_TEXT = -1,
        LANG_LUA = 0,
        LANG_AUTOCODE = 1,
        LANG_TEASCRIPT
    };

    explicit ScriptEditorWindow( QWidget *parent = 0);
    virtual ~ScriptEditorWindow();
    void setLang(ScriptLangs lang);

private:
    void setupEditor();
    Highlighter *m_highLighter = nullptr;
};


namespace Ui {
class ScriptEditor;
}

class ScriptEditor : public QMainWindow
{
    Q_OBJECT

public:
    explicit ScriptEditor(QWidget *parent = 0);
    ~ScriptEditor();

private slots:
    void on_actionNew_triggered();

private:
    QMdiSubWindow *addSubWindow();
    Ui::ScriptEditor *ui;
};

#endif // SCRIPT_EDITOR_H
