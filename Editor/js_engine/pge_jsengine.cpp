#include "pge_jsengine.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QMessageBox>
#include <common_features/main_window_ptr.h>

#include <common_features/logger.h>

void PGE_JsEngine::logError(const QJSValue &erroredValue)
{
    QString message =
        "Error of the script " + m_scriptFile + ": \n"
        + "Uncaught exception at line "
        + QString::number(erroredValue.property("lineNumber").toInt())
        + ":\n" + erroredValue.toString();
    LogWarning(message);
}

PGE_JsEngine::PGE_JsEngine(QObject *parent) :
    QObject(parent),
    m_jsengine(this)
{
#if ((QT_VERSION_MAJOR == 5 && QT_VERSION_MINOR >= 6)||(QT_VERSION_MAJOR > 5))
    m_jsengine.installExtensions(QJSEngine::GarbageCollectionExtension);
#endif

    QFile bootFile(":/plugin/plugin/main_boot.js");
    bool openResult = bootFile.open(QIODevice::ReadOnly | QIODevice::Text);
    assert(openResult); // This file must open!
    Q_UNUSED(openResult)

    QTextStream bootFileStream(&bootFile);
    bootFileStream.setCodec("UTF-8");

    int numOfNewlines = 0;
    QString bootFileContents;
    while(!bootFileStream.atEnd())
    {
        numOfNewlines++;
        bootFileContents += bootFileStream.readLine() + "\n";
    }

    m_bootContents = qMakePair(std::move(numOfNewlines), std::move(bootFileContents));
}

bool PGE_JsEngine::setFile(QString filePath)
{
    QFile file(filePath);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
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


int PGE_JsEngine::getLastErrorLine() const
{
    return m_lastError.property("lineNumber").toInt();
}

QString PGE_JsEngine::getLastError() const
{
    return m_lastError.toString();
}

bool PGE_JsEngine::checkForErrors(const QJSValue &possibleErrVal, bool *ok)
{
    if(possibleErrVal.isError())
    {
        m_lastError = possibleErrVal;
        logError(possibleErrVal);
        if(ok)
            *ok = false;
        return false;
    }
    m_lastError = QJSValue();
    if(ok)
        *ok = true;
    return true;
}
