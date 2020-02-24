#include "image_calibrator.h"
#include <ui_image_calibrator.h>
#include "../main/graphics.h"
#include "../main/globals.h"
#include "../frame_matrix/matrix.h"
#include <QFileInfo>
#include <QDir>
#include <QSettings>


ImageCalibrator::ImageCalibrator(QWidget *parent) :
    QDialog(parent),
    m_scene(nullptr),
    m_frmX(0),
    m_frmY(0),
    m_imgFrame(nullptr),
    m_phsFrame(nullptr),
    m_physics(nullptr),
    ui(new Ui::ImageCalibrator),
    m_lockUI(false)
{
    ui->setupUi(this);
}

ImageCalibrator::~ImageCalibrator()
{
    delete ui;
}


bool ImageCalibrator::init(QString imgPath)
{
    QString imgFileM;
    QFileInfo ourFile(imgPath);
    QString imgBaseName = ourFile.fileName();
    {
        int i = imgBaseName.lastIndexOf('.');
        imgBaseName.remove(i, imgBaseName.size() - i);
    }
    QString dirPath = ourFile.absoluteDir().path() + "/";

    m_backupPath = dirPath + imgBaseName + "_orig.png";

    if(!QFile::exists(m_backupPath))
    {
        if(!Graphics::loadMaskedImage(dirPath, ourFile.fileName(), imgFileM, m_sprite))
            return false;
    }
    else
        m_sprite = QPixmap(m_backupPath); // load an original sprite instead of current

    if(m_sprite.isNull())
        return false;

    m_spriteOrig = m_sprite;

    m_targetPath = imgPath;
    m_pngPath =     dirPath + imgBaseName + ".png";
    m_gifPath =     dirPath + imgBaseName + ".gif";
    m_gifPathM =    dirPath + imgBaseName + "m.gif";
    m_iniPath =     dirPath + imgBaseName + "_orig_calibrates.ini";

    //generate scene
    ui->PreviewGraph->setScene(new QGraphicsScene(ui->PreviewGraph));
    QGraphicsScene *sc = ui->PreviewGraph->scene();
    m_frmX = 0;
    m_frmY = 0;
    frameOpts xyCell;
    xyCell.offsetX = 0;
    xyCell.offsetY = 0;
    xyCell.W = 0;
    xyCell.H = 0;
    xyCell.used = false;
    xyCell.isDuck = false;
    xyCell.isRightDir = false;
    xyCell.showGrabItem = false;
    // Write default values
    QVector<frameOpts > xRow;

    for(int i = 0; i < 10; i++)
    {
        xRow.clear();

        for(int j = 0; j < 10; j++)
            xRow.push_back(xyCell);

        g_imgOffsets.push_back(xRow);
    }

    loadCalibrates();
    m_imgFrame = sc->addPixmap(m_sprite.copy(m_frmX * 100, m_frmY * 100, 100, 100));
    m_imgFrame->setZValue(0);
    m_imgFrame->setPos(0, 0);
    m_phsFrame = sc->addRect(0, 0, 99, 99, QPen(QBrush(Qt::gray), 1));
    m_phsFrame->setZValue(-2);
    m_phsFrame->setOpacity(0.5);
    m_phsFrame->setPos(0, 0);
    m_physics = sc->addRect(0, 0, 99, 99, QPen(QBrush(Qt::green), 1));
    m_physics->setZValue(4);
    m_physics->setPos(0, 0);
    updateScene();
    return true;
}


void ImageCalibrator::on_FrameX_valueChanged(int arg1)
{
    if(m_lockUI) return;

    m_frmX = arg1;
    updateControls();
    updateScene();
}

void ImageCalibrator::on_FrameY_valueChanged(int arg1)
{
    if(m_lockUI) return;

    m_frmY = arg1;
    updateControls();
    updateScene();
}

void ImageCalibrator::on_OffsetX_valueChanged(int arg1)
{
    if(m_lockUI) return;

    g_imgOffsets[m_frmX][m_frmY].offsetX = arg1;
    updateScene();
}

void ImageCalibrator::on_OffsetY_valueChanged(int arg1)
{
    if(m_lockUI) return;

    g_imgOffsets[m_frmX][m_frmY].offsetY = arg1;
    updateScene();
}

void ImageCalibrator::on_CropW_valueChanged(int arg1)
{
    if(m_lockUI) return;

    g_imgOffsets[m_frmX][m_frmY].W = static_cast<unsigned int>(arg1);
    updateScene();
}

void ImageCalibrator::on_CropH_valueChanged(int arg1)
{
    if(m_lockUI) return;

    g_imgOffsets[m_frmX][m_frmY].H = static_cast<unsigned int>(arg1);
    updateScene();
}

void ImageCalibrator::on_Matrix_clicked()
{
    Matrix dialog(this);
    dialog.setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);
    dialog.setFrame(m_frmX, m_frmY);

    if(dialog.exec() == QDialog::Accepted)
    {
        m_frmX = dialog.frameX;
        m_frmY = dialog.frameY;
        m_lockUI = true;
        ui->FrameX->setValue(m_frmX);
        ui->FrameY->setValue(m_frmY);
        updateControls();
        m_lockUI = false;
    }

    this->raise();
    updateScene();
}

void ImageCalibrator::on_Reset_clicked()
{
    g_imgOffsets[m_frmX][m_frmY].offsetX = 0;
    g_imgOffsets[m_frmX][m_frmY].offsetY = 0;
    g_imgOffsets[m_frmX][m_frmY].W = 0;
    g_imgOffsets[m_frmX][m_frmY].H = 0;
    updateControls();
    updateScene();
}

void ImageCalibrator::makeBackup()
{
    if(!QFile::exists(m_backupPath))
        m_spriteOrig.save(m_backupPath, "PNG");
}

