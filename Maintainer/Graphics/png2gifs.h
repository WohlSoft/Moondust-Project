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

#ifndef PNG2GIFS_H
#define PNG2GIFS_H

#include <QDialog>
#include <QProcess>

namespace Ui {
class PNG2GIFs;
}

class PNG2GIFs : public QDialog
{
    Q_OBJECT

    QProcess m_png2gifs;
public:
    explicit PNG2GIFs(QWidget *parent = nullptr);
    ~PNG2GIFs();

protected:
    void closeEvent(QCloseEvent *e);
    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent *e);

private slots:
    void on_doMadJob_clicked();
    void on_inPathBrowse_clicked();
    void processOutput();
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    void toggleBusy(bool busy);
    Ui::PNG2GIFs *ui;
};

#endif // PNG2GIFS_H
