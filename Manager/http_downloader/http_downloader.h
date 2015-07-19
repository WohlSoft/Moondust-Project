#ifndef HTTPDOWNLOADER_H
#define HTTPDOWNLOADER_H

#include <QDialog>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>

QT_BEGIN_NAMESPACE
class QDialogButtonBox;
class QFile;
class QLabel;
class QLineEdit;
class QProgressDialog;
class QPushButton;
class QSslError;
class QAuthenticator;
QT_END_NAMESPACE

class HttpDownloader : public QObject
{
    Q_OBJECT
public:
    explicit HttpDownloader(QObject *parent = 0);
    void startRequest(QUrl url);
    bool isBusy();
    bool isFine();
    bool isAborted();

public slots:
    void downloadFile(QString urlfile, QString toDir);
    void cancelDownload();

private slots:
    void httpFinished();
    void httpFailed(QNetworkReply::NetworkError);
    void httpReadyRead();
    void updateDataReadProgress(qint64 bytesRead, qint64 totalBytes);
    void slotAuthenticationRequired(QNetworkReply*,QAuthenticator *);
#ifndef QT_NO_SSL
    void sslErrors(QNetworkReply*,const QList<QSslError> &errors);
#endif

signals:
    void canceled();
    void finished();
    void failed(QString error);
    void progress(qint64 bytesRead, qint64 totalBytes);

private:
//    QLabel *statusLabel;
//    QLabel *urlLabel;
//    QLineEdit *urlLineEdit;
//    QProgressDialog *progressDialog;
//    QPushButton *downloadButton;
//    QPushButton *quitButton;
//    QDialogButtonBox *buttonBox;

    QUrl url;
    QString targetFile;
    QNetworkAccessManager qnam;
    QNetworkReply *reply;
    QFile *file;
    int httpGetId;
    bool httpRequestAborted;
    bool _isBusy;
    bool _isFine;
};

#endif // HTTPDOWNLOADER_H
