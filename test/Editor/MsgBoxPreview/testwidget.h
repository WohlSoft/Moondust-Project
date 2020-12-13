#ifndef TESTWIDGET_H
#define TESTWIDGET_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class TestWidget; }
QT_END_NAMESPACE

class TestWidget : public QMainWindow
{
    Q_OBJECT

public:
    TestWidget(QWidget *parent = nullptr);
    ~TestWidget();

private slots:
    void on_demoText_textChanged();

    void on_vanillaMode_clicked(bool checked);

private:
    Ui::TestWidget *ui;
};
#endif // TESTWIDGET_H