void ImageCalibrator::on_WritePNG_GIF_clicked()
{
    makeBackup();
    QPixmap target = generateTarget();
    target.save(m_pngPath, "PNG");

    QImage targetGif = target.toImage();
    Graphics::toMaskedGif(targetGif, m_gifPath);
    saveCalibrates();
    QMessageBox::information(this,
                             "Image was overwritten",
                             "Calibrated sprite was saved in:\n" + m_pngPath + "\n" + m_gifPath, QMessageBox::Ok);
}

void ImageCalibrator::on_WritePNG_clicked()
{
    makeBackup();
    QPixmap target = generateTarget();
    target.save(m_pngPath, "PNG");
    saveCalibrates();
    QMessageBox::information(this,
                             "Image was overwritten",
                             "Calibrated sprite was saved in:\n" + m_pngPath, QMessageBox::Ok);
}

void ImageCalibrator::on_WriteGIF_clicked()
{
    makeBackup();
    QImage target = generateTarget().toImage();
    Graphics::toMaskedGif(target, m_gifPath);
    saveCalibrates();
    QMessageBox::information(this,
                             "Image was overwritten",
                             "Calibrated sprite was saved in:\n" + m_gifPath, QMessageBox::Ok);
}

void ImageCalibrator::updateControls()
{
    m_lockUI = true;
    ui->OffsetX->setValue(g_imgOffsets[m_frmX][m_frmY].offsetX);
    ui->OffsetY->setValue(g_imgOffsets[m_frmX][m_frmY].offsetY);
    ui->CropW->setValue(static_cast<int>(g_imgOffsets[m_frmX][m_frmY].W));
    ui->CropH->setValue(static_cast<int>(g_imgOffsets[m_frmX][m_frmY].H));
    m_lockUI = false;
}

void ImageCalibrator::updateScene()
{
    m_imgFrame->setPixmap(
        getFrame(m_frmX,
                 m_frmY,
                 g_imgOffsets[m_frmX][m_frmY].offsetX,
                 g_imgOffsets[m_frmX][m_frmY].offsetY,
                 static_cast<int>(g_imgOffsets[m_frmX][m_frmY].W),
                 static_cast<int>(g_imgOffsets[m_frmX][m_frmY].H))
    );
    m_physics->setRect(g_framesX[m_frmX][m_frmY].offsetX,
                       g_framesX[m_frmX][m_frmY].offsetY,
                       g_frameWidth - 1,
                       (g_framesX[m_frmX][m_frmY].isDuck ?
                        g_frameHeightDuck : g_frameHeight) - 1);
}

void ImageCalibrator::saveCalibrates()
{
    QSettings conf(m_iniPath, QSettings::IniFormat);

    for(int i = 0; i < 10; i++)
    {
        for(int j = 0; j < 10; j++)
        {
            conf.beginGroup(QString::number(i) + "-" + QString::number(j));
            conf.setValue("x", g_imgOffsets[i][j].offsetX);
            conf.setValue("y", g_imgOffsets[i][j].offsetY);
            conf.setValue("w", g_imgOffsets[i][j].W);
            conf.setValue("h", g_imgOffsets[i][j].H);
            conf.endGroup();
        }
    }
}

void ImageCalibrator::loadCalibrates()
{
    QSettings conf(m_iniPath, QSettings::IniFormat);

    for(int i = 0; i < 10; i++)
    {
        for(int j = 0; j < 10; j++)
        {
            conf.beginGroup(QString::number(i) + "-" + QString::number(j));
            g_imgOffsets[i][j].offsetX = conf.value("x", 0).toInt();
            g_imgOffsets[i][j].offsetY = conf.value("y", 0).toInt();
            g_imgOffsets[i][j].W = conf.value("w", 0).toUInt();
            g_imgOffsets[i][j].H = conf.value("h", 0).toUInt();
            conf.endGroup();
        }
    }
}

QPixmap ImageCalibrator::generateTarget()
{
    QPixmap target(1000, 1000);
    target.fill(Qt::transparent);
    QPainter x(&target);

    for(int i = 0; i < 10; i++)
    {
        for(int j = 0; j < 10; j++)
        {
            x.drawPixmap(i * 100, j * 100, 100, 100,
                         getFrame(i, j, g_imgOffsets[i][j].offsetX, g_imgOffsets[i][j].offsetY,
                                  static_cast<int>(g_imgOffsets[i][j].W),
                                  static_cast<int>(g_imgOffsets[i][j].H))
                        );
        }
    }

    x.end();
    return target;
}

QPixmap ImageCalibrator::getFrame(int x, int y, int oX, int oY, int cW, int cH)
{
    int pX = 0, pY = 0, pXo = 0, pYo = 0;
    int X, Y;
    X = x * 100 + oX;
    Y = y * 100 + oY;

    if(X < 0)
    {
        pX = X;
        pXo = -pX;
    }
    else if(X + 100 > 1000)
        pXo = X + 100 - 1000;

    if(Y < 0)
    {
        pY = Y;
        pYo = -pY;
    }
    else if(Y + 100 > 1000)
        pYo = Y + 100 - 1000;

    qDebug() << pX << 100 - pXo;
    qDebug() << pY << 100 - pYo;
    qDebug() << X - pX << Y - pY << 100 - pXo << 100 - pYo;
    QPixmap frame(100, 100);
    frame.fill(Qt::transparent);
    QPainter pt(&frame);
    pt.drawPixmap(-pX, -pY, 100 - pXo - cW, 100 - pYo - cH,
                  m_sprite.copy(X - pX,
                                Y - pY, 100 - pXo - cW, 100 - pYo - cH));
    pt.end();
    return frame;
}
