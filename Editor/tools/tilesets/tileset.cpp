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

#include <QPainter>
#include <QPaintEvent>
#include <QMimeData>
#include <QFileInfo>
#include <QDrag>

#include <common_features/graphics_funcs.h>
#include <common_features/items.h>
#include <pge_qt_compat.h>

#include "../../defines.h"
#include "tileset.h"


tileset::tileset(DataConfig *conf, int type, QWidget *parent, int baseSize, int rows, int cols, QGraphicsScene *scene) :
    QWidget(parent)
{
    mode = GFX_Staff;
    scn = scene;

    if(scene != nullptr)
    {
        if(QString(scn->metaObject()->className()) == "LvlScene") mode = GFX_Level;
        else if(QString(scn->metaObject()->className()) == "WldScene") mode = GFX_World;
    }

    setEditMode(true);
    m_baseSize = baseSize;
    m_rows = rows;
    m_cols = cols;
    m_conf = conf;
    m_type = type;
    updateSize();
}

void tileset::clear()
{
    pieces.clear();
    highlightedRect = QRect();
    update();
}

void tileset::paintEvent(QPaintEvent *ev)
{
    QPainter painter;
    painter.begin(this);
    painter.fillRect(ev->rect(), Qt::white);

    // ------- Draw grid -------
    painter.setPen(QPen(QBrush(Qt::lightGray), 1, Qt::DashDotLine));
    for(int w = 1; w < m_cols; w++)
    {
        painter.drawLine((w * 32) - 1,
                         0,
                         (w * 32) - 1,
                         m_rows * 32);
    }

    for(int h = 1; h < m_rows; h++)
    {
        painter.drawLine(0,
                         (h * 32) - 1,
                         m_cols * 32,
                         (h * 32) - 1);
    }
    // -------------------------

    if(highlightedRect.isValid())
    {
        painter.setBrush(QColor("#ffcccc"));
        painter.setPen(Qt::NoPen);
        painter.drawRect(highlightedRect.adjusted(0, 0, -1, -1));
    }

    if(pieces.isEmpty())
    {
        painter.setPen(QPen(QBrush(Qt::black), 1));

        if(highlightedRect.isEmpty())
        {
            painter.drawText(QRect(0, 0, m_cols * 32, m_rows * 32),
                             Qt::AlignCenter,
                             tr("Drag & Drop items into this box!\nRight-click to remove!"));
        }
    }
    else
    {
        for(int i = 0; i < pieces.size(); ++i)
            painter.drawPixmap(pieces[i].rect, pieces[i].pixmap);
    }

    painter.end();
}

void tileset::dragEnterEvent(QDragEnterEvent *event)
{
    QString fmt = isSupportedFormat(event->mimeData()->formats());

    if(fmt.isEmpty())
        event->ignore();
    else
        event->accept();
}

void tileset::dragLeaveEvent(QDragLeaveEvent *event)
{
    highlightedRect = QRect();
    update();
    event->accept();
}

void tileset::dragMoveEvent(QDragMoveEvent *event)
{
    QString fmt = isSupportedFormat(event->mimeData()->formats());

    if(!fmt.isEmpty() && findPiece(targetSquare(event->Q_EventPos())) == -1)
    {
        highlightedRect = targetSquare(event->Q_EventPos());
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
    else
    {
        highlightedRect = QRect();
        event->ignore();
    }

    update();
}

void tileset::dropEvent(QDropEvent *event)
{
    QString fmt = isSupportedFormat(event->mimeData()->formats());

    if(!fmt.isEmpty() && findPiece(targetSquare(event->Q_EventPos())) == -1)
    {
        QByteArray pieceData = event->mimeData()->data(fmt);
        QDataStream stream(&pieceData, QIODevice::ReadOnly);
        QRect square = targetSquare(event->Q_EventPos());
        int objID, objType;
        stream >> objID;
        stream >> objType;

        Piece p;
        Items::getItemGFX(objType, (unsigned long)objID, p.pixmap, scn, false, QSize(m_baseSize, m_baseSize));
        p.rect = square;
        p.id = objID;
        p.type = objType;
        pieces.append(p);

        highlightedRect = QRect();
        update();

        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
    else
    {
        highlightedRect = QRect();
        event->ignore();
    }
}

void tileset::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton)
    {
        if(m_editMode)
        {
            auto item = findPiece(targetSquare(event->pos()));
            pieces.removeAt(item);
            update();
            return;
        }

        return;
    }

    QRect square = targetSquare(event->pos());
    int found = findPiece(square);

    if(found == -1)
        return;

    if(!m_editMode)
    {
        emit clickedItem(pieces[found].type >= 0 ? pieces[found].type : m_type, pieces[found].id);
        return;
    }

    Piece p = pieces[found];
    pieces.removeAt(found);
    update();

    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);

    dataStream << (int)p.id;
    dataStream << (int)p.type;

    QMimeData *mimeData = new QMimeData;
    mimeData->setData(getMimeType(p.type), itemData);

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setHotSpot(event->pos() - square.topLeft());
    drag->setPixmap(p.pixmap);

    if(drag->exec(Qt::MoveAction) == 0)
    {
        pieces.insert(found, p);
        update();
    }
}

