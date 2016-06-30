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

    void callFunction(QString functionName, QJSValueList &args);
    bool callBoolFunction(QString functionName, QJSValueList &args);

signals:

public slots:

private:
    QJSEngine m_jsengine;
    QJSValue result;
};

#endif // PGE_JSENGINE_H
