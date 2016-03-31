#include "script_editor.h"
#include "ui_script_editor.h"

ScriptEditor::ScriptEditor(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ScriptEditor)
{
    ui->setupUi(this);
}

ScriptEditor::~ScriptEditor()
{
    delete ui;
}
