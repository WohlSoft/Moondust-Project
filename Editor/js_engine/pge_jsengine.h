#ifndef PGE_JSENGINE_H
#define PGE_JSENGINE_H

#include <QJSEngine>
#include <type_traits>
#include <QFile>
#include <QTextStream>

class PGE_JsEngine : public QObject
{
    Q_OBJECT

    QString m_scriptContents;
    QString m_scriptFile;
    QJSValue m_lastError;
    QJSEngine m_jsengine;
public:
    explicit PGE_JsEngine(QObject *parent = 0);
    virtual ~PGE_JsEngine() {}

    bool setFile(QString filePath);
    void setCode(QString &code);

    template<typename T>
    T loadFileByExpcetedResult(const QString& filePath, bool* ok)
    {

        QFile file(filePath);
        if(!file.open(QIODevice::ReadOnly|QIODevice::Text)){
            if(ok)
                *ok = false;
            return T();
        }

        QTextStream str(&file);
        str.setCodec("UTF-8");

        QString m_scriptContents = str.readAll();
        file.close();

        QJSValue result = m_jsengine.evaluate(m_scriptContents, m_scriptFile);
        if(!checkForErrors(result, ok))
            return T();

        return qjsvalue_cast<T>(result);
    }

    template<typename T>
    void bindProxy(T* obj, const QString& regName){
        static_assert(std::is_base_of<QObject, T>::value, "obj must be base of QObject!");

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

    template<typename... Args>
    void call(const QString& functionName, bool* ok, Args&&... args){
        QJSValue function = m_jsengine.evaluate(functionName);
        if(!checkForErrors(function, ok))
            return;

        QJSValue result = function.call(QJSValueList({std::forward<Args>(args)...}));
        if(!checkForErrors(result, ok))
            return;
    }

    QJSValue getLastError();
private:
    bool checkForErrors(const QJSValue& possibleErrVal, bool* ok = nullptr);
    void logError(const QJSValue &erroredValue);
};

#endif // PGE_JSENGINE_H
