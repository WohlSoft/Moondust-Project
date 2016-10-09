#include "variables_box.h"
#include "ui_variables_box.h"

#include "../../mainwindow.h"
#include <ui_mainwindow.h>


void MainWindow::on_actionVariables_triggered(bool checked)
{
    dock_VariablesBox->setVisible(checked);
    if(checked) dock_VariablesBox->raise();
}


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
    connect(this, SIGNAL(visibilityChanged(bool)), mw()->ui->actionVariables, SLOT(setChecked(bool)));

    setFloating(true);
    setGeometry(
                mwg.x()+mwg.width()-width()-GOffset,
                mwg.y()+120,
                width(),
                height()
                );

    m_lastVisibilityState = isVisible();
    mw()->docks_level_and_world.
          addState(this, &m_lastVisibilityState);
}

VariablesBox::~VariablesBox()
{
    delete ui;
}

void VariablesBox::re_translate()
{
    ui->retranslateUi(this);
}

void VariablesBox::refreshInformation()
{

}
