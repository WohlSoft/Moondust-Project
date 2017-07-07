#ifndef INISPEC_H
#define INISPEC_H

#include <QAbstractTableModel>
#include <QMap>

class DB_Manager;

class IniSpec : public QAbstractTableModel
{
    Q_OBJECT

    DB_Manager * m_db = nullptr;

    struct IniSection
    {
        bool    isRequired = true;
        QString name;
        QString description;
    };

    struct IniField
    {
        QString title;
        QString category;
        bool isRequired = true;
        QString type;
        QString allowed_values;
        QString default_value;
        QString description;
    };

    QMap<QString, IniSection>   m_sections;
    QString                     m_cur_section_name;
    IniSection*                 m_cur_section = nullptr;
    QVector<IniField>           m_current_fields;

    enum Columns
    {
        COL_FieldTitle = 0,
        COL_Category,
        COL_IsReqired,
        COL_Type,
        COL_PossibleValues,
        COL_DefaultValue,
        COL_Description
    };

public:
    explicit IniSpec(QObject *parent = nullptr, DB_Manager *db = nullptr);

    bool loadIniFile(QString filePath);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Editable:
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

private:
};

#endif // INISPEC_H
