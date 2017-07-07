#include "ini_spec.h"
#include "db_manager.h"

IniSpec::IniSpec(QObject *parent, DB_Manager *db)
    : QAbstractTableModel(parent),
      m_db(db)
{
    m_current_fields.push_back({"test", "dummy", false, "string", "Any", "[blank]", "This is a string!"});
}

bool IniSpec::loadIniFile(QString filePath)
{
    //FIXME: Implement this!
    return false;
}

QVariant IniSpec::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole)
        return QVariant();

    if(orientation == Qt::Horizontal)
    {
        switch(section)
        {
        case COL_FieldTitle:
            return tr("Field name");
        case COL_Category:
            return tr("Category");
        case COL_IsReqired:
            return tr("Is Required");
        case COL_Type:
            return tr("Type");
        case COL_PossibleValues:
            return tr("Possible values");
        case COL_DefaultValue:
            return tr("Default values");
        case COL_Description:
            return tr("Description");
        }
    }
    return QVariant("");
}

int IniSpec::rowCount(const QModelIndex &) const
{
    return m_current_fields.size();
}

int IniSpec::columnCount(const QModelIndex &) const
{
    return 7;
}

QVariant IniSpec::data(const QModelIndex &index, int role) const
{
    if((index.row() < 0) || (index.row() >= m_current_fields.size()))
        QVariant();

    const IniField &f = m_current_fields[index.row()];

    if((role == Qt::DisplayRole) || (role == Qt::EditRole))
    {
        switch(index.column())
        {
        case COL_FieldTitle:
            return f.title;
        case COL_Category:
            return f.category;
        case COL_IsReqired:
            return tr("Required");
        case COL_Type:
            return f.type;
        case COL_PossibleValues:
            return f.allowed_values;
        case COL_DefaultValue:
            return f.default_value;
        case COL_Description:
            return f.description;
        }
    }
    else if(role == Qt::CheckStateRole)
    {
        if(index.column() == COL_IsReqired)
            return f.isRequired ? Qt::Checked : Qt::Unchecked;
        else
            return QVariant();
    }

    return QVariant();
}

bool IniSpec::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if((index.row() < 0) || (index.row() >= m_current_fields.size()))
        return false;

    if (data(index, role) != value)
    {
        IniField &f = m_current_fields[index.row()];
        switch(index.column())
        {
        case COL_FieldTitle:
            return false;//Can't set this!
        case COL_Category:
            f.category = value.toString();
            break;
        case COL_IsReqired:
            f.isRequired = (value.toInt() == Qt::Checked);
            break;
        case COL_Type:
            f.type = value.toString();
            break;
        case COL_PossibleValues:
            f.allowed_values = value.toString();
            break;
        case COL_DefaultValue:
            f.default_value = value.toString();
            break;
        case COL_Description:
            f.description = value.toString();
            break;
        }
        emit dataChanged(index, index, QVector<int>() << role);

        return true;
    }
    return false;
}

Qt::ItemFlags IniSpec::flags(const QModelIndex &index) const
{
    switch(index.column())
    {
    case COL_FieldTitle:
        return Qt::ItemIsEnabled|Qt::ItemIsSelectable;
    case COL_Category:
    case COL_Type:
    case COL_PossibleValues:
    case COL_DefaultValue:
    case COL_Description:
        return Qt::ItemIsEnabled|Qt::ItemIsSelectable|Qt::ItemIsEditable;
    case COL_IsReqired:
        return Qt::ItemIsEnabled|Qt::ItemIsSelectable|Qt::ItemIsUserCheckable;
    }

    return Qt::NoItemFlags;
}
