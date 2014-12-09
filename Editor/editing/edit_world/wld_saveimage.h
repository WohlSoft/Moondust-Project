#ifndef WLD_SAVEIMAGE_H
#define WLD_SAVEIMAGE_H

#include <QDialog>

namespace Ui {
class WldSaveImage;
}

class WldSaveImage : public QDialog
{
    Q_OBJECT

public:
    explicit WldSaveImage(QRect sourceRect, QSize targetSize, bool proportion, QWidget *parent = 0);
    QRect getRect;
    QSize imageSize;
    bool saveProportion;
    bool hidePaths;
    bool hideMusBoxes;

    ~WldSaveImage();

private slots:

    void on_imgHeight_valueChanged(int arg1);
    void on_imgWidth_valueChanged(int arg1);
    void on_SaveProportion_clicked(bool checked);
    void on_HidePaths_clicked(bool checked);
    void on_HideMusBoxes_clicked(bool checked);
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();


private:
    Ui::WldSaveImage *ui;
};

#endif // WLD_SAVEIMAGE_H
