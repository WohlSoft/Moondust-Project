#include "http_downloader.h"

#include <QMessageBox>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QtNetwork>
#include <QtWidgets>

#include <QtDebug>

HttpDownloader::HttpDownloader(QObject *parent) : QObject(parent)
{
    _isBusy=false;
    _isFine=false;

    connect(&qnam, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)),
            this, SLOT(slotAuthenticationRequired(QNetworkReply*,QAuthenticator*)));
    #ifndef QT_NO_SSL
        connect(&qnam, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)),
                    this, SLOT(sslErrors(QNetworkReply*,QList<QSslError>)));
    #endif
}


void HttpDownloader::startRequest(QUrl url)
{
    reply = qnam.get(QNetworkRequest(url));
    connect(reply, SIGNAL(finished()),
            this, SLOT(httpFinished()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(httpFailed(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(readyRead()),
            this, SLOT(httpReadyRead()));
    connect(reply, SIGNAL(downloadProgress(qint64,qint64)),
            this, SLOT(updateDataReadProgress(qint64,qint64)));
    qDebug()<<"Begin downloading";
}


void HttpDownloader::downloadFile(QString urlfile, QString toDir)
{
    if(_isBusy) return;
    qnam.clearAccessCache();
    _isBusy=true;
    _isFine=false;
    url = urlfile;

    QFileInfo fileInfo(url.path());
    QString fileName = fileInfo.fileName();
    if (fileName.isEmpty())
        fileName = "index.html";

    targetFile=QFileInfo(toDir+"/"+fileName).filePath();
    QDir target = QFileInfo(toDir+"/"+fileName).absoluteDir();
    target.mkpath(target.absolutePath());

    if (QFile::exists(targetFile))
    {
        QFile::remove(targetFile);
    }

    file = new QFile(targetFile);
    if (!file->open(QIODevice::WriteOnly)) {
        QMessageBox::information(NULL, tr("HTTP"),
                                 tr("Unable to save the file %1: %2.")
                                 .arg(fileName).arg(file->errorString()));
        delete file;
        file = 0;
        _isBusy=false;
        _isFine=false;
        return;
    }
    // schedule the request
    httpRequestAborted = false;
    startRequest(url);
}

void HttpDownloader::cancelDownload()
{
    if(!_isBusy) return;
    httpRequestAborted = true;
    _isBusy=false;
    _isFine=false;
    if(!reply) return;
    reply->abort();
    emit canceled();
}

void HttpDownloader::httpFinished()
{
    if (httpRequestAborted) {
        if (file) {
            file->close();
            file->remove();
            delete file;
            file = 0;
        }
        _isBusy=false;
        reply->deleteLater();
        return;
    }
    file->flush();
    file->close();
    QString errorString;
    _isBusy=false;

    QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if (reply->error()) {
        file->remove();
        errorString=tr("Download failed: %1.")
                                 .arg(reply->errorString());
        _isBusy=false;
        _isFine=false;
    } else
    if (!redirectionTarget.isNull())
    {
        QUrl newUrl = url.resolved(redirectionTarget.toUrl());
        url = newUrl;
        reply->deleteLater();
        file->open(QIODevice::WriteOnly);
        file->resize(0);
        startRequest(url);
        _isBusy=true;
        return;

    } else {
        _isFine=true;
    }
    reply->close();
    reply->deleteLater();
    reply = 0;
    delete file;
    file = 0;

    if(!_isFine)
        emit failed(errorString);
    else
        emit finished();
}

void HttpDownloader::httpFailed(QNetworkReply::NetworkError)
{
    qWarning() << reply->errorString();
}

void HttpDownloader::httpReadyRead()
{
    // this slot gets called every time the QNetworkReply has new data.
    // We read all of its new data and write it into the file.
    // That way we use less RAM than when reading it at the finished()
    // signal of the QNetworkReply
    if (file)
        file->write(reply->readAll());
}

void HttpDownloader::updateDataReadProgress(qint64 bytesRead, qint64 totalBytes)
{
    if (httpRequestAborted)
        return;

    emit progress(bytesRead, totalBytes);
}

void HttpDownloader::slotAuthenticationRequired(QNetworkReply*, QAuthenticator *authenticator)
{
//    QDialog dlg;
//    dlg.adjustSize();
//    ui.siteDescription->setText(tr("%1 at %2").arg(authenticator->realm()).arg(url.host()));

//    // Did the URL have information? Fill the UI
//    // This is only relevant if the URL-supplied credentials were wrong
//    ui.userEdit->setText(url.userName());
//    ui.passwordEdit->setText(url.password());

//    if (dlg.exec() == QDialog::Accepted) {
//        authenticator->setUser(ui.userEdit->text());
//        authenticator->setPassword(ui.passwordEdit->text());
          authenticator->setUser("login");
          authenticator->setPassword("password");
          //    }
}

bool HttpDownloader::isBusy()
{
    return _isBusy;
}

bool HttpDownloader::isFine()
{
    return _isFine;
}

bool HttpDownloader::isAborted()
{
    return httpRequestAborted;
}

#ifndef QT_NO_SSL
void HttpDownloader::sslErrors(QNetworkReply*,const QList<QSslError> &errors)
{
    QString errorString;
    foreach (const QSslError &error, errors) {
        if (!errorString.isEmpty())
            errorString += ", ";
        errorString += error.errorString();
    }

    if (QMessageBox::warning(NULL, tr("HTTP"),
                             tr("One or more SSL errors has occurred: %1").arg(errorString),
                             QMessageBox::Ignore | QMessageBox::Abort) == QMessageBox::Ignore) {
        reply->ignoreSslErrors();
    }
}
#endif
