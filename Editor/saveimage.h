#ifndef SAVEIMAGE_H
#define SAVEIMAGE_H

#include <QDialog>

namespace Ui {
class ExportToImage;
}

class ExportToImage : public QDialog
{
    Q_OBJECT
    
public:
    explicit ExportToImage(QWidget *parent = 0);
    ~ExportToImage();
    
private:
    Ui::ExportToImage *ui;
};

#endif // SAVEIMAGE_H
