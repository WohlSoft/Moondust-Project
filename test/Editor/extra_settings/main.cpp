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

    QDialog dlg;

    QVBoxLayout *preLayout = new QVBoxLayout();
    preLayout->setMargin(0);
    dlg.setLayout(preLayout);

    QScrollArea *scrollWidget = new QScrollArea(&dlg);
    scrollWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollWidget->updateGeometry();

    JsonSettingsWidget *extraSettings = new JsonSettingsWidget(scrollWidget);
    scrollWidget->setWidgetResizable(true);

    preLayout->addWidget(scrollWidget);

    if(!extraSettings->loadLayoutFromFile(g_sample_settings, g_sample_layout))
    {
        QMessageBox::warning(nullptr, "oops", extraSettings->errorString());
        return 1;
    }

    QWidget *w = extraSettings->getWidget();
    scrollWidget->setWidget(w);

    dlg.resize(w->sizeHint().width() + 24, 800);

    QObject::connect(extraSettings, &JsonSettingsWidget::settingsChanged,
                            []()
    {
        qDebug() << "Setup Changed!";
    });

//    QScrollArea box;
//    box.setWidget(w);
    dlg.show();

    int ret = app.exec();

    extraSettings->saveSettingsIntoFile(g_sample_settings);

    return ret;
}
