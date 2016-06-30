/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2016 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "js_common.h"
#include <QMessageBox>
#include <QWidget>

PGE_JS_Common::PGE_JS_Common(QObject *parent)
    : PGE_JS_ProxyBase(parent)
{}

PGE_JS_Common::~PGE_JS_Common() {}

void PGE_JS_Common::bindObjects(QJSEngine *engine)
{
    if(!engine) return;
    PGE_JS_Common* proxy = new PGE_JS_Common(parent());

    proxy->m_parentWidget = m_parentWidget;

    QJSValue objectValue = engine->newQObject(proxy);
    engine->globalObject().setProperty("PGE", objectValue);
}

int PGE_JS_Common::msgBoxInfo(QString title, QString message)
{
    return int(QMessageBox::information(m_parentWidget, title, message));
}

int PGE_JS_Common::msgBoxWarning(QString title, QString message)
{
    return int(QMessageBox::warning(m_parentWidget, title, message));
}

int PGE_JS_Common::msgBoxError(QString title, QString message)
{
    return int(QMessageBox::critical(m_parentWidget, title, message));
}

