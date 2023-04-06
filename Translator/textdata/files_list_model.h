#ifndef FILESLISTMODEL_H
#define FILESLISTMODEL_H

#include <QAbstractItemModel>
#include <QVector>
#include <QIcon>
#include "textdata.h"


class FilesListModel : public QAbstractItemModel
{
    Q_OBJECT

    TranslateProject *m_project = nullptr;
    struct TrView
    {
        int type;
        QString key;
        QString title;
        QString dir;
        QString path;
        QVector<TrView> children;
        QVector<TrView> *p;
        int parent;
        int row;
    };

    typedef QVector<TrView> TrViewList;
    TrViewList m_view;
    QString m_path;
    QString m_title;
    QIcon m_rootIcon;

    static void buildRelationsRekurs(TrView &it, TrViewList *parent, int parentIndex);
    void addChild(TrView &it);
    void buildRelations();

public:
    explicit FilesListModel(TranslateProject *project, QObject *parent = nullptr);

    enum Type
    {
        T_WORLD = 0,
        T_LEVEL,
        T_SCRIPT,
        T_DIR
    };

    enum Roles
    {
        R_TYPE = Qt::UserRole,
        R_KEY
    };

    void rebuildView(const QString &path);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
};

#endif // FILESLISTMODEL_H
