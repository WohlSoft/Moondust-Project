#include "variables_box.h"
#include "ui_variables_box.h"

#include "../../mainwindow.h"
#include <ui_mainwindow.h>

VariablesBox::VariablesBox(QWidget *parent) :
    QDockWidget(parent),
    MWDock_Base(parent),
    ui(new Ui::VariablesBox)
{
    setVisible(false);
    setAttribute(Qt::WA_ShowWithoutActivating);
    ui->setupUi(this);

    QRect mwg = mw()->geometry();
    int GOffset=240;
    mw()->addDockWidget(Qt::RightDockWidgetArea, this);
    connect(mw(), SIGNAL(languageSwitched()), this, SLOT(re_translate()));
    setFloating(true);
    setGeometry(
                mwg.x()+mwg.width()-width()-GOffset,
                mwg.y()+120,
                width(),
                height()
                );

}

VariablesBox::~VariablesBox()
{
    delete ui;
}

void VariablesBox::re_translate()
{
    ui->retranslateUi(this);
}