int tileset::findPiece(const QRect &pieceRect) const
{
    for(int i = 0; i < pieces.size(); ++i)
    {
        if(pieceRect == pieces[i].rect)
            return i;
    }

    return -1;
}

const QRect tileset::targetSquare(const QPoint &position) const
{
    return QRect(position.x() / getBaseSize() * getBaseSize(), position.y() / getBaseSize() * getBaseSize(), getBaseSize(), getBaseSize());
}

QString tileset::isSupportedFormat(const QStringList &formats)
{
    QSet<QString> fmts;
    fmts << QString("text/x-pge-piece-block")
         << QString("text/x-pge-piece-bgo")
         << QString("text/x-pge-piece-npc")
         << QString("text/x-pge-piece-tile")
         << QString("text/x-pge-piece-path")
         << QString("text/x-pge-piece-scenery")
         << QString("text/x-pge-piece-level")
         << QString("text/x-pge-piece");

    foreach(const QString &q, formats)
    {
        if(fmts.contains(q))
            return q;
    }

    return QString();
}

QString tileset::getMimeType(int type)
{
    switch(type)
    {
    case ItemTypes::LVL_Block:
        return QString("text/x-pge-piece-block");
    case ItemTypes::LVL_BGO:
        return QString("text/x-pge-piece-bgo");
    case ItemTypes::LVL_NPC:
        return QString("text/x-pge-piece-npc");
    case ItemTypes::WLD_Tile:
        return QString("text/x-pge-piece-tile");
    case ItemTypes::WLD_Path:
        return QString("text/x-pge-piece-path");
    case ItemTypes::WLD_Scenery:
        return QString("text/x-pge-piece-scenery");
    case ItemTypes::WLD_Level:
        return QString("text/x-pge-piece-level");
    default:
        break;
    }

    return QString("image/x-pge-piece");
}

QString tileset::name() const
{
    return m_name;
}

void tileset::setName(const QString &name)
{
    m_name = name;
}

bool tileset::editMode() const
{
    return m_editMode;
}

void tileset::setEditMode(bool editMode)
{
    m_editMode = editMode;
    setAcceptDrops(editMode);
}

int tileset::type() const
{
    return m_type;
}

void tileset::setType(const int &type)
{
    m_type = type;
}

int tileset::getBaseSize() const
{
    return m_baseSize;
}

SimpleTileset tileset::toSimpleTileset()
{
    SimpleTileset s;
    s.rows = m_rows;
    s.cols = m_cols;
    s.type = m_type;
    s.tileSetName = m_name;

    QRect scope(0, 0, m_baseSize * m_cols, m_baseSize * m_rows);

    for(int i = 0; i < pieces.size(); ++i)
    {
        Piece &p = pieces[i];
        SimpleTilesetItem it;

        if(!((scope + QMargins(1, 1, 1, 1)).contains(p.rect, true)))
            continue; // Don't include out of scope items!

        it.col = p.rect.x() / m_baseSize;
        it.row = p.rect.y() / m_baseSize;
        it.id = p.id;
        it.type = p.type;
        s.items << it;
    }

    return s;
}

