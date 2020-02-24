#ifndef EXTRASETTINGSBOX_H
#define EXTRASETTINGSBOX_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class ExtraSettingsBox; }
QT_END_NAMESPACE

class ExtraSettingsBox : public QDialog
{
    Q_OBJECT

public:
    ExtraSettingsBox(QWidget *parent = nullptr);
    ~ExtraSettingsBox();

private:
    Ui::ExtraSettingsBox *ui;
};
#endif // EXTRASETTINGSBOX_H
