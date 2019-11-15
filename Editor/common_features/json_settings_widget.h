#ifndef JSON_SETTINGS_WIDGET_H
#define JSON_SETTINGS_WIDGET_H

#include <QObject>
#include <QStack>
#include <QtPropertyBrowser/QtProperty>
#include <QJsonDocument>

class QtVariantPropertyManager;

class JsonSettingsWidget : public QObject
{
    Q_OBJECT

    struct SetupStack
    {
        static QJsonObject rectToArray(QVariant r);
        static QJsonObject rectfToArray(QVariant r);
        static QJsonObject sizeToArray(QVariant r);
        static QJsonObject sizefToArray(QVariant r);
        static QJsonObject pointToArray(QVariant r);
        static QJsonObject pointfToArray(QVariant r);

        QStack<QString> m_setupTree;
        QJsonDocument m_setupCache;

        SetupStack() = default;

        QString getPropertyId(const QString &name);

        bool loadSetup(const QByteArray &layoutJson, QString &errorString);
        QByteArray saveSetup();

        void clear();
        void setValue(const QString &propertyId, QVariant value);
    };

public:
    explicit JsonSettingsWidget(QWidget *parent = nullptr);
    explicit JsonSettingsWidget(const QByteArray &layout, QWidget *parent = nullptr);
    virtual ~JsonSettingsWidget();

    bool loadSettingsFromFile(const QString &path);
    bool saveSettingsIntoFile(const QString &path);

    bool loadSettings(const QByteArray &rawData);
    bool loadSettings(const QString &rawData);
    bool loadSettings(const QJsonDocument &rawData);
    QString saveSettings();
    QJsonDocument getSettings();

    bool loadLayout(const QByteArray &layout);
    bool loadLayout(const QByteArray &settings, const QByteArray &layout);
    bool loadLayout(const QJsonDocument &settings, const QByteArray &layout);
    bool loadLayoutFromFile(const QString &settings_path, const QString &layout_path);

    bool isValid();

    QWidget *getWidget();

    QString errorString();

signals:
    void settingsChanged();

private:
    QString     m_errorString;
    SetupStack  m_setupStack;
    QtAbstractPropertyBrowser *m_browser = nullptr;

    QVariant retrieve_property(const SetupStack &setupTree, QString prop, const QVariant &defaultValue);

    bool entryHasType(const QString &type);
    void loadLayoutEntries(SetupStack setupTree,
                      const QJsonArray &elements,
                      QtVariantPropertyManager *manager,
                      QtProperty *target,
                      QString &err,
                      QWidget *parent = nullptr);

    QtAbstractPropertyBrowser *loadLayoutDetail(SetupStack &stack,
                                                const QByteArray &layoutJson,
                                                QString &err);

};

#endif // JSON_SETTINGS_WIDGET_H
