#ifndef DIALOGUESLISTMODEL_H
#define DIALOGUESLISTMODEL_H

#include <QAbstractTableModel>
#include "textdata.h"

class DialoguesListModel : public QAbstractTableModel
{
    Q_OBJECT

    TranslateProject *m_project = nullptr;
    TranslationData_Level *m_level = nullptr;
    QString m_levelKey;
    QVector<TranslationData_Dialogue> *m_dialogues = nullptr;

    struct TrView
    {
        int i;
        QString note;
    };

    typedef QVector<TrView> TrViewList;
    TrViewList m_view;

public:
    explicit DialoguesListModel(TranslateProject *project, QObject *parent = nullptr);

    enum Columns
    {
        C_INDEX = 0,
        C_TITLE,
        C_COUNT
    };

    void setData(const QString &lang, const QString &level);
    void clear();

    TranslationData_Level &level() const;
    QString levelKey() const;

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Editable:
    bool setData(const QModelIndex &index,
                 const QVariant &value,
                 int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

private:
};

#endif // DIALOGUESLISTMODEL_H
