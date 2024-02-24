/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2024 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef JSON_SETTINGS_WIDGET_H
#define JSON_SETTINGS_WIDGET_H

#include <QObject>
#include <QStack>
#include <QWidget>
#include <QLabel>
#include <QJsonDocument>


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
        static QJsonObject rectToArray(const QVariant& r);
        static QJsonObject rectfToArray(const QVariant& r);
        static QJsonObject sizeToArray(const QVariant& r);
        static QJsonObject sizefToArray(const QVariant& r);
        static QJsonObject pointToArray(const QVariant& r);
        static QJsonObject pointfToArray(const QVariant& r);

        QStack<QString> m_setupTree;
        QJsonDocument m_setupCache;

        SetupStack() = default;

        QString getPropertyId(const QString &name);

        bool loadSetup(const QByteArray &layoutJson, QString &errorString);
        QByteArray saveSetup();

        void clear();
        void setValue(const QString &propertyId, const QVariant &value);
        void setValue(const QString &propertyId, const QJsonArray &value);
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
    void fileOpenRequested(const QString &file);

private:
    QString     m_errorString;
    SetupStack  m_setupStack;
    QWidget *m_browser = nullptr;
    bool m_spacerNeeded = false;

    QString m_directoryEpisode;
    QString m_directoryData;

    DataConfig *m_configPack = nullptr;

    QVariant retrieve_property(const SetupStack &setupTree, QString prop, const QVariant &defaultValue);
    QJsonArray retrieve_property(const SetupStack &setupTree, QString prop, const QJsonArray &defaultValue);

    enum LineEditType
    {
        JSS_LineEdit_Text = 0,
        JSS_LineEdit_Music,
        JSS_LineEdit_SFX,
        JSS_LineEdit_Level,
        JSS_LineEdit_File
    };

    QString browseForFileValue(QWidget *target, LineEditType type,
                               const QString &root, const QString& filePath,
                               const QStringList &filters,
                               const QString &dialogueTitle, const QString &dialogueDescription,
                               bool* ok = nullptr);

    bool entryHasType(const QString &type);
    void loadLayoutEntries(SetupStack setupTree,
                      const QJsonArray &elements,
                      QWidget *target,
                      QString &err,
                      QWidget *parent = nullptr);

    QWidget *loadLayoutDetail(SetupStack &stack,
                                                const QByteArray &layoutJson,
                                                QString &err);

    QString m_langFull;
    QString m_langShort;
    void valTrInitLang();

    QJsonValue valTr(const QJsonObject& v, const QString &key);

};

#endif // JSON_SETTINGS_WIDGET_H
