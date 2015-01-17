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

    checkingInProcess=false;
    _backup="";
    which_version=V_STABLE;
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
    if(checkingInProcess) return;
    url = STABLE_CHECK;
    buffer.clear();
    httpRequestAborted = false;
    checkingInProcess = true;
    _backup = ui->CheckStable->text();
    ui->CheckStable->setText(tr("Checking..."));
    ui->CheckStable->setEnabled(false);
    which_version=V_STABLE;
    startRequest(url);
}



void UpdateChecker::on_CheckAlpha_clicked()
{
    if(checkingInProcess) return;
    url = DEVEL_CHECK;
    buffer.clear();
    httpRequestAborted = false;
    checkingInProcess = true;
    _backup = ui->CheckAlpha->text();
    ui->CheckAlpha->setText(tr("Checking..."));
    ui->CheckAlpha->setEnabled(false);
    which_version=V_DEVEL;
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
    checkingInProcess=false;

    switch(which_version)
    {
    case V_STABLE:
        ui->CheckStable->setText(_backup);
        ui->CheckStable->setEnabled(true);
        break;
    case V_DEVEL:
        ui->CheckAlpha->setText(_backup);
        ui->CheckAlpha->setEnabled(true);
        break;
    }
}

void UpdateChecker::httpFinished()
{
    checkingInProcess=false;

    switch(which_version)
    {
    case V_STABLE:
        ui->CheckStable->setText(_backup);
        ui->CheckStable->setEnabled(true);
        break;
    case V_DEVEL:
        ui->CheckAlpha->setText(_backup);
        ui->CheckAlpha->setEnabled(true);
        break;
    }

    if (httpRequestAborted) {
        buffer.clear();
        reply->deleteLater();
        //progressDialog->hide();
        return;
    }

    QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if (reply->error()) {
        buffer.clear();
        QMessageBox::warning(this, tr("HTTP"),
                                 tr("Check failed: %1.")
                                 .arg(reply->errorString()));

        switch(which_version)
        {
        case V_STABLE:
            ui->updatesStable->setText(QString("<span style=\"color:#FF0000;\">%1</span>").arg(tr("Check failed!")));
            break;
        case V_DEVEL:
            ui->updatesAlpha->setText(QString("<span style=\"color:#FF0000;\">%1</span>").arg(tr("Check failed!")));
            break;
        }
    }
    else if (!redirectionTarget.isNull())
    {
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
        QString latest = QString::fromLocal8Bit(buffer);
        latest.remove('\n');
        latest = latest.trimmed();

        switch(which_version)
        {
        case V_STABLE:
            if(latest == QString("%1%2").arg(_FILE_VERSION).arg(_FILE_RELEASE))
                ui->updatesStable->setText(QString("<span style=\"color:#005500;\">You have a latest version!</span>"));
            else
                ui->updatesStable->setText(QString("<span style=\"color:#0000FF;\"><a href=\"%1\">Available new update!</a></span>")
                                           .arg(STABLE_LINK));
            break;
        case V_DEVEL:
            if(latest == QString("%1").arg(_DATE_OF_BUILD))
                ui->updatesAlpha->setText(QString("<span style=\"color:#005500;\">You have a latest version!</span>"));
            else
                ui->updatesAlpha->setText(QString("<span style=\"color:#0000FF;\"><a href=\"%1\">Available new update!</a></span>")
                                           .arg(DEVEL_LINK));
            break;
        }

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

