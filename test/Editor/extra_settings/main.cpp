#include "extra_settings_box.h"

#include <common_features/json_settings_widget.h>

#include <QApplication>
#include <QFile>

#include <QMessageBox>
#include <QtDebug>
#include <QScrollArea>
#include <QVBoxLayout>

static const char *g_sample_layout = "sample.json";
static const char *g_sample_settings = "setup.json";

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setStyle("Windows");

    JsonSettingsWidget extraSettings;

    if(!extraSettings.loadLayoutFromFile(g_sample_settings, g_sample_layout))
    {
        QMessageBox::warning(nullptr, "oops", extraSettings.errorString());
        return 1;
    }

    extraSettings.connect(&extraSettings, &JsonSettingsWidget::settingsChanged,
                            []()
    {
        qDebug() << "Setup Changed!";
    });

    QWidget *w = extraSettings.getWidget();
//    QScrollArea box;
//    box.resize(w->sizeHint().width() + 24, 800);
//    box.setWidget(w);
    w->show();

    int ret = app.exec();

    extraSettings.saveSettingsIntoFile(g_sample_settings);

    return ret;
}
