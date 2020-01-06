#ifndef TESTBOX_H
#define TESTBOX_H

#include <QDialog>
#include "../../../Editor/common_features/direction_switch_widget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class TestBox; }
QT_END_NAMESPACE

class TestBox : public QDialog
{
    Q_OBJECT

public:
    TestBox(QWidget *parent = nullptr);
    ~TestBox();

private slots:
    void directionValue(int direct);
    void on_diagonals_clicked(bool checked);

    void on_roteCenter_clicked(bool checked);

    void on_disable_clicked(bool checked);

private:
    DirectionSwitchWidget *m_testWidget;
    Ui::TestBox *ui;
};
#endif // TESTBOX_H
