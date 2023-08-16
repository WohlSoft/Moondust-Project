/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2023 Vitaly Novichkov <admin@wohlnet.ru>
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
#include <pge_qt_compat.h>

#include <version.h>
#include <common_features/app_path.h>
#include <common_features/version_cmp.h>

#include <QDateTime>
#include <QStringList>

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

    shadowCheck = false;

    //Temporary disable checkboxes, because feature was not implemented
    ui->AutoCheckStable->setEnabled(false);
    ui->AutoCheckAlpha->setEnabled(false);
}

UpdateChecker::~UpdateChecker()
{
    delete ui;
}

void UpdateChecker::on_close_clicked()
{
    this->close();
}

static int Month2Code(QString &mon)
{
    if(mon=="Jan")
        return 1;
    else if(mon=="Feb")
        return 2;
    else if(mon=="Mar")
        return 3;
    else if(mon=="Apr")
        return 4;
    else if(mon=="May")
        return 5;
    else if(mon=="Jun")
        return 6;
    else if(mon=="Jul")
        return 7;
    else if(mon=="Aug")
        return 8;
    else if(mon=="Sep")
        return 9;
    else if(mon=="Oct")
        return 10;
    else if(mon=="Nov")
        return 11;
    else if(mon=="Dec")
        return 12;
    else
        return 1;
}

static qint64 BuildDateToTimestamp(const char *buildTime)
{
    QString src(buildTime);
    qint64 result = 0;
    QDate date;
    QTime time;
    int years = 0;
    int months = 0;
    int days = 0;
    int hours = 0;
    int minutes = 0;
    int seconds = 0;

    src = src.simplified();
    src.replace(' ', '-');
    src.replace('.', '-');
    src.replace(':', '-');
    QStringList datetime = src.split('-', QSTRING_SPLIT_BEHAVIOUR(SkipEmptyParts));

    for(int i=0; i<datetime.size(); i++)
    {
        QString &elem = datetime[i];
        switch(i)
        {
        case 0:
            months = Month2Code(elem); break;
        case 1:
            days = elem.toInt(); break;
        case 2:
            years = elem.toLong(); break;
        case 3:
            hours = elem.toLong(); break;
        case 4:
            minutes = elem.toLong(); break;
        case 5:
            seconds = elem.toLong(); break;
        }
    }

    date.setDate(years, months, days);
    time.setHMS(hours, minutes, seconds);

#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
    result = QDateTime(date, time).toSecsSinceEpoch();
#else
    result = (qint64)QDateTime(date, time).toTime_t();
#endif

    return result;
}

static qint64 DatetimeToTimestamp(QString src)
{
    qint64 result = 0;
    QDate date;
    QTime time;
    int years = 0;
    int months = 0;
    int days = 0;
    int hours = 0;
    int minutes = 0;
    int seconds = 0;

    src = src.simplified();
    src.replace(' ', '-');
    src.replace('.', '-');
    src.replace(':', '-');
    QStringList datetime = src.split('-', QSTRING_SPLIT_BEHAVIOUR(SkipEmptyParts));

    for(int i=0; i<datetime.size(); i++)
    {
        QString &elem = datetime[i];
        switch(i)
        {
        case 0:
            years = elem.toInt(); break;
        case 1:
            months = elem.toInt(); break;
        case 2:
            days = elem.toInt(); break;
        case 3:
            hours = elem.toInt(); break;
        case 4:
            minutes = elem.toInt(); break;
        case 5:
            seconds = elem.toInt(); break;
        }
    }
    date.setDate(years, months, days);
    time.setHMS(hours, minutes, seconds);

#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
    result = QDateTime(date, time).toSecsSinceEpoch();
#else
    result = (qint64)QDateTime(date, time).toTime_t();
#endif

    return result;
}

//static int DatetimeToTimestamp(const char *buildTime)
//{
//    QString src(buildTime);
//    return DatetimeToTimestamp(src);
//}




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
    }
    else
    {
        //QString fileName = QFileInfo(QUrl(urlLineEdit->text()).path()).fileName();
        //statusLabel->setText(tr("Downloaded %1 to %2.").arg(fileName).arg(QDir::currentPath()));
        QString latest = QString::fromLocal8Bit(buffer);
        latest = latest.trimmed();
        latest.remove('\n');

        bool isLatest = ( DatetimeToTimestamp(latest) <= BuildDateToTimestamp(V_DATE_OF_BUILD) );

        switch(which_version)
        {
        case V_STABLE:
            //qDebug() << QString("%1").arg(_LATEST_STABLE) << "vs" << latest;

            qDebug() << QString("%1").arg(V_DATE_OF_BUILD) << "vs" << latest;
            //qDebug() << VersionCmp::compare(QString("%1").arg(_LATEST_STABLE), latest) << " win!" <<
            qDebug() << ( isLatest ? "fresh" : "old" );

            //( ( QString("%1").arg(_LATEST_STABLE) == VersionCmp::compare(QString("%1").arg(_LATEST_STABLE), latest) ) ? "fresh" : "old" );
            //if( QString("%1").arg(_LATEST_STABLE) == VersionCmp::compare(QString("%1").arg(_LATEST_STABLE), latest) )

            if( isLatest )
                ui->updatesStable->setText( QString( "<span style=\"color:#005500;\">%1</span>" )
                                            .arg( tr("You have a latest version!" ) ) );
            else
                ui->updatesStable->setText( QString( "<span style=\"color:#0000FF;\"><a href=\"%1\">%2</a></span>" )
                                            .arg(STABLE_LINK).arg( tr("Available new update!") ) );
            break;
        case V_DEVEL:
            if( isLatest )
                ui->updatesAlpha->setText( QString("<span style=\"color:#005500;\">%1</span>").arg( tr("You have a latest version!") ) );
            else
                ui->updatesAlpha->setText( QString("<span style=\"color:#0000FF;\"><a href=\"%1\">%3 %2</a></span>" )
                                           .arg(DEVEL_LINK).arg(latest).arg( tr("Latest update is") ) );
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

void UpdateChecker::runShadowUpdateCheck()
{
    shadowCheck=true;
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

