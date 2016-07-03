#ifndef PGE_EDITORPLUGINITEM_H
#define PGE_EDITORPLUGINITEM_H

#include <QObject>
#include <QString>
#include <QJSValue>

class PGE_EditorPluginManager;

class PGE_EditorPluginItem : public QObject
{
    Q_OBJECT
    friend class PGE_EditorPluginManager;
protected:
    PGE_EditorPluginItem(PGE_EditorPluginManager* manager, const QString& packageName, const QString& errorMessage);
    PGE_EditorPluginItem(PGE_EditorPluginManager* manager, const QString& packageName, const QString& pluginName, const QString& authorName, const QString& description, int pluginVersionNumber);
public:
    PGE_EditorPluginItem(const PGE_EditorPluginItem& other) = delete;

    QString pluginName() const;
    QString authorName() const;
    int pluginVersionNum() const;
    QString description() const;
    QString packageName() const;
    bool isValid() const;
    QString errorMessage() const;


protected:
    void setPackageName(const QString &packageName);
    void setIsValid(bool isValid);
    void setErrorMessage(const QString &errorMessage);

private:
    PGE_EditorPluginManager* m_pluginManager;
    QString m_packageName;
    QString m_pluginName;
    QString m_authorName;
    QString m_description;
    int m_pluginVersionNum;
    bool m_isValid;
    QString m_errorMessage;
};

#endif // PGE_EDITORPLUGINITEM_H
