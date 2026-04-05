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

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QDialog>
#include <QMenu>

#include <translator-qt/translator.h>

namespace Ui
{
    class MaintainerMain;
}

class MaintainerMain : public QDialog
{
    Q_OBJECT

public:
    explicit MaintainerMain(QWidget *parent = nullptr);
    ~MaintainerMain();

    void showWIP();

private slots:
    void languageSwitched();

    void on_quitOut_clicked();
    void on_music_converter_batch_clicked();
    void on_episode_converter_clicked();
    void on_gifs2png_converter_clicked();
    void on_png2gifs_converter_clicked();
    void on_lazyfix_tool_clicked();
    void on_palette_filter_clicked();
    void on_pathcase_fixer_clicked();
#ifdef PGE_ENABLE_XTCONVERT
    void on_xt_convert_clicked();
#endif

private:
    Ui::MaintainerMain *ui;
    QMenu      m_langMenu;
    Translator m_translator;
};

#endif // MAIN_WINDOW_H
