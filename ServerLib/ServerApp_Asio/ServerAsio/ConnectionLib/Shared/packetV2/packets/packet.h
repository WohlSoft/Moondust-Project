#ifndef PACKET_H
#define PACKET_H

#include <QMetaType>
#include "../../user/pgenet_user.h"

#include <QObject>
#include <QJsonDocument>

#include "../../util/threadedlogger.h"

class Packet : public QObject
{
    Q_OBJECT

    Q_DISABLE_COPY(Packet)
protected:
    Packet(QObject* parent = 0) : QObject(parent) {}
public:
    virtual ~Packet() {}

    void encode(QJsonObject& data)
    {
        const QMetaObject* obj = metaObject();
        QVariantMap dataMap = data.toVariantMap();

        for(int i = 0; i < obj->propertyCount(); ++i){
            QMetaProperty nextProperty = obj->property(i);
            dataMap[nextProperty.name()] = nextProperty.read(this);
        }

        data = QJsonObject::fromVariantMap(dataMap);
    }


    bool decode(QJsonObject& data)
    {
        const QMetaObject* obj = metaObject();
        QVariantMap dataMap = data.toVariantMap();

        for(int i = 0; i < obj->propertyCount(); ++i){
            QMetaProperty nextProperty = obj->property(i);
            if(QString(nextProperty.name()) == "objectName") continue;
            if(!dataMap.contains(nextProperty.name())){
                gThreadedLogger->logError(QString(nextProperty.name()) + " property for class " + obj->className() + " does not exist in JSON packet!");
                return false;
            }
            if(!nextProperty.write(this, dataMap[nextProperty.name()])){
                gThreadedLogger->logError(QString("Failed to write ") + QString(nextProperty.name()) + " property for class " + obj->className() + "! Wrong type?");
                return false;
            }
        }
        return true;
    }


    PGENET_User *getUser() const
    {
        return user;
    }
    void setUser(PGENET_User *value)
    {
        user = value;
    }

    int getSessionID() const
    {
        return sessionID;
    }
    void setSessionID(int value)
    {
        sessionID = value;
    }

    int getPacketID() const
    {
        return packetID;
    }
    void setPacketID(int value)
    {
        packetID = value;
    }

protected:
    PGENET_User* user;
    int sessionID;
    int packetID;
};


#endif // PACKET_H
