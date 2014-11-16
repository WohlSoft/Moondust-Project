#include "smartimporter.h"

SmartImporter::SmartImporter(QObject *parent) :
    QObject(parent)
{}

SmartImporter::SmartImporter(QString importPath, QObject *parent) :
    QObject(parent)
{
    this->importPath = importPath;
}

bool SmartImporter::attemptFastImport()
{
    return false;
}
QString SmartImporter::getImportPath() const
{
    return importPath;
}

void SmartImporter::setImportPath(const QString &value)
{
    importPath = value;
}

