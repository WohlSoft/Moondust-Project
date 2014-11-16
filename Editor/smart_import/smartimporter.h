#ifndef SMARTIMPORTER_H
#define SMARTIMPORTER_H

#include <QObject>
#include "level_scene/lvl_scene.h"

///
/// \brief The SmartImporter is a class which imports custom graphics by dragging a folder into the level window.
///
class SmartImporter : public QObject
{
    Q_OBJECT
public:
    explicit SmartImporter(QObject *parent = 0);
    explicit SmartImporter(QString importPath, QObject *parent = 0);

    bool attemptFastImport();

    QString getImportPath() const;
    void setImportPath(const QString &value);

signals:

public slots:


private:
    QString importPath;
};

#endif // SMARTIMPORTER_H
