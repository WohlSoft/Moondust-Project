#include "pge_editorpluginitem.h"
#include "pge_editorpluginmanager.h"

PGE_EditorPluginItem::PGE_EditorPluginItem(PGE_EditorPluginManager *manager, const QString &packageName, const QString &errorMessage) :
    QObject(manager),
    m_pluginManager(manager),
    m_packageName(packageName),
    m_pluginName(""),
    m_authorName(""),
    m_description(""),
    m_pluginVersionNum(0),
    m_isValid(false),
    m_errorMessage(errorMessage)
{}

PGE_EditorPluginItem::PGE_EditorPluginItem(PGE_EditorPluginManager* manager, const QString& packageName, const QString &pluginName, const QString &authorName, const QString &description, int pluginVersionNumber) :
    QObject(manager),
    m_pluginManager(manager),
    m_packageName(packageName),
    m_pluginName(pluginName),
    m_authorName(authorName),
    m_description(description),
    m_pluginVersionNum(pluginVersionNumber),
    m_isValid(true),
    m_errorMessage("")
{}

QString PGE_EditorPluginItem::pluginName() const
{
    return m_pluginName;
}

QString PGE_EditorPluginItem::authorName() const
{
    return m_authorName;
}

int PGE_EditorPluginItem::pluginVersionNum() const
{
    return m_pluginVersionNum;
}

QString PGE_EditorPluginItem::description() const
{
    return m_description;
}

QString PGE_EditorPluginItem::packageName() const
{
    return m_packageName;
}

void PGE_EditorPluginItem::setPackageName(const QString &packageName)
{
    m_packageName = packageName;
}

bool PGE_EditorPluginItem::isValid() const
{
    return m_isValid;
}

void PGE_EditorPluginItem::setIsValid(bool isValid)
{
    m_isValid = isValid;
}

QString PGE_EditorPluginItem::errorMessage() const
{
    return m_errorMessage;
}

void PGE_EditorPluginItem::setErrorMessage(const QString &errorMessage)
{
    m_errorMessage = errorMessage;
}

