#ifndef JSON_SETTINGS_WIDGET_H
#define JSON_SETTINGS_WIDGET_H

#include <QObject>
#include <QStack>
#include <QWidget>
#include <QLabel>
#include <QJsonDocument>
#include <QGroupBox>
#include <QPushButton>
#include <QJsonArray>

class ColorPreview : public QWidget
{
    Q_OBJECT

    QColor m_color;
public:
    explicit ColorPreview(QWidget *parent);
    ~ColorPreview() override;

    void setColor(QColor color);
    QColor color() const;

    virtual QSize sizeHint() const override;

protected:
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

signals:
    void sizeChanged(QSize newSize);
};

class DataConfig;
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
        void removeElement(const QString &propertyId);
    };

    class JsonListSettingsGroup {
    public:
        QGroupBox* groupBox;
        QJsonArray children;
        int size = 0;
        int maxSize = 0;
        QPushButton* addButton;
        QWidget* parent;
        QString err;
    };

public:
    explicit JsonSettingsWidget(QWidget *parent = nullptr);
    explicit JsonSettingsWidget(const QByteArray &layout, QWidget *parent = nullptr);
    virtual ~JsonSettingsWidget();

    void setSearchDirectories(const QString &episode, const QString &data);
    void setConfigPack(DataConfig *config);

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

    bool spacerNeeded();

    bool isValid();

    QWidget *getWidget();

    QString errorString();

signals:
    void settingsChanged();

private:
    QString     m_errorString;
    SetupStack  m_setupStack;
    QWidget *m_browser = nullptr;
    bool m_spacerNeeded = false;

    QString m_directoryEpisode;
    QString m_directoryData;

    QMap<QString, JsonListSettingsGroup> m_instantiatedQJsonLists;

    DataConfig *m_configPack = nullptr;

    QVariant retrieve_property(const SetupStack &setupTree, QString prop, const QVariant &defaultValue);

    void AddListElement(SetupStack setupTree, QString id);
    void RemoveListElement(SetupStack setupTree, QString id, QFrame* itemSubGroup);

    bool entryHasType(const QString &type);
    void loadLayoutEntries(SetupStack setupTree,
                      const QJsonArray &elements,
                      QWidget *target,
                      const QString &err,
                      QWidget *parent = nullptr);

    QWidget *loadLayoutDetail(SetupStack &stack,
                                                const QByteArray &layoutJson,
                                                QString &err);

};

#endif // JSON_SETTINGS_WIDGET_H
