#include "db_manager.h"
#include <QMessageBox>
#include <QSqlError>
#include <QtDebug>

const QString table_inifiles =  "CREATE TABLE IF NOT EXISTS ini_files ("
                                "`filename_id` integer PRIMARY KEY NOT NULL, "
                                "`filename`       VARCHAR(255),"
                                "`file_required` integer DEFAULT 0, "
                                "`file_description`  VARCHAR(2048)"
                                ");";

const QString table_inisections =   "CREATE TABLE IF NOT EXISTS ini_sections ("
                                    "`section_id` integer PRIMARY KEY NOT NULL, "
                                    "`filename_id` integer NOT NULL, "
                                    "`section_required` integer DEFAULT 0, "
                                    "`section_name` VARCHAR(255), "
                                    "`section_description`  VARCHAR(2048)"
                                    ");";

const QString table_inifield_categories =   "CREATE TABLE IF NOT EXISTS ini_field_categories ("
                                            "`category_name` VARCHAR(255) PRIMARY KEY NOT NULL, "
                                            "`category_description` VARCHAR(1024)"
                                            ");";

const QString table_inifields = "CREATE TABLE IF NOT EXISTS ini_fields ("
                                "`field_id` integer PRIMARY KEY NOT NULL, "
                                "`section_id` integer NOT NULL, "
                                "`category_name` VARCHAR(255), "
                                "`required` integer DEFAULT 0, "
                                "`priority` integer DEFAULT 0, "
                                "`field_name` VARCHAR(255), "
                                "`field_type` VARCHAR(255), "
                                "`field_default` VARCHAR(1024), "
                                "`field_allowed_values` VARCHAR(2048), "
                                "`field_description`  VARCHAR(2048)"
                                ");";

DB_Manager::DB_Manager()
{}

DB_Manager::~DB_Manager()
{}

bool DB_Manager::init(QWidget *parent)
{
    m_base = QSqlDatabase::addDatabase("QSQLITE");
    qDebug() << QSqlDatabase::drivers();

    QString file = qApp->applicationFilePath() + "/ini_doc_cache.sqlite";
    m_base.setDatabaseName("ini_doc_cache.sqlite");
    if(!m_base.open())
    {
        QMessageBox::warning(parent,
                             tr("Failed to initialize cache"),
                             tr("Can't create or open cache file %1\n\nError: %2").arg(file).arg(m_base.lastError().text()));
        return false;
    }

    QSqlQuery a_query;
    bool res = true;

    res &= a_query.exec(table_inifiles);
    res &= a_query.exec(table_inisections);
    res &= a_query.exec(table_inifields);
    res &= a_query.exec(table_inifield_categories);

    if(!res)
    {
        QMessageBox::warning(parent,
                             tr("Failed to initialize cache"),
                             tr("Can't initialize cache\n\nError: %1").arg(m_base.lastError().text()));
        return false;
    }
    return true;
}
