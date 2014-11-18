#include "smartimporter.h"

SmartImporter::SmartImporter(QObject *parent) :
    QObject(parent)
{}

SmartImporter::SmartImporter(QWidget *parentWid, const QString &importPath, QWidget *targetImport, QObject *parent) :
    QObject(parent)
{
    this->parentWid = parentWid;

    if(importPath.endsWith("/") || importPath.endsWith("\\"))
        this->importPath = importPath;
    else
        this->importPath = importPath + QString("/");

    targetLevelWindow = qobject_cast<leveledit*>(targetImport);
    targetWorldWindow = qobject_cast<WorldEdit*>(targetImport);
}

bool SmartImporter::isValid()
{
    return targetLevelWindow || targetWorldWindow;
}

bool SmartImporter::attemptFastImport()
{
    if(targetLevelWindow){
        if(targetLevelWindow->isUntitled){
            QMessageBox::warning(parentWid, tr("File not saved"), tr("You need to save the level, so you can import custom graphics!"), QMessageBox::Ok);
            return false;
        }
        CustomDirManager uLVL(targetLevelWindow->LvlData.path, targetLevelWindow->LvlData.filename);
        QDir sourceDir(importPath);
        if(!sourceDir.exists())
            return false;

        QStringList allFiles = sourceDir.entryList(QDir::Files | QDir::Readable, QDir::Name);
        QStringList filteredFiles;
        foreach (QString tarFile, allFiles) {
            if(tarFile.startsWith("block-") && tarFile.endsWith(".gif"))
                filteredFiles << importPath + tarFile;
            if(tarFile.startsWith("background-") && tarFile.endsWith(".gif"))
                filteredFiles << importPath + tarFile;
            if(tarFile.startsWith("background2-") && tarFile.endsWith(".gif"))
                filteredFiles << importPath + tarFile;
            if(tarFile.startsWith("npc-") && (tarFile.endsWith(".gif")||tarFile.endsWith(".txt")))
                filteredFiles << importPath + tarFile;
            if(tarFile.startsWith("effect-") && tarFile.endsWith(".gif"))
                filteredFiles << importPath + tarFile;
            if(tarFile.startsWith("yoshib-") && tarFile.endsWith(".gif"))
                filteredFiles << importPath + tarFile;
            if(tarFile.startsWith("yoshit-") && tarFile.endsWith(".gif"))
                filteredFiles << importPath + tarFile;
            if(tarFile.startsWith("mario-") && tarFile.endsWith(".gif"))
                filteredFiles << importPath + tarFile;
            if(tarFile.startsWith("luigi-") && tarFile.endsWith(".gif"))
                filteredFiles << importPath + tarFile;
            if(tarFile.startsWith("peach-") && tarFile.endsWith(".gif"))
                filteredFiles << importPath + tarFile;
            if(tarFile.startsWith("toad-") && tarFile.endsWith(".gif"))
                filteredFiles << importPath + tarFile;
            if(tarFile.startsWith("link-") && tarFile.endsWith(".gif"))
                filteredFiles << importPath + tarFile;
        }

        uLVL.createDirIfNotExsist();
        uLVL.import(filteredFiles);
        return true;
    }
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

