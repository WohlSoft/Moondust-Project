/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2015 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QSettings>
#include <QtNetwork>
#include <QMessageBox>
#include <QFile>

#include <version.h>
#include <common_features/app_path.h>

#include "updater_links.h"

#include "check_updates.h"
#include "ui_check_updates.h"

UpdateChecker::UpdateChecker(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UpdateChecker)
{
    ui->setupUi(this);
}

UpdateChecker::~UpdateChecker()
{
    delete ui;
}

void UpdateChecker::on_close_clicked()
{
    this->close();
}





void UpdateChecker::on_CheckStable_clicked()
{
    url = STABLE_CHECK;
    buffer.clear();
    httpRequestAborted = false;
    startRequest(url);
}







void UpdateChecker::startRequest(QUrl url)
{
    reply = qnam.get(QNetworkRequest(url));
    connect(reply, SIGNAL(finished()),
            this, SLOT(httpFinished()));
    connect(reply, SIGNAL(readyRead()),
            this, SLOT(httpReadyRead()));
    connect(reply, SIGNAL(downloadProgress(qint64,qint64)),
            this, SLOT(updateDataReadProgress(qint64,qint64)));
}

void UpdateChecker::cancelDownload()
{
    //statusLabel->setText(tr("Download canceled."));
    httpRequestAborted = true;
    reply->abort();
    //downloadButton->setEnabled(true);
}

void UpdateChecker::httpFinished()
{
    if (httpRequestAborted) {
        buffer.clear();
        reply->deleteLater();
        //progressDialog->hide();
        return;
    }

    QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if (reply->error()) {
        buffer.clear();
        QMessageBox::information(this, tr("HTTP"),
                                 tr("Download failed: %1.")
                                 .arg(reply->errorString()));
        ui->updatesStable->setText(QString("Fail: %1").arg(reply->errorString()));
        //downloadButton->setEnabled(true);
    } else if (!redirectionTarget.isNull()) {
        QUrl newUrl = url.resolved(redirectionTarget.toUrl());
        if (QMessageBox::question(this, tr("HTTP"),
                                  tr("Redirect to %1 ?").arg(newUrl.toString()),
                                  QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
            url = newUrl;
            reply->deleteLater();
            buffer.clear();
            startRequest(url);
            return;
        }
    } else {
        //QString fileName = QFileInfo(QUrl(urlLineEdit->text()).path()).fileName();
        //statusLabel->setText(tr("Downloaded %1 to %2.").arg(fileName).arg(QDir::currentPath()));
        ui->updatesStable->setText(QString("Latest is %1").arg(QString::fromLocal8Bit(buffer)));
        //downloadButton->setEnabled(true);
    }

    reply->deleteLater();
    reply = 0;
}

void UpdateChecker::httpReadyRead()
{
    // this slot gets called every time the QNetworkReply has new data.
    // We read all of its new data and write it into the file.
    // That way we use less RAM than when reading it at the finished()
    // signal of the QNetworkReply
    buffer.append(reply->readAll());
}

void UpdateChecker::updateDataReadProgress(qint64 bytesRead, qint64 totalBytes)
{
    if (httpRequestAborted)
        return;

    Q_UNUSED(totalBytes);
    Q_UNUSED(bytesRead);
}

#ifndef QT_NO_SSL
void UpdateChecker::sslErrors(QNetworkReply*,const QList<QSslError> &errors)
{
    QString errorString;
    foreach (const QSslError &error, errors) {
        if (!errorString.isEmpty())
            errorString += ", ";
        errorString += error.errorString();
    }

    if (QMessageBox::warning(this, tr("HTTP"),
                             tr("One or more SSL errors has occurred: %1").arg(errorString),
                             QMessageBox::Ignore | QMessageBox::Abort) == QMessageBox::Ignore) {
        reply->ignoreSslErrors();
    }
}
#endif