void tileset::loadSimpleTileset(const SimpleTileset &tileset)
{
    clear();
    setRows(tileset.rows);
    setCols(tileset.cols);
    setType(tileset.type);
    setName(tileset.tileSetName);

    for(int i = 0; i < tileset.items.size(); ++i)
    {
        auto &item = tileset.items[i];
        Piece p;

        p.type = item.type >= 0 ? item.type : m_type;
        Items::getItemGFX(p.type, item.id, p.pixmap, scn, false, QSize(m_baseSize, m_baseSize));
        p.rect = QRect(item.col * m_baseSize, item.row * m_baseSize, m_baseSize, m_baseSize);
        p.id = item.id;

        pieces.append(p);
    }
}

void tileset::SaveSimpleTileset(const QString &path, const SimpleTileset &tileset)
{
    QString modifiedPath = path;
    QSettings simpleTilesetINI(modifiedPath, QSettings::IniFormat);

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    simpleTilesetINI.setIniCodec("UTF-8");
#endif

    simpleTilesetINI.clear();
    simpleTilesetINI.beginGroup("tileset"); //HEADER
    simpleTilesetINI.setValue("rows", tileset.rows);
    simpleTilesetINI.setValue("cols", tileset.cols);
    simpleTilesetINI.setValue("name", tileset.tileSetName);
    simpleTilesetINI.setValue("type", static_cast<int>(tileset.type));
    simpleTilesetINI.endGroup();

    for(int i = 0; i < tileset.items.size(); ++i)
    {
        auto &item = tileset.items[i];

        simpleTilesetINI.beginGroup(QString("item-%1-%2").arg(item.col).arg(item.row));
        simpleTilesetINI.setValue("id", item.id);

        if(item.type >= 0)
            simpleTilesetINI.setValue("type", item.type);
        else
            simpleTilesetINI.remove("type");

        simpleTilesetINI.endGroup();
    }
}

bool tileset::OpenSimpleTileset(const QString &path, SimpleTileset &tileset)
{
    QSettings simpleTilesetINI(path, QSettings::IniFormat);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    simpleTilesetINI.setIniCodec("UTF-8");
#endif

    QStringList groups = simpleTilesetINI.childGroups();
    int tilesetindex;

    if((tilesetindex = groups.indexOf("tileset")) < 0)
        return false;

    QFileInfo pathInfo(path);
    simpleTilesetINI.beginGroup("tileset");
    tileset.rows = (unsigned int)simpleTilesetINI.value("rows", 3).toInt();
    tileset.cols = (unsigned int)simpleTilesetINI.value("cols", 3).toInt();
    tileset.type = simpleTilesetINI.value("type", 0).toInt();
    tileset.tileSetName = simpleTilesetINI.value("name", "").toString();
    tileset.fileName = pathInfo.fileName();
    simpleTilesetINI.endGroup();

    groups.removeAt(tilesetindex);

    for(int i = 0; i < groups.size(); ++i)
    {
        QString gr = groups[i];
        if(!gr.startsWith("item-"))
            return false;

        QStringList spData = gr.split("-");

        if(spData.size() != 3)
            return false;

        bool succ = false;

        SimpleTilesetItem it;
        it.col = spData[1].toInt(&succ);
        if(!succ)
            return false;

        it.row = spData[2].toInt(&succ);
        if(!succ)
            return false;

        simpleTilesetINI.beginGroup(gr);
        it.id = simpleTilesetINI.value("id", 0).toUInt();
        it.type = simpleTilesetINI.value("type", -1).toInt();
        simpleTilesetINI.endGroup();
        tileset.items << it;
    }

    return true;
}

void tileset::setBaseSize(int value)
{
    m_baseSize = value;
    updateSize();
    update();
}


void tileset::updateSize()
{
    setMaximumSize(QSize(m_baseSize * m_cols, m_baseSize * m_rows));
    setMinimumSize(QSize(m_baseSize * m_cols, m_baseSize * m_rows));
}

// void tileset::removeOuterItems(QRect updatedRect)
// {
//     if(pieces.size() == 0)
//         return;

//     for(int i = 0; i < pieces.size(); ++i)
//     {
//         if(!((updatedRect + QMargins(1, 1, 1, 1)).contains(pieces[i].rect, true)))
//             pieces.removeAt(i--);
//     }
// }

int tileset::cols() const
{
    return m_cols;
}

void tileset::setCols(int cols)
{
    m_cols = cols;
    updateSize();
    update();
}

int tileset::rows() const
{
    return m_rows;
}

void tileset::setRows(int rows)
{
    m_rows = rows;
    updateSize();
    update();
}
