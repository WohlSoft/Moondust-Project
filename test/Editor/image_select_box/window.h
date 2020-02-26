#ifndef IMAGESELECTTEST_H
#define IMAGESELECTTEST_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class ImageSelectTest; }
QT_END_NAMESPACE

class ImageSelectTest : public QMainWindow
{
    Q_OBJECT

public:
    ImageSelectTest(QWidget *parent = nullptr);
    ~ImageSelectTest();

private:
    Ui::ImageSelectTest *ui;
};
#endif // IMAGESELECTTEST_H
