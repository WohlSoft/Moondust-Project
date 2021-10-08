#include <QFileInfo>
#include <QDir>
#include <QSettings>

#include "image_calibrator.h"
#include <ui_image_calibrator.h>
#include "calibration_main.h"
#include "main/graphics.h"
#include "main/mouse_scene.h"
#include "frame_matrix/matrix.h"


ImageCalibrator::ImageCalibrator(Calibration *conf, QWidget *parent) :
    QDialog(parent),
    m_conf(conf),
    m_frmX(0),
    m_frmY(0),
    ui(new Ui::ImageCalibrator),
    m_lockUI(false)
{
    m_mw = qobject_cast<CalibrationMain*>(parent);
    Q_ASSERT(m_mw);
    Q_ASSERT(conf);
    ui->setupUi(this);

    ui->preview->setAllowScroll(true);
    QObject::connect(ui->preview, &FrameTuneScene::deltaX,
                     this, [this](Qt::MouseButton button, int delta)->void
    {
        switch(button)
        {
        case Qt::LeftButton:
            if(ui->hitboxMove->isChecked())
            {
                m_conf->frames[{m_frmX, m_frmY}].offsetX += delta;
                m_hitboxModified = true;
            }
            ui->OffsetX->setValue(ui->OffsetX->value() - delta);
            break;
        case Qt::RightButton:
            ui->CropW->setValue(ui->CropW->value() - delta);
            break;
        default:
            break;
        }
    });

    QObject::connect(ui->preview, &FrameTuneScene::deltaY,
                     this, [this](Qt::MouseButton button, int delta)->void
    {
        switch(button)
        {
        case Qt::LeftButton:
            if(ui->hitboxMove->isChecked())
            {
                m_conf->frames[{m_frmX, m_frmY}].offsetY += delta;
                m_hitboxModified = true;
            }
            ui->OffsetY->setValue(ui->OffsetY->value() - delta);
            break;
        case Qt::RightButton:
            ui->CropH->setValue(ui->CropH->value() - delta);
            break;
        default:
            break;
        }
    });

    QObject::connect(ui->preview, &FrameTuneScene::mouseReleased,
                     this, [this]()->void
    {
        if(ui->preview->mode() == FrameTuneScene::MODE_NONE)
            m_matrix->updateScene(generateTarget());
        else
            frameEdited();
    });

    m_matrix = new Matrix(conf, m_mw, this);
    m_matrix->setModal(false);
    m_matrix->setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);
    m_matrix->allowEnableDisable(false);
    m_matrix->changeGridSize(conf->matrixWidth, conf->matrixHeight);

    QObject::connect(m_matrix, &Matrix::frameSelected,
                     this, &ImageCalibrator::frameSelected);

    QObject::connect(m_matrix, &Matrix::referenceSelected,
                     this, &ImageCalibrator::referenceSelected);

    QObject::connect(&m_watchingFrame, &QFileSystemWatcher::fileChanged,
                     this, &ImageCalibrator::tempFrameUpdated);

    ui->toolDrag->setShortcut(Qt::Key_D);
    ui->toolPencil->setShortcut(Qt::Key_N);
    ui->toolPicker->setShortcut(Qt::Key_O);
    ui->toolRubber->setShortcut(Qt::Key_R);

    ui->toolDrag->setChecked(true);

    QObject::connect(ui->toolDrag, &QPushButton::clicked,
                     this, &ImageCalibrator::toolChanged);
    QObject::connect(ui->toolPencil, &QPushButton::clicked,
                     this, &ImageCalibrator::toolChanged);
    QObject::connect(ui->toolPicker, &QPushButton::clicked,
                     this, &ImageCalibrator::toolChanged);
    QObject::connect(ui->toolRubber, &QPushButton::clicked,
                     this, &ImageCalibrator::toolChanged);
}

