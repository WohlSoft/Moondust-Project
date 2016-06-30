#ifndef PGE_JSENGINE_H
#define PGE_JSENGINE_H

#include <QJSEngine>
#include "proxies/_js_proxy_base.h"

class PGE_JsEngine : public QObject
{
    Q_OBJECT

    QString m_scriptContents;
    QString m_scriptFile;
    void logError(QJSValue &erroredValue);
public:
    explicit PGE_JsEngine(QObject *parent = 0);

    bool setFile(QString filePath);
    void setCode(QString &code);
    void bindProxy(PGE_JS_ProxyBase& px);
    void callFunction(QString functionName, QJSValueList &args);
    bool callBoolFunction(QString functionName, QJSValueList &args);

signals:

public slots:

private:
    QJSEngine m_jsengine;
    QJSValue result;
};

#endif // PGE_JSENGINE_H
