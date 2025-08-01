#include <QFileInfo>
#include <QDir>
#include <QSettings>
#include <QDialog>
#include <QStatusBar>
#include <QFocusEvent>
#include <QColorDialog>

#include "image_calibrator.h"
#include <ui_image_calibrator.h>
#include "calibration_main.h"
#include "main/graphics.h"
// #include "main/mouse_scene.h"
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

    ui->preview->setBgChess(true);

    ui->preview->setAllowScroll(true);
    QObject::connect(ui->preview, &FrameTuneScene::delta,
                     this, [this](Qt::MouseButton button, int deltaX, int deltaY)->void
    {
        switch(button)
        {
        case Qt::LeftButton:
            if(ui->moveAllFrames->isChecked())
                updateAllOffsets(deltaX, deltaY);
            else
                updateOffsets(m_frmX, m_frmY, deltaX, deltaY);
            break;

        case Qt::RightButton:
            if(deltaX != 0)
                ui->CropW->setValue(ui->CropW->value() - deltaX);
            if(deltaY != 0)
                ui->CropH->setValue(ui->CropH->value() - deltaY);
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
        else if(!ui->preview->isToolBusy())
            frameEdited();
    });

    m_matrix = new Matrix(conf, m_mw, Matrix::LabelSpriteEditor, this);
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
    // ui->toolPencil->setShortcut(Qt::Key_N);
    // ui->toolPicker->setShortcut(Qt::Key_O);
    // ui->toolRubber->setShortcut(Qt::Key_R);
    // ui->toolSelect->setShortcut(Qt::Key_S);
    // ui->toolSelectCopy->setShortcut(Qt::Key_C);

    // ui->toolUndo->setShortcut(QKeySequence("Ctrl+Z"));
    // ui->toolRedo->setShortcut(QKeySequence("Ctrl+Y"));

    ui->toolDrag->setChecked(true);

    m_status = new QStatusBar(this);
    ui->calibratorLayout->addWidget(m_status, 4, 0, 1, 4);

    QObject::connect(ui->toolDrag, &QToolButton::clicked,
                     this, &ImageCalibrator::toolChanged);
    QObject::connect(ui->toolColour, &QToolButton::clicked,
                     this, &ImageCalibrator::pickColour);
    QObject::connect(ui->toolSelect, &QToolButton::clicked,
                     this, &ImageCalibrator::toolChanged);
    QObject::connect(ui->toolSelectCopy, &QToolButton::clicked,
                     this, &ImageCalibrator::toolChanged);
    QObject::connect(ui->toolPencil, &QToolButton::clicked,
                     this, &ImageCalibrator::toolChanged);
    QObject::connect(ui->toolPicker, &QToolButton::clicked,
                     this, &ImageCalibrator::toolChanged);
    QObject::connect(ui->toolRubber, &QToolButton::clicked,
                     this, &ImageCalibrator::toolChanged);
    QObject::connect(ui->toolLine, &QToolButton::clicked,
                     this, &ImageCalibrator::toolChanged);
    QObject::connect(ui->toolRects, &QToolButton::clicked,
                     this, &ImageCalibrator::toolChanged);

    QObject::connect(ui->toolUndo, &QToolButton::clicked,
                     this, &ImageCalibrator::historyUndo);
    QObject::connect(ui->toolRedo, &QToolButton::clicked,
                     this, &ImageCalibrator::historyRedo);

    // Tools actions
    QAction *mirrorSprite = m_toolActions.addAction(tr("Mirror SMBX sprite..."));
    QObject::connect(mirrorSprite, static_cast<void (QAction::*)(bool)>(&QAction::triggered),
                     this, &ImageCalibrator::toolMirrorSMBXFrames);
    QAction *eraseUnused = m_toolActions.addAction(tr("Erase image data of unused frames..."));
    QObject::connect(eraseUnused, static_cast<void (QAction::*)(bool)>(&QAction::triggered),
                     this, [this]()->void
    {
        int ret = QMessageBox::question(this,
                                        tr("Erase unused frame images"),
                                        tr("Are you sure you want to erase image data of all unused frames?"),
                                        QMessageBox::Yes|QMessageBox::No);

        if(ret == QMessageBox::Yes)
            ui->preview->runAction(FrameTuneScene::ACTION_ERASE_UNUSED);
    });

    QAction *flipH = m_toolActions.addAction(tr("Flip horizontally"));
    QObject::connect(flipH, static_cast<void (QAction::*)(bool)>(&QAction::triggered),
                     this, [this]()->void
    {
        ui->preview->runAction(FrameTuneScene::ACTION_HFLIP_CUR_FRAME);
    });

    QAction *flipV = m_toolActions.addAction(tr("Flip vertically"));
    QObject::connect(flipV, static_cast<void (QAction::*)(bool)>(&QAction::triggered),
                     this, [this]()->void
    {
        ui->preview->runAction(FrameTuneScene::ACTION_VFLIP_CUR_FRAME);
    });

    m_toolActions.addSeparator();

    QAction *clipCopy = m_toolActions.addAction(tr("Copy frame to clipboard"));
    QObject::connect(clipCopy, static_cast<void (QAction::*)(bool)>(&QAction::triggered),
                     this, [this]()->void
    {
        ui->preview->runAction(FrameTuneScene::ACTION_COPY_FRAME);
    });

    QAction *clipPaste = m_toolActions.addAction(tr("Paste frame from clipboard"));
    QObject::connect(clipPaste, static_cast<void (QAction::*)(bool)>(&QAction::triggered),
                     this, [this]()->void
    {
        ui->preview->runAction(FrameTuneScene::ACTION_PASTE_FRAME);
    });

    m_toolActions.addSeparator();

    QAction *clipCopy2x = m_toolActions.addAction(tr("Copy frame to clipboard and shrink 2x"));
    QObject::connect(clipCopy2x, static_cast<void (QAction::*)(bool)>(&QAction::triggered),
                     this, [this]()->void
    {
        ui->preview->runAction(FrameTuneScene::ACTION_COPY_FRAME_2X_SHRINK);
    });

    QAction *clipPaste2x = m_toolActions.addAction(tr("Paste frame from clipboard and grow 2x"));
    QObject::connect(clipPaste2x, static_cast<void (QAction::*)(bool)>(&QAction::triggered),
                     this, [this]()->void
    {
        ui->preview->runAction(FrameTuneScene::ACTION_PASTE_FRAME_2X_GROW);
    });

    m_toolActions.addSeparator();

    QAction *cleanFrame = m_toolActions.addAction(tr("Clean frame"));
    QObject::connect(cleanFrame, static_cast<void (QAction::*)(bool)>(&QAction::triggered),
                     this, [this]()->void
    {
        ui->preview->runAction(FrameTuneScene::ACTION_ERASE_FRAME);
    });

    QAction *fix2pix = m_toolActions.addAction(tr("Fix the 2pix format"));
    QObject::connect(fix2pix, static_cast<void (QAction::*)(bool)>(&QAction::triggered),
                     this, [this]()->void
    {
        ui->preview->runAction(FrameTuneScene::ACTION_FIX_2PIX);
    });


    ui->toolQuickActions->setMenu(&m_toolActions);

    QObject::connect(ui->preview, &FrameTuneScene::actionMirrorSMBX,
                     this, &ImageCalibrator::toolMirrorSMBXAction);
    QObject::connect(ui->preview, &FrameTuneScene::actionFramePasted,
                     this, &ImageCalibrator::frameEdited);
    QObject::connect(ui->preview, &FrameTuneScene::actionUnusedErased,
                     this, &ImageCalibrator::toolEraseUnused);
    QObject::connect(ui->preview, &FrameTuneScene::actionFrameFlipedH,
                     this, &ImageCalibrator::toolFlipFrameH);
    QObject::connect(ui->preview, &FrameTuneScene::actionFrameFlipedV,
                     this, &ImageCalibrator::toolFlipFrameV);
    QObject::connect(ui->preview, &FrameTuneScene::drawColourChanged,
                     this, &ImageCalibrator::refreshColours);
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
    m_imgOffsets.clear();
    m_imgOffsets.resize(m_conf->matrixWidth);
    m_history.clear();
    m_history.resize(m_conf->matrixWidth);

    for(int x = 0; x < m_conf->matrixWidth; x++)
    {
        m_imgOffsets[x].fill(xyCell, m_conf->matrixHeight);
        m_history[x].fill(FrameHistory(), m_conf->matrixHeight);
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

void ImageCalibrator::setFrame(int x, int y)
{
    m_lockUI = true;
    ui->FrameX->setValue(x);
    ui->FrameY->setValue(y);
    m_lockUI = false;
    m_matrix->setFrame(x, y);
    frameSelected(x, y);
}

void ImageCalibrator::setPreviewOffset(const QPoint &off)
{
    ui->preview->setOffset(off);
}

void ImageCalibrator::setPreviewZoom(double zoom)
{
    ui->preview->setZoom(zoom);
}

void ImageCalibrator::showSpriteMap()
{
    ui->Matrix->setChecked(true);
    on_Matrix_clicked(true);
}

void ImageCalibrator::closeEvent(QCloseEvent *)
{
    if(m_matrix)
        m_matrix->close();
}

void ImageCalibrator::focusInEvent(QFocusEvent *)
{
    tempFrameUpdatedProceed();
}

void ImageCalibrator::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Escape)
    {
        if(ui->preview->isToolBusy())
            ui->preview->cancelTool();
        return;
    }

    switch(e->key())
    {
    case Qt::Key_O:
        ui->toolPicker->click();
        break;

    case Qt::Key_N:
        ui->toolPencil->click();
        break;

    case Qt::Key_L:
        ui->toolLine->click();
        break;

    case Qt::Key_R:
        ui->toolRects->click();
        break;

    case Qt::Key_F:
        if(e->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier))
            ui->preview->runAction(FrameTuneScene::ACTION_VFLIP_CUR_FRAME);
        if(e->modifiers() == Qt::ControlModifier)
            ui->preview->runAction(FrameTuneScene::ACTION_HFLIP_CUR_FRAME);
        break;

    case Qt::Key_E:
        if(e->modifiers() == Qt::ControlModifier)
            ui->preview->runAction(FrameTuneScene::ACTION_ERASE_FRAME);
        break;

    case Qt::Key_C:
        if(e->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier))
            ui->preview->runAction(FrameTuneScene::ACTION_COPY_FRAME_2X_SHRINK);
        if(e->modifiers() == Qt::ControlModifier)
            ui->preview->runAction(FrameTuneScene::ACTION_COPY_FRAME);
        else
            ui->toolSelectCopy->click();
        break;

    case Qt::Key_V:
        if(e->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier))
            ui->preview->runAction(FrameTuneScene::ACTION_PASTE_FRAME_2X_GROW);
        if(e->modifiers() == Qt::ControlModifier)
            ui->preview->runAction(FrameTuneScene::ACTION_PASTE_FRAME);
        break;

    case Qt::Key_Z:
        if(e->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier))
            ui->toolRedo->click();
        if(e->modifiers() == Qt::ControlModifier)
            ui->toolUndo->click();
        break;

    case Qt::Key_Y:
        if(e->modifiers() == Qt::ControlModifier)
            ui->toolUndo->click();
        break;

    case Qt::Key_S:
        if(e->modifiers() == Qt::ControlModifier)
            ui->WritePNG->click();
        else
            ui->toolSelect->click();
        break;
    }

    QDialog::keyPressEvent(e);
}