ImageCalibrator::~ImageCalibrator()
{
    if(m_matrix)
        m_matrix->close();
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
    m_spriteModified = false;

    m_targetPath = imgPath;
    m_pngPath =     dirPath + imgBaseName + ".png";
    m_gifPath =     dirPath + imgBaseName + ".gif";
    m_gifPathM =    dirPath + imgBaseName + "m.gif";
    m_iniPath =     dirPath + imgBaseName + "_orig_calibrates.ini";

    m_frmX = 0;
    m_frmY = 0;

    CalibrationFrame xyCell;
    xyCell.offsetX = 0;
    xyCell.offsetY = 0;
    xyCell.w = 0;
    xyCell.h = 0;
    xyCell.used = false;
    xyCell.isDuck = false;
    xyCell.isRightDir = false;
    xyCell.showGrabItem = false;

    // Write default values
    QVector<CalibrationFrame > xRow;

    for(int y = 0; y < m_conf->matrixHeight; y++)
    {
        xRow.clear();

        for(int x = 0; x < m_conf->matrixWidth; x++)
            xRow.push_back(xyCell);

        m_imgOffsets.push_back(xRow);
    }

    loadCalibrates();

    updateScene();
    return true;
}

void ImageCalibrator::unInit()
{
    delete m_matrix;
    m_matrix = nullptr;
}

