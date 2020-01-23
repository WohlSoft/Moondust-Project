#include "case_fixer.h"
#include "ui_case_fixer.h"

CaseFixer::CaseFixer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CaseFixer)
{
    ui->setupUi(this);
}

CaseFixer::~CaseFixer()
{
    delete ui;
}

void CaseFixer::on_episodeBrowse_clicked()
{

}

void CaseFixer::on_configPackBrowse_clicked()
{

}
