#include "png2gifs_gui.h"
#include "png2gifs_converter.h"
#include <ui_png2gifs_gui.h>

#include <QDragEnterEvent>
#include <QCloseEvent>
//#include <QtConcurrent/QtConcurrent>
#include <QFileDialog>
#include <QMessageBox>
#include <QMimeData>
#include <QUrl>


PNG2GIFsGUI::PNG2GIFsGUI(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PNG2GIFsGUI)
{
    ui->setupUi(this);
    isConverting=false;
    connect(&loop, SIGNAL(timeout()), this, SLOT(doLoop()) );
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

    //QtConcurrent::run(this, &doConversion(PNG2GIFsGUI*), this);
    doConversion(this);
}

void PNG2GIFsGUI::closeEvent(QCloseEvent *event)
{
    if(isConverting)
    {
        QMessageBox::warning(this, "Busy", "Conversion in process!");
        event->ignore();
    }
    else
    {
        event->accept();
    }
}

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

    if(ptr->ConversionTask.isEmpty())
    {
        ptr->ui->progressBar->setMaximum(100);
        ptr->ui->progressBar->setValue(0);
        ptr->ui->LabelDropFiles->setText(_backup);
        ptr->ui->LabelDropFiles->setEnabled(true);
        return;
    }

    int answers_success=0;
    int answers_skipped=0;
    int answers_failed=0;

    ptr->isConverting=true;

    ptr->ui->progressBar->setMaximum(ptr->ConversionTask.size());
    ptr->ui->progressBar->setValue(0);

    ptr->loop.start(1);
    qApp->processEvents();
    foreach(QString q, ptr->ConversionTask)
    {
        QString OPath=ptr->ui->targetPath->text();
        QString path=QFileInfo(q).absoluteDir().path()+"/";
        QString fname = QFileInfo(q).fileName();
        if(OPath.isEmpty()) OPath=path;
        switch(doMagicIn(path, fname, OPath))
        {
            case CNV_SUCCESS:answers_success++;break;
            case CNV_FAILED:answers_failed++;break;
            case CNV_SKIPPED:answers_skipped++;break;
        }

        ptr->ui->progressBar->setValue(ptr->ui->progressBar->value()+1);
        qApp->processEvents();
    }
    ptr->loop.stop();

    ptr->isConverting=false;

    ptr->ui->progressBar->setMaximum(100);
    ptr->ui->progressBar->setValue(0);

    ptr->ui->LabelDropFiles->setText(_backup);
    ptr->ui->LabelDropFiles->setEnabled(true);

    QMessageBox::information(ptr, "Converted",
                             QString("Conversion was completed!\n"
                                     "Successfully converted: %1/%2 images\n"
                                     "Failed conversions: %3 files\n"
                                     "And %4 skipped files.")
                             .arg(answers_success)
                             .arg(ptr->ConversionTask.size())
                             .arg(answers_failed).arg(answers_skipped));
}

void PNG2GIFsGUI::doLoop()
{
    qApp->processEvents();
}
