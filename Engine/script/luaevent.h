#ifndef LUAEVENT_H
#define LUAEVENT_H


#include <QString>
#include <QVariantList>

#include <luabind/luabind.hpp>
#include <lua_inclues/lua.hpp>

class LuaEngine;

class LuaEvent{

    friend class LuaEngine;
private:

    QString m_eventName;
    LuaEngine* m_engine;

    lua_State* getNativeState();

protected:
    std::vector<luabind::object> objList;
public:
    LuaEvent(LuaEngine* engine);
    QString eventName() const;
    void setEventName(const QString &eventName);

    template<typename T>
    void addParameter(T par){
        objList.emplace_back(getNativeState(), par);
    }

};




#endif // LUAEVENT_H


