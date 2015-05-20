#ifndef ASYNCSTARCOUNTER_H
#define ASYNCSTARCOUNTER_H

#include <QObject>
#include <QMutex>
#include <QMutexLocker>
#include <QProgressDialog>
#include <QtConcurrent>
#include <data_configs/data_configs.h>
#include <PGE_File_Formats/file_formats.h>




class AsyncStarCounter : public QObject
{
    Q_OBJECT
public:
    explicit AsyncStarCounter(const QString& dir, const QVector<WorldLevels>& levels, const QString& introLevel, dataconfigs* configs, QObject *parent = 0);

    void startAndShowProgress();

    bool isCancelled() const;
    int countedStars() const;

private:

    int checkNextLevel(QString FilePath);

    //Locked functions
    //void addToCount(int value);

    bool alreadyContainsInDoneFiles(const QString& path);
    void addToDoneFiles(const QString& path);

    void addToDoFiles(const QString& path);

    //Input
    QString m_dir;
    QVector<WorldLevels> m_levels;
    QString m_intro;
    dataconfigs* m_configs;

    //Check data
    QStringList m_doneFiles;
    QStringList m_toDoFiles;

    //Output
    int m_count;
    bool m_wasCancelled;

    //Locker
    QMutex m_mutex;
};

#endif // ASYNCSTARCOUNTER_H
