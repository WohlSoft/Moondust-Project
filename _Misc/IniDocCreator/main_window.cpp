#include "main_window.h"
#include "ui_main_window.h"
#include <QMessageBox>
#include "ini_spec.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_cache.init(this);

    m_ini_spec = new IniSpec(ui->fields, &m_cache);
    ui->fields->setModel(m_ini_spec);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type())
    {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::on_actionQuit_triggered()
{
    this->close();
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, tr("About INI documentation creator"),
                       tr("This is a small utility created by Vitaly Novichkov \"Wohlstand\" in 2017'th year.\n\n"
                          "Utility is created to make easier creation of HTML documentation for "
                          "PGE config pack INI files by opening example files and manual filling of "
                          "missing descriptions and importand information.")
                       );
}
