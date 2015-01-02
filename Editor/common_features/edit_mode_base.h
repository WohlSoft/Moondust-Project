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

#ifndef EDITMODE_H
#define EDITMODE_H

#include <QGraphicsScene>
#include <QObject>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>

class EditMode : public QObject
{
    Q_OBJECT
public:
    explicit EditMode(QString toolTitle, QGraphicsScene *ParentScene = 0, QObject *parent = 0);
    virtual ~EditMode();
    virtual void set();
    QString name() const;
    virtual void mousePress(QGraphicsSceneMouseEvent *mouseEvent);
    virtual void mouseMove(QGraphicsSceneMouseEvent *mouseEvent);
    virtual void mouseRelease(QGraphicsSceneMouseEvent *mouseEvent);

    virtual void keyPress(QKeyEvent *keyEvent);
    virtual void keyRelease(QKeyEvent *keyEvent);

    bool noEvent();

signals:

public slots:

protected:
    QString toolName;
    QGraphicsScene *scene;
    bool dontCallEvent;
};

#endif // EDITMODE_H
