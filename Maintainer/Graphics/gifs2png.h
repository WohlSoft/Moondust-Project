/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2026 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef GIFS2PNG_H
#define GIFS2PNG_H

#include <QDialog>
#include <QProcess>

namespace Ui {
class GIFs2PNG;
}

class GIFs2PNG : public QDialog
{
    Q_OBJECT

    QProcess m_gifs2png;
public:
    explicit GIFs2PNG(QWidget *parent = nullptr);
    ~GIFs2PNG();

    void setConfigPackPath(const QString &path);

protected:
    void closeEvent(QCloseEvent *e);
    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent *e);

private slots:
    void on_doMadJob_clicked();
    void on_configPackChoose_currentIndexChanged(int index);
    void on_configPackPathBrowse_clicked();
    void on_inPathBrowse_clicked();
    void processOutput();
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    void findConfigPacks();
    void toggleBusy(bool busy);
    Ui::GIFs2PNG *ui;
};

#endif // GIFS2PNG_H
