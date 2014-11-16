#ifndef SMARTIMPORTER_H
#define SMARTIMPORTER_H

#include <QObject>
#include "edit_level/level_edit.h"
#include "edit_world/world_edit.h"

///
/// \brief The SmartImporter is a class which imports custom graphics by dragging a folder into the level window.
///
class SmartImporter : public QObject
{
    Q_OBJECT
public:
    explicit SmartImporter(QObject *parent = 0);
    explicit SmartImporter(QWidget *parentWid, const QString& importPath, QWidget* targetImport, QObject *parent = 0);

    bool isValid();
    bool attemptFastImport();

    QString getImportPath() const;
    void setImportPath(const QString &value);

signals:

public slots:

private:

    leveledit* targetLevelWindow;
    WorldEdit* targetWorldWindow;
    QString importPath;
    QWidget *parentWid;
};

#endif // SMARTIMPORTER_H
