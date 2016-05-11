#ifndef EPISODE_CONVERTER_H
#define EPISODE_CONVERTER_H

#include <QDialog>

namespace Ui {
class EpisodeConverter;
}

class EpisodeConverter : public QDialog
{
    Q_OBJECT

public:
    explicit EpisodeConverter(QWidget *parent = 0);
    ~EpisodeConverter();

private slots:
    void on_closeBox_clicked();

    void on_pushButton_clicked();

    void on_DoMadJob_clicked();

private:
    Ui::EpisodeConverter *ui;
};

#endif // EPISODE_CONVERTER_H
