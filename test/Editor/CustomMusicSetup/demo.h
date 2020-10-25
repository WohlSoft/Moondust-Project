#ifndef DEMO_H
#define DEMO_H

#include <QMainWindow>
#include "../../../Editor/main_window/dock/custom_music_setup.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Demo; }
QT_END_NAMESPACE

class Demo : public QMainWindow
{
    Q_OBJECT

    CustomMusicSetup m_musSetup;

public:
    Demo(QWidget *parent = nullptr);
    ~Demo();

private slots:
    void on_musSetup_clicked();
    void on_music_editingFinished();

    void musicChanged(const QString &music);

private:
    Ui::Demo *ui;
};
#endif // DEMO_H
