#include "script_editor.h"
#include "ui_script_editor.h"

#include <common_features/util.h>
#include <mainwindow.h>



Highlighter::Highlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{}

void Highlighter::highlightBlock(const QString &text)
{
    foreach(const HighlightingRule &rule, highlightingRules)
    {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while(index >= 0)
        {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }
    setCurrentBlockState(0);

    int startIndex = 0;
    if(previousBlockState() != 1)
        startIndex = commentStartExpression.indexIn(text);

    while(startIndex >= 0)
    {
        int endIndex = commentEndExpression.indexIn(text, startIndex);
        int commentLength;
        if(endIndex == -1)
        {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        }
        else
        {
            commentLength = endIndex - startIndex
                            + commentEndExpression.matchedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
    }
}


Highlighter_CPP::Highlighter_CPP(QTextDocument *parent) :
    Highlighter(parent)
{
    HighlightingRule rule;

    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);
    QStringList keywordPatterns;
    keywordPatterns << "\\bchar\\b" << "\\bclass\\b" << "\\bconst\\b"
                    << "\\bdouble\\b" << "\\benum\\b" << "\\bexplicit\\b"
                    << "\\bfriend\\b" << "\\binline\\b" << "\\bint\\b"
                    << "\\blong\\b" << "\\bnamespace\\b" << "\\boperator\\b"
                    << "\\bprivate\\b" << "\\bprotected\\b" << "\\bpublic\\b"
                    << "\\bshort\\b" << "\\bsignals\\b" << "\\bsigned\\b"
                    << "\\bslots\\b" << "\\bstatic\\b" << "\\bstruct\\b"
                    << "\\btemplate\\b" << "\\btypedef\\b" << "\\btypename\\b"
                    << "\\bunion\\b" << "\\bunsigned\\b" << "\\bvirtual\\b"
                    << "\\bvoid\\b" << "\\bvolatile\\b";
    foreach(const QString &pattern, keywordPatterns)
    {
        rule.pattern = QRegExp(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    classFormat.setFontWeight(QFont::Bold);
    classFormat.setForeground(Qt::darkMagenta);
    rule.pattern = QRegExp("\\bQ[A-Za-z]+\\b");
    rule.format = classFormat;
    highlightingRules.append(rule);

    singleLineCommentFormat.setForeground(Qt::red);
    rule.pattern = QRegExp("//[^\n]*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    multiLineCommentFormat.setForeground(Qt::red);

    quotationFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegExp("\".*\"");
    rule.format = quotationFormat;
    highlightingRules.append(rule);

    functionFormat.setFontItalic(true);
    functionFormat.setForeground(Qt::blue);
    rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
    rule.format = functionFormat;
    highlightingRules.append(rule);

    commentStartExpression = QRegExp("/\\*");
    commentEndExpression = QRegExp("\\*/");
}





ScriptEditorWindow::ScriptEditorWindow(QWidget *parent) :
    QTextEdit(parent),
    m_highLighter(nullptr)
{
    setupEditor();
}

ScriptEditorWindow::~ScriptEditorWindow()
{}

void ScriptEditorWindow::setLang(ScriptEditorWindow::ScriptLangs lang)
{
    delete m_highLighter;
    m_highLighter = nullptr;

    switch(lang)
    {
    case LANG_LUA:
        //highlighter = new Highlighter_LUA( this->document() );
        break;
    case LANG_AUTOCODE:
        //highlighter = new Highlighter_AUTOCODE( this->document() );
        break;
    case LANG_TEASCRIPT:
        //highlighter = new Highlighter_TeaScript( this->document() );
        break;
    case LANG_CPP:
        m_highLighter = new Highlighter_CPP(this->document());
        break;
    case LANG_TEXT:
    default:
        m_highLighter = nullptr;
    }
}

void ScriptEditorWindow::setupEditor()
{
    QFont font;
    font.setFamily("Courier");
    font.setFixedPitch(true);
    font.setPointSize(10);

    setFont(font);
    //    QFile file("mainwindow.h");
    //    if (file.open(QFile::ReadOnly | QFile::Text))
    //        editor->setPlainText(file.readAll());
}






//! TEMPORARY! When works will be done, move this pointer into MainWindow (without header)
static ScriptEditor *g_scriptEditor = nullptr;

void MainWindow::on_actionScriptEditor_triggered()
{
    if(!g_scriptEditor)
        g_scriptEditor = new ScriptEditor(this);
    g_scriptEditor->show();
    g_scriptEditor->raise();
    g_scriptEditor->setFocus();
}



ScriptEditor::ScriptEditor(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ScriptEditor)
{
    ui->setupUi(this);
    ui->centralwidget->setViewMode(QMdiArea::TabbedView);
    connect(ui->actionCloseScriptEditor, SIGNAL(triggered(bool)), this, SLOT(close()));
}

ScriptEditor::~ScriptEditor()
{
    delete ui;
}

void ScriptEditor::on_actionNew_triggered()
{
    QMdiSubWindow *window = addSubWindow();
    static int UntitleCounter = 0;
    window->setWindowTitle(QString("Untitled-%1.lua").arg(UntitleCounter++));
    ScriptEditorWindow *box = qobject_cast<ScriptEditorWindow *>(window->widget());
    if(box)
        box->setLang(ScriptEditorWindow::LANG_CPP);
    window->show();
    ui->statusbar->showMessage(tr("Empty script has been added!"), 3000);
}

QMdiSubWindow *ScriptEditor::addSubWindow()
{
    QMdiSubWindow *w = ui->centralwidget->addSubWindow(new ScriptEditorWindow(ui->centralwidget));
    w->setAttribute(Qt::WA_DeleteOnClose, true);
    return w;
}
