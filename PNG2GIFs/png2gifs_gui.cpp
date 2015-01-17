#include "png2gifs_gui.h"
#include "png2gifs_converter.h"
#include <ui_png2gifs_gui.h>

#include <QDragEnterEvent>
#include <QtConcurrent/QtConcurrent>
#include <QFileDialog>
#include <QMimeData>
#include <QUrl>

PNG2GIFsGUI::PNG2GIFsGUI(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PNG2GIFsGUI)
{
    ui->setupUi(this);
    isConverting=false;
}

PNG2GIFsGUI::~PNG2GIFsGUI()
{
    delete ui;
}


void PNG2GIFsGUI::dragEnterEvent(QDragEnterEvent *e)
{
    if(isConverting) return;
    if (e->mimeData()->hasUrls()) {
        e->acceptProposedAction();
    }
}


void PNG2GIFsGUI::dropEvent(QDropEvent *e)
{
    if(isConverting) return;

    this->raise();
    this->setFocus(Qt::ActiveWindowFocusReason);

    ConversionTask.clear();
    foreach (const QUrl &url, e->mimeData()->urls())
    {
        const QString fileName = url.toLocalFile();
        if(fileName.endsWith(".png", Qt::CaseInsensitive))
            ConversionTask<<fileName;
    }

    taskFuture = QtConcurrent::run(this, &doConversion, this);
}

//void PNG2GIFsGUI::closeEvent(QCloseEvent *event)
//{
//    if (ui->centralWidget->currentSubWindow()) {
//        event->ignore();
//    }
//    else
//    {
//        saveSettings();
//        closeEditor();
//        event->accept();
//    }
//}

void PNG2GIFsGUI::on_LabelDropFiles_clicked()
{
    QStringList fileName_DATA = QFileDialog::getOpenFileNames(this,
       trUtf8("Open file"), qApp->applicationDirPath(),
       QString("PNG Images (*.PNG)\n"),0);
       if(fileName_DATA.isEmpty()) return;

}

void PNG2GIFsGUI::on_targetPathBrowse_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Target Directory"),
                                                 (ui->targetPath->text().isEmpty() ? qApp->applicationDirPath() : ui->targetPath->text()),
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
    if(dir.isEmpty()) return;
    ui->targetPath->setText(dir);
}

void PNG2GIFsGUI::doConversion(PNG2GIFsGUI *ptr)
{
    QString _backup;
    _backup = ptr->ui->LabelDropFiles->text();
    ptr->ui->LabelDropFiles->setText("Converting...");
    ptr->ui->LabelDropFiles->setEnabled(false);

    if(ConversionTask.isEmpty())
    {
        ptr->ui->progressBar->setMaximum(100);
        ptr->ui->progressBar->setValue(0);
        ptr->ui->LabelDropFiles->setText(_backup);
        ptr->ui->LabelDropFiles->setEnabled(true);
        return;
    }

    isConverting=true;

    ptr->ui->progressBar->setMaximum(ConversionTask.size());
    ptr->ui->progressBar->setValue(0);

    //qApp->processEvents();
    foreach(QString q, ConversionTask)
    {
        QString OPath=ptr->ui->targetPath->text();
        QString path=QFileInfo(q).absoluteDir().path()+"/";
        QString fname = QFileInfo(q).fileName();
        if(OPath.isEmpty()) OPath=path;
        doMagicIn(path, fname, OPath);
        ptr->ui->progressBar->setValue(ptr->ui->progressBar->value()+1);
        //qApp->processEvents();
    }

    isConverting=false;

    ptr->ui->progressBar->setMaximum(100);
    ptr->ui->progressBar->setValue(0);

    ptr->ui->LabelDropFiles->setText(_backup);
    ptr->ui->LabelDropFiles->setEnabled(true);
}
