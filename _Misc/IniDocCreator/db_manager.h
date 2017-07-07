#ifndef DB_MANAGER_H
#define DB_MANAGER_H

#include <QWidget>
#include <QtSql>

class DB_Manager : public QObject
{
    Q_OBJECT
    QSqlDatabase m_base;
public:
    DB_Manager();
    virtual ~DB_Manager();

    bool init(QWidget *parent = nullptr);
};

#endif // DB_MANAGER_H
