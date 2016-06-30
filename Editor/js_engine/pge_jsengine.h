#ifndef PGE_JSENGINE_H
#define PGE_JSENGINE_H

#include <QJSEngine>
#include <type_traits>

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

    template<typename T>
    void bindProxy(T* obj, const QString& regName){
        static_assert(std::is_base_of<QObject, T>::value, "obj must be base of QObject!");

        m_jsengine.globalObject().setProperty(regName, m_jsengine.newQObject(obj));
    }

    template<typename RetVal, typename... Args>
    RetVal call(QString functionName, Args&&... args){
        static_assert(std::is_default_constructible<RetVal>::value, "RetVal must be constructable without any args!");

        QJSValue function = m_jsengine.evaluate(functionName);
        if(!function.isError())
        {
            QJSValue result = function.call(QJSValueList({std::forward<Args>(args)...}));
            if(result.isError()){
                logError( result );
                return RetVal();
            }
            return qjsvalue_cast<RetVal>(result);
        } else {
            logError( function );
            return RetVal();
        }
    }

    template<typename... Args>
    void call(QString functionName, Args&&... args){
        QJSValue function = m_jsengine.evaluate(functionName);
        if(!function.isError())
        {
            QJSValue result = function.call(QJSValueList({std::forward<Args>(args)...}));
            if(result.isError()){
                logError( result );
            }
        } else {
            logError( function );
        }
    }


private:
    QJSEngine m_jsengine;
    QJSValue result;
};

#endif // PGE_JSENGINE_H
