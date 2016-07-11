#ifndef PGE_JSENGINE_H
#define PGE_JSENGINE_H

#include <QJSEngine>
#include <type_traits>
#include <QFile>
#include <QTextStream>
#include <common_features/util.h>
#include <QPair>

#include <QDebug>

// This specialization allows us to use void for qjsvalue_cast
template<>
inline void qjsvalue_cast<void>(const QJSValue&){
    return;
}

class PGE_JsEngine : public QObject
{
    Q_OBJECT

    QPair<int, QString> m_bootContents;
    QString m_scriptContents;
    QString m_scriptFile;
    QJSValue m_lastError;
    QJSEngine m_jsengine;
public:
    explicit PGE_JsEngine(QObject *parent = 0);
    virtual ~PGE_JsEngine() {}

    bool setFile(QString filePath);

    template<typename T>
    T loadFileByExpcetedResult(const QString& filePath, bool* ok)
    {
        // Add booting-js file as resource to set local variables
        // i.e. filePath of script
        QFile file(filePath);
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
            if(ok)
                *ok = false;
            return T();
        }

        QTextStream str(&file);
        str.setCodec("UTF-8");

        // TODO: Replace boot placeholders
        QString m_scriptContents = m_bootContents.second + str.readAll();
        file.close();

        QJSValue result = m_jsengine.evaluate(m_scriptContents, m_scriptFile, -m_bootContents.first + 1);
        if(!checkForErrors(result, ok))
            return T();

        return qjsvalue_cast<T>(result);
    }



    inline void bindProxy(QObject* obj, const QString& regName){
        m_jsengine.globalObject().setProperty(regName, m_jsengine.newQObject(obj));
    }

    template<typename RetVal, typename... Args>
    RetVal call(const QString& functionName, bool* ok, Args&&... args){
        static_assert(std::is_default_constructible<RetVal>::value, "RetVal must be constructable without any args!");

        QJSValue function = m_jsengine.evaluate(functionName);
        if(!checkForErrors(function, ok))
            return RetVal();

        QJSValue result = function.call(QJSValueList({std::forward<Args>(args)...}));
        if(!checkForErrors(result, ok))
                return RetVal();

        return qjsvalue_cast<RetVal>(result);
    }

    int getLastErrorLine() const;
    QString getLastError() const;
private:
    bool checkForErrors(const QJSValue& possibleErrVal, bool* ok = nullptr);
    void logError(const QJSValue &erroredValue);
};

#endif // PGE_JSENGINE_H
