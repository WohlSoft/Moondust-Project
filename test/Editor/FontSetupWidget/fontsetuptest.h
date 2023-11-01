#ifndef FONTSETUPTEST_H
#define FONTSETUPTEST_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class FontSetupTest; }
QT_END_NAMESPACE

class FontSetupTest : public QMainWindow
{
    Q_OBJECT

public:
    FontSetupTest(QWidget *parent = nullptr);
    ~FontSetupTest();

private:
    Ui::FontSetupTest *ui;
};
#endif // FONTSETUPTEST_H
