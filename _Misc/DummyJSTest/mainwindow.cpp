#include <QDebug>
#include <QMessageBox>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    bindStuff();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::beep()
{
    QMessageBox::information(this, "Meow", "Just :3");
}




void MainWindow::bindStuff()
{

    //Construct proxy with giving pointer to self.
    //No need to keep pointer to proxy becuase:
    //- registered QObject's parent gives guarantee that it
    //  will be destroyed on destroying of parent
    //- QJSEngine itself destroys all binded objects on self destruction
    MWProxy *px = new MWProxy(this);
    //Make a bind entry
    QJSValue objectValue = myEngine.newQObject(px);
    //Feeding our object to QJSEngine with giving internal name
    myEngine.globalObject().setProperty("mainWindow", objectValue);
}

void MainWindow::on_pokeIt_clicked()
{
    //Executing some function from binded object
    QJSValue result = myEngine.evaluate("mainWindow.beep();");

    //Just for case, check a result
    if (result.isError())
    {
        //If script ran with errors
        QMessageBox::critical(this, "Script error", "HOLY #@&*?!\n"
                                                    "Look yourself what happen:\n" + result.toString() );
    }
}



