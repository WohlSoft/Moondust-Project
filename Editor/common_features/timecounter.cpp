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

#include "timecounter.h"
#include "logger.h"

TimeCounter::TimeCounter()
{}

void TimeCounter::start()
{
    m_ticker.start();
    m_ticker.restart();
}

void TimeCounter::stop(QString msg, int items)
{
    int counter = m_ticker.elapsed();
    if(items >= 0)
        LogDebug(QString("Performance test-> %1 %2 in %3 ms").arg(items).arg(msg).arg(counter));
}

int TimeCounter::current()
{
    return m_ticker.elapsed();
}
