#ifndef JS_UTILS_H
#define JS_UTILS_H

#include <QWidget>

inline QWidget* getWidgetParent(QObject* obj){
    QObject* parent = obj->parent();
    if(parent)
        return qobject_cast<QWidget*>(parent);
    return nullptr;
}

#endif // JS_UTILS_H
