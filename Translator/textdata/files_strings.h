#ifndef FILESSTRINGS_H
#define FILESSTRINGS_H

#include <QAbstractTableModel>

#include "files_list_model.h"
#include "text_types.h"


class FilesStringsModel : public QAbstractTableModel
{
    Q_OBJECT

    TranslateProject *m_project = nullptr;
    TranslationData *m_language = nullptr;
    TranslationData_Level *m_level = nullptr;
    TranslationData_World *m_world = nullptr;
    TranslationData_Script *m_script = nullptr;

    struct TrView
    {
        int source;
        int state;
        int type;
        int key;
        QString title;
        QString note;
        QString tr_note;
        QString root;
    };

    typedef QVector<TrView> TrViewList;
    TrViewList m_view;

public:
    explicit FilesStringsModel(TranslateProject *project, QObject *parent = nullptr);

    enum Columns
    {
        C_TYPE = 0,
        C_STATE,
        C_TITLE,
        C_NOTE,
        C_COUNT
    };

    enum Roles
    {
        R_NOTE = Qt::UserRole,
        R_ROOT,
        R_GROUP,
        R_TYPE,
        R_KEY,
    };

    void setData(const QString &lang, int s, const QString &key);
    void clear();

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
};

#endif // FILESSTRINGS_H