void ImageCalibrator::closeEvent(QCloseEvent *)
{
    if(m_matrix)
        m_matrix->close();
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

void ImageCalibrator::frameSelected(int x, int y)
{
    m_frmX = x;
    m_frmY = y;
    m_lockUI = true;
    ui->FrameX->setValue(m_frmX);
    ui->FrameY->setValue(m_frmY);
    updateControls();
    m_lockUI = false;
    updateScene();
}

void ImageCalibrator::referenceSelected(int x, int y)
{
    ui->preview->setRefImage(getFrame(x,
                                      y,
                                      m_imgOffsets[x][y].offsetX,
                                      m_imgOffsets[x][y].offsetY,
                                      static_cast<int>(m_imgOffsets[x][y].w),
                                      static_cast<int>(m_imgOffsets[x][y].h)));
}

void ImageCalibrator::on_OffsetX_valueChanged(int arg1)
{
    if(m_lockUI) return;

    m_imgOffsets[m_frmX][m_frmY].offsetX = arg1;
    updateScene();
}

void ImageCalibrator::on_OffsetY_valueChanged(int arg1)
{
    if(m_lockUI) return;

    m_imgOffsets[m_frmX][m_frmY].offsetY = arg1;
    updateScene();
}

void ImageCalibrator::on_CropW_valueChanged(int arg1)
{
    if(m_lockUI) return;

    m_imgOffsets[m_frmX][m_frmY].w = static_cast<unsigned int>(arg1);
    updateScene();
}

void ImageCalibrator::on_CropH_valueChanged(int arg1)
{
    if(m_lockUI) return;

    m_imgOffsets[m_frmX][m_frmY].h = static_cast<unsigned int>(arg1);
    updateScene();
}

bool ImageCalibrator::hitboxModified() const
{
    return m_hitboxModified;
}

bool ImageCalibrator::hitboxNeedSave() const
{
    return m_hitboxNeedSave;
}

void ImageCalibrator::on_Matrix_clicked(bool checked)
{
    if(checked)
    {
        m_matrix->show();
        QRect g = this->frameGeometry();
        m_matrix->move(g.right(), g.top());
        m_matrix->update();
        m_matrix->repaint();
    }
    else
        m_matrix->hide();
}

void ImageCalibrator::on_Reset_clicked()
{
    m_imgOffsets[m_frmX][m_frmY].offsetX = 0;
    m_imgOffsets[m_frmX][m_frmY].offsetY = 0;
    m_imgOffsets[m_frmX][m_frmY].w = 0;
    m_imgOffsets[m_frmX][m_frmY].h = 0;
    updateControls();
    updateScene();
}

void ImageCalibrator::makeBackup()
{
    if(m_spriteModified || !QFile::exists(m_backupPath))
        m_spriteOrig.save(m_backupPath, "PNG");
    m_spriteModified = false;
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
    ui->OffsetX->setValue(m_imgOffsets[m_frmX][m_frmY].offsetX);
    ui->OffsetY->setValue(m_imgOffsets[m_frmX][m_frmY].offsetY);
    ui->CropW->setValue(static_cast<int>(m_imgOffsets[m_frmX][m_frmY].w));
    ui->CropH->setValue(static_cast<int>(m_imgOffsets[m_frmX][m_frmY].h));
    m_lockUI = false;
}

void ImageCalibrator::updateScene()
{
    ui->preview->setImage(getFrame(m_frmX,
                                   m_frmY,
                                   m_imgOffsets[m_frmX][m_frmY].offsetX,
                                   m_imgOffsets[m_frmX][m_frmY].offsetY,
                                   static_cast<int>(m_imgOffsets[m_frmX][m_frmY].w),
                                   static_cast<int>(m_imgOffsets[m_frmX][m_frmY].h)));

    ui->preview->setGlobalSetup(*m_conf);
    ui->preview->setFrameSetup(m_conf->frames[{m_frmX, m_frmY}]);
}

void ImageCalibrator::saveCalibrates()
{
    // FIXME: Make grid size and frame size being dynamical, not fixed 100x100!!!
    QSettings conf(m_iniPath, QSettings::IniFormat);

    for(int i = 0; i < 10; i++)
    {
        for(int j = 0; j < 10; j++)
        {
            conf.beginGroup(QString::number(i) + "-" + QString::number(j));
            conf.setValue("x", m_imgOffsets[i][j].offsetX);
            conf.setValue("y", m_imgOffsets[i][j].offsetY);
            conf.setValue("w", m_imgOffsets[i][j].w);
            conf.setValue("h", m_imgOffsets[i][j].h);
            conf.endGroup();
        }
    }

    if(m_hitboxModified)
        m_hitboxNeedSave = true;
}

void ImageCalibrator::loadCalibrates()
{
    // FIXME: Make grid size and frame size being dynamical, not fixed 100x100!!!
    QSettings conf(m_iniPath, QSettings::IniFormat);

    for(int i = 0; i < 10; i++)
    {
        for(int j = 0; j < 10; j++)
        {
            conf.beginGroup(QString::number(i) + "-" + QString::number(j));
            m_imgOffsets[i][j].offsetX = conf.value("x", 0).toInt();
            m_imgOffsets[i][j].offsetY = conf.value("y", 0).toInt();
            m_imgOffsets[i][j].w = conf.value("w", 0).toUInt();
            m_imgOffsets[i][j].h = conf.value("h", 0).toUInt();
            conf.endGroup();
        }
    }
}

QPixmap ImageCalibrator::generateTarget()
{
    // FIXME: Make grid size and frame size being dynamical, not fixed 100x100!!!
    QPixmap target(m_sprite.width(), m_sprite.height());
    target.fill(Qt::transparent);
    QPainter x(&target);

    int cellWidth = m_sprite.width() / m_conf->matrixWidth;
    int cellHeight = m_sprite.height() / m_conf->matrixHeight;

    for(int i = 0; i < m_conf->matrixWidth; i++)
    {
        for(int j = 0; j < m_conf->matrixHeight; j++)
        {
            x.drawPixmap(i * cellWidth, j * cellHeight, cellWidth, cellHeight,
                         getFrame(i, j, m_imgOffsets[i][j].offsetX, m_imgOffsets[i][j].offsetY,
                                  static_cast<int>(m_imgOffsets[i][j].w),
                                  static_cast<int>(m_imgOffsets[i][j].h))
                        );
        }
    }

    x.end();
    return target;
}

QPixmap ImageCalibrator::getFrame(int x, int y, int oX, int oY, int cW, int cH)
{
    // FIXME: Make grid size and frame size being dynamical, not fixed 100x100!!!
    int pX = 0, pY = 0, pXo = 0, pYo = 0;
    int X, Y;
    int cellWidth = m_sprite.width() / m_conf->matrixWidth;
    int cellHeight = m_sprite.height() / m_conf->matrixHeight;

    X = x * cellWidth + oX;
    Y = y * cellHeight + oY;

    if(X < 0)
    {
        pX = X;
        pXo = -pX;
    }
    else if(X + cellWidth > m_sprite.width())
        pXo = X + cellWidth - m_sprite.width();

    if(Y < 0)
    {
        pY = Y;
        pYo = -pY;
    }
    else if(Y + cellHeight > m_sprite.height())
        pYo = Y + cellHeight - m_sprite.height();

    qDebug() << pX << cellWidth - pXo;
    qDebug() << pY << cellHeight - pYo;
    qDebug() << X - pX << Y - pY << cellWidth - pXo << cellHeight - pYo;

    QPixmap frame(cellWidth, cellHeight);
    frame.fill(Qt::transparent);

    QPainter pt(&frame);
    pt.drawPixmap(QRect(-pX,
                        -pY,
                        cellWidth - pXo - cW,
                        cellHeight - pYo - cH),
                  m_sprite,
                  QRect(X - pX,
                        Y - pY,
                        cellWidth - pXo - cW,
                        cellHeight - pYo - cH));
    pt.end();

    return frame;
}

void ImageCalibrator::on_refOpacity_sliderMoved(int position)
{
    ui->preview->setRefOpacity(position);
}


void ImageCalibrator::on_refClear_clicked()
{
    ui->preview->clearRef();
}


void ImageCalibrator::on_openFrameInEditor_clicked()
{
    auto of = m_watchingFrame.files();
    for(auto &f : of)
        m_watchingFrame.removePath(f);

    QString tempFramePath = QDir::tempPath() + "/calibrator-temp-frame.png";
    int cellWidth = m_sprite.width() / m_conf->matrixWidth;
    int cellHeight = m_sprite.height() / m_conf->matrixHeight;
    auto frm = m_sprite.copy((m_frmX * cellWidth), (m_frmY * cellHeight), cellWidth, cellHeight);
    if(frm.save(tempFramePath, "PNG"))
    {
        m_tempFileX = m_frmX;
        m_tempFileY = m_frmY;
        QProcess::startDetached("gimp", {tempFramePath});
        m_watchingFrame.addPath(tempFramePath);
    }
}

void ImageCalibrator::toolChanged(bool)
{
    auto *s = sender();

    ui->toolDrag->setChecked(false);
    ui->toolPencil->setChecked(false);
    ui->toolPicker->setChecked(false);
    ui->toolRubber->setChecked(false);

    if(s == ui->toolDrag)
    {
        ui->preview->setMode(FrameTuneScene::MODE_NONE);
        ui->toolDrag->setChecked(true);
    }
    else if(s == ui->toolPencil)
    {
        ui->preview->setMode(FrameTuneScene::MODE_DRAW);
        ui->toolPencil->setChecked(true);
    }
    else if(s == ui->toolPicker)
    {
        ui->preview->setMode(FrameTuneScene::MODE_PICK_COLOR);
        ui->toolPicker->setChecked(true);
    }
    else if(s == ui->toolRubber)
    {
        ui->preview->setMode(FrameTuneScene::MODE_RUBBER);
        ui->toolRubber->setChecked(true);
    }
}

void ImageCalibrator::tempFrameUpdated(const QString &path)
{
    QPixmap f = QPixmap(path);
    if(f.isNull())
        return; // Invalid frame

    int cellWidth = m_sprite.width() / m_conf->matrixWidth;
    int cellHeight = m_sprite.height() / m_conf->matrixHeight;
    int cellX = (m_tempFileX * cellWidth);
    int cellY = (m_tempFileY * cellHeight);

    QPainter p(&m_sprite);
    p.save();
    p.setCompositionMode (QPainter::CompositionMode_Source);
    p.fillRect(cellX, cellY, cellWidth, cellHeight, Qt::transparent);
    p.restore();
    p.drawPixmap(cellX, cellY, f);
    p.end();

    m_spriteOrig = m_sprite;
    m_spriteModified = true;

    updateScene();
    m_matrix->updateScene(generateTarget());
}

void ImageCalibrator::frameEdited()
{
    auto f = ui->preview->getImage();

    int cellWidth = m_sprite.width() / m_conf->matrixWidth;
    int cellHeight = m_sprite.height() / m_conf->matrixHeight;
    auto &c = m_imgOffsets[m_frmX][m_frmY];
    int cellX = (m_frmX * cellWidth) + c.offsetX;
    int cellY = (m_frmY * cellHeight) + c.offsetY;

    QPainter p(&m_sprite);
    p.save();
    p.setCompositionMode (QPainter::CompositionMode_Source);
    p.fillRect(cellX, cellY, cellWidth, cellHeight, Qt::transparent);
    p.restore();
    p.drawPixmap(cellX, cellY, f);
    p.end();

    m_spriteOrig = m_sprite;
    m_spriteModified = true;
    m_matrix->updateScene(generateTarget());
}
