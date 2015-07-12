#ifndef TIP_OF_DAY_H
#define TIP_OF_DAY_H

#include <QDialog>
#include <QStringList>

namespace Ui {
class TipOfDay;
}

class TipOfDay : public QDialog
{
    Q_OBJECT

public:
    explicit TipOfDay(QWidget *parent = 0);
    ~TipOfDay();

private slots:
    void on_prevTip_clicked();
    void on_nextTip_clicked();

private:
    Ui::TipOfDay *ui;
    QStringList tips;
    QStringList tips_viewed;
    int curTip;
};

#endif // TIP_OF_DAY_H
