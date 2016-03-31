#ifndef SCRIPT_EDITOR_H
#define SCRIPT_EDITOR_H

#include <QMainWindow>

namespace Ui {
class ScriptEditor;
}

class ScriptEditor : public QMainWindow
{
    Q_OBJECT

public:
    explicit ScriptEditor(QWidget *parent = 0);
    ~ScriptEditor();

private:
    Ui::ScriptEditor *ui;
};

#endif // SCRIPT_EDITOR_H
