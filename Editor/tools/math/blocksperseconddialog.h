#ifndef BLOCKSPERSECONDDIALOG_H
#define BLOCKSPERSECONDDIALOG_H

#include <QDialog>

namespace Ui {
class BlocksPerSecondDialog;
}

class BlocksPerSecondDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BlocksPerSecondDialog(QWidget *parent = 0);
    explicit BlocksPerSecondDialog(unsigned int defBlockSize, QWidget *parent = 0);
    ~BlocksPerSecondDialog();

    unsigned int blockSize() const;
    void setBlockSize(unsigned int blockSize);

    double result() const;
    void setResult(double result);

    double framesPerSecond() const;
    void setFramesPerSecond(double framesPerSecond);

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    unsigned int m_blockSize;
    double m_result;
    double m_framesPerSecond;
    Ui::BlocksPerSecondDialog *ui;
};

#endif // BLOCKSPERSECONDDIALOG_H
