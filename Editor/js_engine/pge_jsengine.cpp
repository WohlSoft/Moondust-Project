#include "pge_jsengine.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QMessageBox>
#include <common_features/mainwinconnect.h>

#include <common_features/logger.h>

void PGE_JsEngine::logError(QJSValue &erroredValue)
{
    QString message =
                  "Error of the script " + m_scriptFile + ":"
                + "Uncaught exception at line"
                + QString::number(erroredValue.property("lineNumber").toInt())
                + ":" + erroredValue.toString();
    LogWarning(message);

    #if DEBUG_BUILD
    QMessageBox::critical(MainWinConnect::pMainWin, "JS Error", message);
    #endif
}

PGE_JsEngine::PGE_JsEngine(QObject *parent) : QObject(parent)
{
    #if ((QT_VERSION_MAJOR == 5 && QT_VERSION_MINOR >= 6)||(QT_VERSION_MAJOR > 5))
    m_jsengine.installExtensions(QJSEngine::GarbageCollectionExtension);
    #endif
}

bool PGE_JsEngine::setFile(QString filePath)
{
    QFile file(filePath);
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text))
        return false;

    QTextStream str(&file);
    str.setCodec("UTF-8");

    m_scriptContents    = str.readAll();
    m_scriptFile        = filePath;
    file.close();

    QJSValue result = m_jsengine.evaluate(m_scriptContents, m_scriptFile);
    if(result.isError())
    {
        logError(result);
        return false;
    }
    return true;
}

void PGE_JsEngine::setCode(QString &code)
{
    m_scriptFile = "";
    m_scriptContents = code;

    QJSValue result = m_jsengine.evaluate(m_scriptContents, m_scriptFile);
    if( result.isError() )
        logError(result);
}


