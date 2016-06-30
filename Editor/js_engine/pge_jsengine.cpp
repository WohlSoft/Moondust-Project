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
    m_jsengine.installExtensions(QJSEngine::GarbageCollectionExtension);
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


void PGE_JsEngine::callFunction(QString functionName, QJSValueList &args)
{
    QString error;
    QJSValue function = m_jsengine.evaluate(functionName, error);
    if(!function.isError())
    {
        QJSValue result = function.call(args);
        if(result.isError())
            logError( result );
    } else {
        logError( function );
    }
}

bool PGE_JsEngine::callBoolFunction(QString functionName, QJSValueList &args)
{
    QString error;
    QJSValue function = m_jsengine.evaluate(functionName, error);
    if(!function.isError())
    {
        QJSValue result = function.call(args);
        if(result.isError())
            logError( result );
        return result.toBool();
    } else {
        logError( function );
        return false;
    }
}
