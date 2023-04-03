#ifndef FILESLISTMODEL_H
#define FILESLISTMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include "textdata.h"


class FilesListModel : public QAbstractTableModel
{
    Q_OBJECT

    TranslateProject *m_project = nullptr;
    struct TrView
    {
        int type;
        QString title;
    };

    QVector<TrView> m_view;


public:
    explicit FilesListModel(TranslateProject *project, QObject *parent = nullptr);

    enum Type
    {
        T_WORLD = 0,
        T_LEVEL,
        T_SCRIPT
    };

    enum Roles
    {
        R_TYPE = Qt::UserRole
    };

    void rebuildView();

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
};

#endif // FILESLISTMODEL_H
