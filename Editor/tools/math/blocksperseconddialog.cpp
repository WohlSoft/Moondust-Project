#include "blocksperseconddialog.h"
#include "ui_blocksperseconddialog.h"

#include <QMessageBox>

BlocksPerSecondDialog::BlocksPerSecondDialog(QWidget *parent) :
    QDialog(parent),
    m_framesPerSecond(65.0),
    ui(new Ui::BlocksPerSecondDialog)
{
    ui->setupUi(this);
    ui->spinBlockSize->setValue(32);
}

BlocksPerSecondDialog::BlocksPerSecondDialog(unsigned int defBlockSize, QWidget *parent) :
    QDialog(parent),
    m_framesPerSecond(65.0),
    ui(new Ui::BlocksPerSecondDialog)
{
    ui->setupUi(this);
    ui->spinBlockSize->setValue(defBlockSize);
}

BlocksPerSecondDialog::~BlocksPerSecondDialog()
{
    delete ui;
}

void BlocksPerSecondDialog::on_buttonBox_accepted()
{
    if(ui->spinBlocks->value() == 0.0 || ui->spinSecond->value() == 0.0){
        QMessageBox::warning(this, "Error", "Seconds value or blocks value must be non-zero.");
        return;
    }
    m_result = (double)ui->spinBlockSize->value() * ui->spinBlocks->value() / ui->spinSecond->value() / m_framesPerSecond;
    this->accept();
}

void BlocksPerSecondDialog::on_buttonBox_rejected()
{
    this->reject();
}
double BlocksPerSecondDialog::framesPerSecond() const
{
    return m_framesPerSecond;
}

void BlocksPerSecondDialog::setFramesPerSecond(double framesPerSecond)
{
    if(!framesPerSecond)
        return;
    m_framesPerSecond = framesPerSecond;
}

double BlocksPerSecondDialog::result() const
{
    return m_result;
}

void BlocksPerSecondDialog::setResult(double result)
{
    m_result = result;
}

unsigned int BlocksPerSecondDialog::blockSize() const
{
    return m_blockSize;
}

void BlocksPerSecondDialog::setBlockSize(unsigned int blockSize)
{
    m_blockSize = blockSize;
}

