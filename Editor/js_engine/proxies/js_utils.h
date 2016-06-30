#ifndef JS_UTILS_H
#define JS_UTILS_H

#include <QWidget>
#include <QJSValue>
#include <QJSEngine>
#include <cassert>
#include <utility>

inline QWidget* getWidgetParent(QObject* obj){
    QObject* parent = obj->parent();
    if(parent)
        return qobject_cast<QWidget*>(parent);
    return nullptr;
}

template<typename T, typename... Args>
inline QJSValue genQObjectWrapper(QObject* factoryObject, Args&&... args){
    QJSEngine* engine = qjsEngine(factoryObject);
    assert(engine);
    return engine->newQObject(new T(std::forward<Args>(args)...));
}

#endif // JS_UTILS_H