void ImageCalibrator::keyReleaseEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Escape)
        return;

    QDialog::keyPressEvent(e);
}


void ImageCalibrator::on_FrameX_valueChanged(int arg1)
{
    if(m_lockUI) return;
    m_frmX = arg1;
    m_matrix->setFrame(m_frmX, m_frmY);
    updateControls();
    updateScene();
}

void ImageCalibrator::on_FrameY_valueChanged(int arg1)
{
    if(m_lockUI) return;

    m_frmY = arg1;
    m_matrix->setFrame(m_frmX, m_frmY);
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
    m_setupBuffer = m_conf->frames[{x, y}];
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

void ImageCalibrator::updateOffset(int x, int y, int offsetX, int offsetY)
{
    auto &o = m_imgOffsets[x][y];
    o.offsetX = offsetX;
    o.offsetY = offsetY;

    if(x == m_frmX && y == m_frmY)
    {
        ui->OffsetX->blockSignals(true);
        ui->OffsetX->setValue(o.offsetX);
        ui->OffsetX->blockSignals(false);
        ui->OffsetY->blockSignals(true);
        ui->OffsetY->setValue(o.offsetY);
        ui->OffsetY->blockSignals(false);
    }

    updateScene();
}

void ImageCalibrator::updateOffsets(int x, int y, int deltaX, int deltaY)
{
    auto &o = m_imgOffsets[x][y];
    o.offsetX -= deltaX;
    o.offsetY -= deltaY;

    if(x == m_frmX && y == m_frmY)
    {
        ui->OffsetX->blockSignals(true);
        ui->OffsetX->setValue(o.offsetX);
        ui->OffsetX->blockSignals(false);

        ui->OffsetY->blockSignals(true);
        ui->OffsetY->setValue(o.offsetY);
        ui->OffsetY->blockSignals(false);
    }

    if(ui->hitboxMove->isChecked())
    {
        auto &f = m_conf->frames[{x, y}];
        f.offsetX += deltaX;
        f.offsetY += deltaY;
        m_hitboxModified = true;
    }

    updateScene();
}

void ImageCalibrator::updateAllOffsets(int deltaX, int deltaY)
{
    for(int x = 0; x < m_conf->matrixWidth; ++x)
    {
        for(int y = 0; y < m_conf->matrixHeight; ++y)
        {
            auto &o = m_imgOffsets[x][y];
            o.offsetX -= deltaX;
            o.offsetY -= deltaY;

            if(x == m_frmX && y == m_frmY)
            {
                ui->OffsetX->blockSignals(true);
                ui->OffsetX->setValue(o.offsetX);
                ui->OffsetX->blockSignals(false);
                ui->OffsetY->blockSignals(true);
                ui->OffsetY->setValue(o.offsetY);
                ui->OffsetY->blockSignals(false);
            }

            if(ui->hitboxMove->isChecked())
            {
                auto &f = m_conf->frames[{x, y}];
                f.offsetX += deltaX;
                f.offsetY += deltaY;
                m_hitboxModified = true;
            }
        }
    }

    updateScene();
}

void ImageCalibrator::on_dublPixDrawMode_clicked(bool checked)
{
    ui->preview->set2pixDrawMode(checked);
}

void ImageCalibrator::toolMirrorSMBXFrames()
{
    if(m_conf->matrixWidth != 10 || m_conf->matrixHeight != 10 || m_conf->spriteWidth != 1000 || m_conf->spriteHeight != 1000)
    {
        QMessageBox::warning(this,
                             tr("Incompatible sprite"),
                             tr("Can't perform the SMBX sprite mirror to incompatible sprite (the width and/or height of matrix are not equal to 10 and sprite is not equal to 1000x1000)."));
        return;
    }

    QDialog resetSetup(this);
    QGridLayout *l = new QGridLayout(&resetSetup);

    resetSetup.setWindowTitle(tr("Mirror frames on the SMBX sprite...", "Mirror SMBX frames"));

    resetSetup.setLayout(l);
    QLabel *info = new QLabel(tr("Select the way how frames will be mirrored:", "Mirror SMBX frames"), &resetSetup);
    l->addWidget(info , 0, 0, 1, 3);

    QGroupBox *dir = new QGroupBox(tr("Copy direction"), &resetSetup);
    QHBoxLayout *dirL = new QHBoxLayout(dir);
    dir->setLayout(dirL);
    l->addWidget(dir, 1, 0, 1, 3);

    QRadioButton *dirLeft = new QRadioButton(tr("Left to right"), dir);
    dirL->addWidget(dirLeft);
    QRadioButton *dirRight = new QRadioButton(tr("Right to left"), dir);
    dirL->addWidget(dirRight);
    dirRight->setChecked(true);


    QPushButton *hit = new QPushButton(&resetSetup);
    hit->setText(tr("Confirm"));
    l->addWidget(hit, 7, 1, 1, 1);
    QObject::connect(hit, static_cast<void (QAbstractButton::*)(bool)>(&QAbstractButton::clicked),
                     &resetSetup, &QDialog::accept);

    int ret = resetSetup.exec();
    if(ret == QDialog::Accepted)
    {
        if(dirLeft->isChecked())
            ui->preview->runAction(FrameTuneScene::ACTION_SMBX64_LEFT_TO_RIGHT);
        else if(dirRight->isChecked())
            ui->preview->runAction(FrameTuneScene::ACTION_SMBX64_RIGHT_TO_LEFT);

        updateControls();
        updateScene();
    }
}

void ImageCalibrator::toolMirrorSMBXAction(int dir)
{
    if(dir > 1)
        dir = 1;
    else if(dir < -1)
        dir = -1;
    else if(dir == 0)
    {
        qWarning() << "Invalid direction value: " << dir;
        return;
    }

    if(m_conf->matrixWidth != 10 || m_conf->matrixHeight != 10 || m_conf->spriteWidth != 1000 || m_conf->spriteHeight != 1000)
    {
        QMessageBox::warning(this,
                             tr("Incompatible sprite"),
                             tr("Can't perform the SMBX sprite mirror to incompatible sprite (the width and/or height of matrix are not equal to 10 and sprite is not equal to 1000x1000)."));
        return;
    }

    for(int i = 0; i <= 49; ++i)
    {
        int src = i * dir;
        int dst = src * -1;
        int src_frm_x = static_cast<int>(std::floor((src + 49.0f) / 10.0f));
        int src_frm_y = (src + 49) % 10;
        int dst_frm_x = static_cast<int>(std::floor((dst + 49.0f) / 10.0f));
        int dst_frm_y = (dst + 49) % 10;
        QImage img;
        QRgb tmp_pix;
        int w = m_conf->cellWidth;
        int h = m_conf->cellHeight;
        int xl = 0, xr = w;
        int yt = 0, yb = h;
        int dst_x = dst_frm_x * w;
        int dst_y = dst_frm_y * h;

        auto &frmSrc = m_conf->frames[{src_frm_x, src_frm_y}];
        auto &frmDst = m_conf->frames[{dst_frm_x, dst_frm_y}];

        if(!frmSrc.used)
            continue; // Skip unused frames

        img = getFrame(src_frm_x, src_frm_y).toImage();

        // Find left side
        for(int x = 0; x < w; ++x)
        {
            bool found = false;
            for(int y = 0; y < h; ++y)
            {
                if(img.pixelColor(x, y).alpha() > 0)
                {
                    found = true;
                    xl = x;
                    break;
                }
            }

            if(found)
                break;
        }

        // Find right side
        for(int x = w - 1; x >= 0; --x)
        {
            bool found = false;
            for(int y = 0; y < h; ++y)
            {
                if(img.pixelColor(x, y).alpha() > 0)
                {
                    found = true;
                    xr = x + 1;
                    break;
                }
            }

            if(found)
                break;
        }

        // Find top side
        for(int y = 0; y < h; ++y)
        {
            bool found = false;
            for(int x = 0; x < w; ++x)
            {
                if(img.pixelColor(x, y).alpha() > 0)
                {
                    found = true;
                    yt = y;
                    break;
                }
            }

            if(found)
                break;
        }

        // find bottom side
        for(int y = h - 1; y >= 0; --y)
        {
            bool found = false;
            for(int x = 0; x < w; ++x)
            {
                if(img.pixelColor(x, y).alpha() > 0)
                {
                    found = true;
                    yb = y + 1;
                    break;
                }
            }

            if(found)
                break;
        }

        if(xl > xr || yt > yb)
            continue; // Seems an empty, nothing to copy

        // Flip the frame itself
        for(int exl = xl, exr = xr - 1; exl < exr; ++exl, --exr)
        {
            for(int y = yt; y < yb; ++y)
            {
                tmp_pix = img.pixel(exr, y);
                img.setPixel(exr, y, img.pixel(exl, y));
                img.setPixel(exl, y, tmp_pix);
            }
        }

        // Put updated frame to target
        QPainter dstPaint(&m_sprite);
        dstPaint.setCompositionMode(QPainter::CompositionMode_Source);
        dstPaint.drawImage(dst_x, dst_y, img);
        dstPaint.end();

        // Draw the same on original (otherwise it will just fail)
        QPainter dstPaintOrig(&m_spriteOrig);
        dstPaintOrig.setCompositionMode(QPainter::CompositionMode_Source);
        dstPaintOrig.drawImage(dst_x, dst_y, img);
        dstPaintOrig.end();

        // Copy and adjust properties
        frmDst.used = frmSrc.used;
        frmDst.isDuck = frmSrc.isDuck;
        frmDst.isMountRiding = frmSrc.isMountRiding;
        frmDst.isRightDir = !frmSrc.isRightDir;
        frmDst.showGrabItem = frmSrc.showGrabItem;
        frmDst.w = frmSrc.w;
        frmDst.h = frmSrc.h;
        frmDst.offsetY = frmSrc.offsetY;

        int relHitL = frmSrc.offsetX - xl;
        int relHitR = xr - (frmSrc.offsetX + m_conf->frameWidth);

        frmDst.offsetX = frmSrc.offsetX - relHitL + relHitR;
        m_hitboxModified = true;

        // Reset image offsets since they became overriden
        auto &o = m_imgOffsets[dst_frm_x][dst_frm_y];
        o.offsetX = 0;
        o.offsetY = 0;
    }

    m_matrix->updateScene(generateTarget());
    updateScene();
}

void ImageCalibrator::toolEraseUnused()
{
    const int w = m_conf->cellWidth;
    const int h = m_conf->cellHeight;

    for(int x = 0; x < m_conf->matrixWidth; ++x)
    {
        for(int y = 0; y < m_conf->matrixHeight; ++y)
        {
            Calibration::FramePos pos = {x, y};
            if(m_conf->frames.contains(pos))
            {
                auto &f = m_conf->frames[pos];
                if(f.used)
                    continue; // We need UNUSED
            }

            // Put updated frame to target
            QPainter dstPaint(&m_sprite);
            dstPaint.setCompositionMode(QPainter::CompositionMode_Source);
            dstPaint.fillRect(x * w, y * h, w, h, QColor(0, 0, 0, 0));
            dstPaint.end();

            // Draw the same on original (otherwise it will just fail)
            QPainter dstPaintOrig(&m_spriteOrig);
            dstPaintOrig.setCompositionMode(QPainter::CompositionMode_Source);
            dstPaintOrig.fillRect(x * w, y * h, w, h, QColor(0, 0, 0, 0));
            dstPaintOrig.end();

            // Reset image offsets since they became overriden
            auto &o = m_imgOffsets[x][y];
            o.offsetX = 0;
            o.offsetY = 0;
        }
    }

    m_matrix->updateScene(generateTarget());
    updateScene();
}

void ImageCalibrator::toolFlipFrameH()
{
    Calibration::FramePos pos = {m_frmX, m_frmY};
    auto &f = m_conf->frames[pos];

    m_setupBuffer = f;

    if(ui->hitboxMove->isChecked())
    {
        int relHitL = f.offsetX;
        int relHitR = m_conf->cellWidth - (f.offsetX + m_conf->frameWidth);
        f.offsetX += -relHitL + relHitR;
        ui->preview->setFrameSetup(f);
    }

    frameEdited();
}

void ImageCalibrator::toolFlipFrameV()
{
    Calibration::FramePos pos = {m_frmX, m_frmY};
    auto &f = m_conf->frames[pos];

    m_setupBuffer = f;

    if(ui->hitboxMove->isChecked())
    {
        int relHitT = f.offsetY;
        int relHitB = m_conf->cellHeight - (f.offsetY + m_conf->frameHeight);
        f.offsetY += -relHitT + relHitB;
        ui->preview->setFrameSetup(f);
    }

    frameEdited();
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
    m_status->showMessage("Calibrated sprite was saved in: " + m_pngPath + " - " + m_gifPath, 5000);
}

void ImageCalibrator::on_WritePNG_clicked()
{
    makeBackup();
    QPixmap target = generateTarget();
    target.save(m_pngPath, "PNG");
    saveCalibrates();
    m_status->showMessage("Calibrated sprite was saved in: " + m_pngPath, 5000);
}

void ImageCalibrator::on_WriteGIF_clicked()
{
    makeBackup();
    QImage target = generateTarget().toImage();
    Graphics::toMaskedGif(target, m_gifPath);
    saveCalibrates();
    m_status->showMessage("Calibrated sprite was saved in:\n" + m_gifPath, 5000);
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
    ui->preview->setImage(getCurrentFrame());
    ui->preview->setGlobalSetup(*m_conf);
    ui->preview->setFrameSetup(m_conf->frames[{m_frmX, m_frmY}]);
    auto &his = m_history[m_frmX][m_frmY];
    ui->toolUndo->setEnabled(his.canUndo());
    ui->toolRedo->setEnabled(his.canRedo());
    refreshColours();
}

void ImageCalibrator::saveCalibrates()
{
    // FIXME: Make grid size and frame size being dynamical, not fixed 100x100!!!
    QSettings conf(m_iniPath, QSettings::IniFormat);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    conf.setIniCodec("UTF-8");
#endif

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
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    conf.setIniCodec("UTF-8");
#endif

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
    QPixmap target(m_sprite.width(), m_sprite.height());
    target.fill(Qt::transparent);
    QPainter x(&target);

    int cellWidth = m_conf->cellWidth;
    int cellHeight = m_conf->cellHeight;

    for(int i = 0; i < m_conf->matrixWidth; i++)
    {
        for(int j = 0; j < m_conf->matrixHeight; j++)
            x.drawPixmap(i * cellWidth, j * cellHeight, cellWidth, cellHeight, getFrame(i, j));
    }

    x.end();
    return target;
}

QPixmap ImageCalibrator::getCurrentFrame()
{
    auto &s = m_imgOffsets[m_frmX][m_frmY];
    return getFrame(m_frmX,
                    m_frmY,
                    s.offsetX, s.offsetY,
                    static_cast<int>(s.w), static_cast<int>(s.h));
}

QPixmap ImageCalibrator::getFrame(int x, int y)
{
    auto &s = m_imgOffsets[x][y];
    return getFrame(x, y,
                    s.offsetX, s.offsetY,
                    static_cast<int>(s.w), static_cast<int>(s.h));
}

QPixmap ImageCalibrator::getFrame(int x, int y, int oX, int oY, int cW, int cH)
{
    int pX = 0, pY = 0, pXo = 0, pYo = 0;
    int X, Y;
    int cellWidth = m_conf->cellWidth;
    int cellHeight = m_conf->cellHeight;

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
    Q_ASSERT(m_mw);
    QString gfxEditor = m_mw->getGfxEditorPath(this);

    if(gfxEditor.isEmpty())
        return;

    auto of = m_watchingFrame.files();
    for(auto &f : of)
        m_watchingFrame.removePath(f);

    QString tempFramePath = QDir::tempPath() + "/calibrator-temp-frame.png";

    auto frm = getCurrentFrame();
    if(frm.save(tempFramePath, "PNG"))
    {
        m_tempFileX = m_frmX;
        m_tempFileY = m_frmY;
        QProcess::startDetached(gfxEditor, {tempFramePath});
        m_watchingFrame.addPath(tempFramePath);
    }
}

void ImageCalibrator::toolChanged(bool)
{
    auto *s = sender();

    ui->toolDrag->setChecked(false);
    ui->toolSelect->setChecked(false);
    ui->toolSelectCopy->setChecked(false);
    ui->toolPencil->setChecked(false);
    ui->toolPicker->setChecked(false);
    ui->toolRubber->setChecked(false);
    ui->toolLine->setChecked(false);
    ui->toolRects->setChecked(false);

    if(s == ui->toolDrag)
    {
        ui->preview->setMode(FrameTuneScene::MODE_NONE);
        ui->toolDrag->setChecked(true);
    }
    else if(s == ui->toolSelect)
    {
        ui->preview->setMode(FrameTuneScene::MODE_SELECT);
        ui->toolSelect->setChecked(true);
    }
    else if(s == ui->toolSelectCopy)
    {
        ui->preview->setMode(FrameTuneScene::MODE_SELECT_COPY);
        ui->toolSelectCopy->setChecked(true);
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
    else if(s == ui->toolLine)
    {
        ui->preview->setMode(FrameTuneScene::MODE_LINE);
        ui->toolLine->setChecked(true);
    }
    else if(s == ui->toolRects)
    {
        ui->preview->setMode(FrameTuneScene::MODE_RECT);
        ui->toolRects->setChecked(true);
    }
}

void ImageCalibrator::pickColour()
{
    QColorDialog d(this);
    d.setOptions(QColorDialog::DontUseNativeDialog);

    int idx = 0;
    for(auto &c : m_recent_colours)
        d.setCustomColor(idx, c);

    d.setCurrentColor(ui->preview->getDrawColour());

    if(d.exec() == QDialog::Accepted)
    {
        int num = d.customCount();
        m_recent_colours.clear();

        for(int i = 0; i < num; ++i)
            m_recent_colours.insert(d.customColor(i));

        ui->preview->setDrawColour(d.currentColor());
    }
}

void ImageCalibrator::refreshColours()
{
    if(m_imgOffsets.empty())
        return; // Otherwise will fail

    m_frame_colours.clear();

    QImage f = getCurrentFrame().toImage();

    for(int y = 0; y < f.height(); ++y)
    {
        for(int x = 0; x < f.width(); ++x)
        {
            QColor pix = f.pixelColor(x, y);

            if(!m_frame_colours.contains(pix))
                m_frame_colours.insert(pix);
        }
    }

    QImage c = QImage(16, 16, QImage::Format_ARGB32);
    c.fill(ui->preview->getDrawColour());
    ui->toolColour->setIcon(QIcon(QPixmap::fromImage(c)));
}

void ImageCalibrator::tempFrameUpdated(const QString &path)
{
    if(!m_pendingFileToUpdate.isEmpty())
        return;

    m_pendingFileToUpdate = path;
    QTimer::singleShot(500, this, &ImageCalibrator::tempFrameUpdatedProceed);
}

void ImageCalibrator::tempFrameUpdatedProceed()
{
    if(m_pendingFileToUpdate.isEmpty())
        return;

    QPixmap f = QPixmap(m_pendingFileToUpdate);
    m_pendingFileToUpdate.clear();

    if(f.isNull())
        return; // Invalid frame

    auto &his = m_history[m_tempFileX][m_tempFileY];
    auto &setup = m_conf->frames[{m_frmX, m_frmY}];

    // Preserve first frame
    if(his.history.empty())
        his.addHistory(getFrame(m_tempFileX, m_tempFileY), m_setupBuffer);

    updateFrame(m_tempFileX, m_tempFileY, f);

    his.addHistory(getFrame(m_tempFileX, m_tempFileY), setup);

    m_setupBuffer = setup;

    m_spriteOrig = m_sprite;
    m_spriteModified = true;
    m_matrix->updateScene(generateTarget());
    updateScene();
}

void ImageCalibrator::frameEdited()
{
    auto f = ui->preview->getImage();
    auto &his = m_history[m_frmX][m_frmY];
    auto &setup = m_conf->frames[{m_frmX, m_frmY}];

    // Preserve first frame
    if(his.history.empty())
        his.addHistory(getCurrentFrame(), m_setupBuffer);

    updateCurrentFrame(f);

    his.addHistory(getCurrentFrame(), setup);

    m_setupBuffer = setup;

    m_spriteOrig = m_sprite;
    m_spriteModified = true;
    m_matrix->updateScene(generateTarget());

    ui->toolUndo->setEnabled(his.canUndo());
    ui->toolRedo->setEnabled(his.canRedo());
}

void ImageCalibrator::updateCurrentFrame(const QPixmap &f)
{
    updateFrame(m_frmX, m_frmY, f);
}

void ImageCalibrator::updateFrame(int x, int y, const QPixmap &f)
{
    auto &c = m_imgOffsets[x][y];
    int cellX = (x * m_conf->cellWidth) + c.offsetX;
    int cellY = (y * m_conf->cellHeight) + c.offsetY;

    QPainter p(&m_sprite);
    p.save();
    p.setCompositionMode (QPainter::CompositionMode_Source);
    p.fillRect(cellX, cellY, m_conf->cellWidth, m_conf->cellHeight, Qt::transparent);
    p.restore();
    p.drawPixmap(cellX, cellY, f);
    p.end();
}

void ImageCalibrator::historyUndo(bool)
{
    auto &his = m_history[m_frmX][m_frmY];
    auto &setup = m_conf->frames[{m_frmX, m_frmY}];
    if(!his.canUndo())
        return;

    if(ui->preview->isToolBusy())
    {
        ui->preview->cancelTool();
        return;
    }

    updateCurrentFrame(his.undo(setup));
    updateScene();
    m_matrix->updateScene(generateTarget());
    ui->toolUndo->setEnabled(his.canUndo());
    ui->toolRedo->setEnabled(his.canRedo());
}

void ImageCalibrator::historyRedo(bool)
{
    auto &his = m_history[m_frmX][m_frmY];
    auto &setup = m_conf->frames[{m_frmX, m_frmY}];
    if(!his.canRedo())
        return;

    if(ui->preview->isToolBusy())
        return; // Can't redo while current tool is busy

    updateCurrentFrame(his.redo(setup));
    updateScene();
    m_matrix->updateScene(generateTarget());
    ui->toolUndo->setEnabled(his.canUndo());
    ui->toolRedo->setEnabled(his.canRedo());
}
