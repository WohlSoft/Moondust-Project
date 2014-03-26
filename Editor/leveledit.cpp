#include <QtGui>
#include <QGraphicsItem>
#include <QPixmap>
#include <QGraphicsScene>
#include <QProgressDialog>

#include "leveledit.h"
#include "ui_leveledit.h"
#include "lvl_filedata.h"
#include "lvlscene.h"
#include "loadingprocess.h"

leveledit::leveledit(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::leveledit)
{
    FileType = 0;
    setAttribute(Qt::WA_DeleteOnClose);
    isUntitled = true;
    isModyfied = false;
    ui->setupUi(this);
}

leveledit::~leveledit()
{
    delete ui;
}


void leveledit::newFile()
{
    static int sequenceNumber = 1;

    isUntitled = true;
    curFile = tr("npc-%1.txt").arg(sequenceNumber++);
    setWindowTitle(curFile + "[*]");

    /*connect(document(), SIGNAL(contentsChanged()),
            this, SLOT(documentWasModified()));*/
}


bool leveledit::loadFile(const QString &fileName, LevelData FileData)
{
    QFile file(fileName);
    LvlData = FileData;
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("MDI"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }
    StartLvlData = LvlData; //Save current history for made reset

    //QApplication::setOverrideCursor(Qt::WaitCursor);
    //setPlainText(in.readAll());

    /*
    LoadingProcess ProgressBar;
    ProgressBar.setWindowFlags (ProgressBar.windowFlags() &
                                ~Qt::WindowContextHelpButtonHint &
                                ~Qt::WindowCloseButtonHint);
    ProgressBar.thread();

    DrawObjects();

    ProgressBar.close();
*/
    int DataSize=0;

    DataSize = LvlData.sections.size();
    DataSize = LvlData.blocks.size();
    DataSize = LvlData.bgo.size();
    DataSize = LvlData.npc.size();

    QProgressDialog progress("Loading level data", "Abort", 0, DataSize, this);
         progress.setWindowTitle("Loading level data");
         progress.setWindowModality(Qt::WindowModal);
         progress.setWindowFlags(progress.windowFlags() & ~Qt::WindowContextHelpButtonHint & ~Qt::WindowCloseButtonHint);
         progress.setCancelButton(0);

    DrawObjects(progress);

    progress.close();

    ui->graphicsView->verticalScrollBar()->setValue(LvlData.sections[0].size_top);
    ui->graphicsView->horizontalScrollBar()->setValue(LvlData.sections[0].size_left);

    //QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);

    return true;
}

void leveledit::DrawObjects(QProgressDialog &progress)
{
    scene = new LvlScene;
    int DataSize = progress.maximum();

    /*
    pScene = new QGraphicsScene (QRectF (0,0,100000000,100000000));

    QGraphicsEllipseItem* ellipse = new QGraphicsEllipseItem;
    ellipse->setRect(QRectF(50, 50, 20, 20));
    ellipse->setBrush(QBrush(Qt::darkGreen));
    ellipse->setPen(QPen(Qt::red, 1, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
    pScene->addItem(ellipse);

    QGraphicsPixmapItem * testNpc = pScene->addPixmap(QPixmap("../images/unknown_npc.gif"));
    testNpc->translate(25.5, 25.5);
    testNpc->setData(0, "Worker");

    testNpc->setPos(60, 60);
    //pScene->addItem(testNpc);
    */

    int i, total=0;

    for(i=0; i<LvlData.sections.size(); i++)
    {
        if(
            (LvlData.sections[i].size_left!=0) ||
            (LvlData.sections[i].size_top!=0)||
            (LvlData.sections[i].size_bottom!=0)||
            (LvlData.sections[i].size_right!=0)
          )

      scene->makeSectionBG(
       LvlData.sections[i].size_left,
       LvlData.sections[i].size_top,
       LvlData.sections[i].size_right,
       LvlData.sections[i].size_bottom
      );
        total++;
        progress.setValue(total);
    }

    scene->setBGO(LvlData, progress);
    scene->setNPC(LvlData, progress);
    scene->setBlocks(LvlData, progress);
    ui->graphicsView->setScene(scene);
    progress.setValue(DataSize);
}

bool leveledit::save()
{
    if (isUntitled) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

bool leveledit::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
                                                    curFile);
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);
}

bool leveledit::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("MDI"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    /*
    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    //out << toPlainText();
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    */

    return true;
}



QString leveledit::userFriendlyCurrentFile()
{
    return strippedName(curFile);
}

void leveledit::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}

void leveledit::documentWasModified()
{
    isModyfied = true;
}

bool leveledit::maybeSave()
{
    if (isModyfied) {
    QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("MDI"),
                     tr("'%1' has been modified.\n"
                        "Do you want to save your changes?")
                     .arg(userFriendlyCurrentFile()),
                     QMessageBox::Save | QMessageBox::Discard
             | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
            return save();
        else if (ret == QMessageBox::Cancel)
            return false;
    }

    return true;
}

void leveledit::setCurrentFile(const QString &fileName)
{
    curFile = QFileInfo(fileName).canonicalFilePath();
    isUntitled = false;
    //document()->setModified(false);
    setWindowModified(false);
    setWindowTitle(userFriendlyCurrentFile() + "[*]");
}

QString leveledit::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

