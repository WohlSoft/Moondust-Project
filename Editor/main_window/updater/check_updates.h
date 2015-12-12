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

#ifndef CHECK_UPDATES_H
#define CHECK_UPDATES_H

#include <QDialog>
#include <QUrl>
#include <QNetworkAccessManager>

namespace Ui {
class UpdateChecker;
}

QT_BEGIN_NAMESPACE
class QDialogButtonBox;
class QFile;
class QLabel;
class QLineEdit;
class QProgressDialog;
class QPushButton;
class QSslError;
class QAuthenticator;
class QNetworkReply;
QT_END_NAMESPACE

class UpdateChecker : public QDialog
{
    Q_OBJECT

public:
    explicit UpdateChecker(QWidget *parent = 0);
    ~UpdateChecker();
    void startRequest(QUrl url);

    enum VersionType
    {
        V_STABLE=0,
        V_DEVEL
    };

private slots:
    void cancelDownload();
    void httpFinished();
    void httpReadyRead();
    void updateDataReadProgress(qint64 bytesRead, qint64 totalBytes);
    void runShadowUpdateCheck();

#ifndef QT_NO_SSL
    void sslErrors(QNetworkReply*,const QList<QSslError> &errors);
#endif

private:
    QUrl url;
    QNetworkAccessManager qnam;
    QNetworkReply *reply;
    int httpGetId;
    bool httpRequestAborted;
    QByteArray buffer;

    bool checkingInProcess;
    QString _backup;
    int which_version;

    bool shadowCheck;
    bool autoCheckStable;
    bool autoCheckAlpha;

private slots:
    void on_close_clicked();
    void on_CheckStable_clicked();

    void on_CheckAlpha_clicked();

private:
    Ui::UpdateChecker *ui;
};

#endif // CHECK_UPDATES_H
