#include "obj_custom_property.h"
#include "data_configs.h"

#include <QJsonDocument>
#include <QJsonObject>

#include <functional>


void dataconfigs::loadCustomProperties()
{
    main_custom_properties.clear();

    for(int i = 0; i < main_npc.size(); i++) {
        const obj_npc& npc = main_npc[i];
        const NpcSetup& npcSetup = npc.setup;

        QString jsonValue = npcSetup.custom_property;
        if(jsonValue.isEmpty())
            continue;

        // TODO: Check if file --> Import JSON file

        // Load JSON
        try { // TODO: Refactor and use https://github.com/WohlSoft/LunaLUA/tree/master/LunadllNewLauncher/SMBXLauncher/Utils/Json
            QJsonParseError err;
            QJsonDocument doc = QJsonDocument::fromJson(jsonValue.toLocal8Bit(), &err);

            auto formatAndThrow = [this, &npcSetup](const QString& jsonError) {
                throw QString("NPC %1 custom property json: %2").arg(npcSetup.id).arg(jsonError);
            };

            if(err.error != QJsonParseError::NoError)
                formatAndThrow(err.errorString());

            auto ensureObject = [i, &formatAndThrow](const QJsonObject& obj, const QString& name, bool optional) -> QJsonObject {
                QJsonValue possibleObj = obj[name];
                if(possibleObj.isUndefined() && optional)
                    return possibleObj.toObject();
                else if(possibleObj.isUndefined())
                    formatAndThrow(name + " does not exist");
                else if(!possibleObj.isObject())
                    formatAndThrow(name + " is not an object");

                return possibleObj.toObject();
            };

            auto visitObjectGroups = [&ensureObject](const QJsonObject& obj, const std::function<void(const QString&,const QJsonObject&)>& visitorFunc) {
                for(const QString& key : obj.keys()) {
                    visitorFunc(key, ensureObject(obj, key, false));
                }
            };


            QJsonObject docObj = doc.object();
            QJsonObject possibleEnumObject = ensureObject(docObj, "enum", true);
            QJsonObject customPropertiesObject = ensureObject(docObj, "properties", false);

            // Output variable
            obj_custom_property property;
            property.npc_id = npcSetup.id;

            // Convert Enum, if not undefined:
            if(!possibleEnumObject.isEmpty()) {
                visitObjectGroups(possibleEnumObject, [&, this](const QString& groupName, const QJsonObject& groupObj){
                    QMap<QString, custom_property_enum> enumElementsForGroup;
                    visitObjectGroups(groupObj, [&, this](const QString& enumName, const QJsonObject& enumObj){
                        custom_property_enum enumElem;
                        QJsonValue textVal = enumObj["text"];
                        if(textVal.isString())
                            formatAndThrow(QString("Enum element %1 is missing or not a string!").arg(enumName));

                        enumElem.text = textVal.toString();

                        // Insert
                        enumElementsForGroup[enumName] = enumElem;
                    });
                    property.enums[groupName] = enumElementsForGroup;
                });
            }

            // Add it!
            main_custom_properties.append(property);
        } catch (const QString& errTxt) {
            addError(errTxt);
        }
    }
}
