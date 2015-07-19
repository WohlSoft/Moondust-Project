#ifndef HTTPDOWNLOADER_H
#define HTTPDOWNLOADER_H

#include <QObject>

class HttpDownloader : public QObject
{
    Q_OBJECT
public:
    explicit HttpDownloader(QObject *parent = 0);

signals:

public slots:
};

#endif // HTTPDOWNLOADER_H
