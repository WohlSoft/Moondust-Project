/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef WINE_SETUP_H
#define WINE_SETUP_H

#include <QDialog>
#include <QProcessEnvironment>
#include <QVector>
#include <QProcess>

#include "wine_setup_cfg.h"

namespace Ui {
class WineSetup;
}

struct PlayOnProfile
{
    enum Bits
    {
        BITS_32 = 32,
        BITS_64 = 64
    } bits;
    QString name;
    QString winePrefix;
    QString wineRoot;
};

class WineSetup : public QDialog
{
    Q_OBJECT

    QVector<PlayOnProfile> m_polProfiles;
    QProcess m_wineTestProc;
public:
    explicit WineSetup(QWidget *parent = nullptr);
    ~WineSetup();

    void fetchPlayOnLinux();

    WineSetupData getSetup();
    void setSetup(const WineSetupData &setup);

    static void prepareSetup(WineSetupData &setup);
    static QProcessEnvironment buildEnv(const WineSetupData &profile);

private slots:
    void on_doImportFromPoL_clicked();
    void on_winePrefixBrowse_clicked();
    void on_wineRootPathBrowse_clicked();
    void on_runWineCfg_clicked();
    void on_runWineCmd_clicked();
    void on_wineStopProc_clicked();

#if QT_VERSION_CHECK(5, 6, 0)
    void testErrorOccurred(QProcess::ProcessError error);
#endif
    void testStarted();
    void testFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void testStateChanged(QProcess::ProcessState newState);
    void testReadyReadStandardError();
    void testReadyReadStandardOutput();

private:
    Ui::WineSetup *ui;
};

#endif // WINE_SETUP_H
