/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
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
#include <QDrag>

#include <common_features/graphics_funcs.h>
#include <common_features/items.h>

#include "tileset.h"

tileset::tileset(dataconfigs* conf, int type, QWidget *parent, int baseSize, int rows, int cols, QGraphicsScene *scene) :
    QWidget(parent)
{
    mode = GFX_Staff;
    scn = scene;
    if(scene!=NULL)
    {
        if(QString(scn->metaObject()->className())=="LvlScene") mode = GFX_Level;
        else
        if(QString(scn->metaObject()->className())=="WldScene") mode = GFX_World;
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
    piecePixmaps.clear();
    pieceRects.clear();
    pieceID.clear();
    highlightedRect = QRect();
    inPlace = 0;
    update();
}

void tileset::paintEvent(QPaintEvent *ev)
{
    QPainter painter;
    painter.begin(this);
    painter.fillRect(ev->rect(), Qt::white);

    if (highlightedRect.isValid()) {
        painter.setBrush(QColor("#ffcccc"));
        painter.setPen(Qt::NoPen);
        painter.drawRect(highlightedRect.adjusted(0, 0, -1, -1));
    }

    if(pieceRects.isEmpty()){
        if(highlightedRect.isEmpty())
            painter.drawText(ev->rect(), Qt::AlignCenter, tr("Drag & Drop items to this box!\nRightclick to remove!"));
    }else{
        for (int i = 0; i < pieceRects.size(); ++i) {
            painter.drawPixmap(QRect(pieceRects[i].x(), pieceRects[i].y(), piecePixmaps[i].width(), piecePixmaps[i].height()), piecePixmaps[i]);
        }
    }

    painter.end();
}

void tileset::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat(getMimeType()))
        event->accept();
    else
        event->ignore();
}

void tileset::dragLeaveEvent(QDragLeaveEvent *event)
{
    highlightedRect = QRect();
    update();
    event->accept();
}

void tileset::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat(getMimeType())
        && findPiece(targetSquare(event->pos())) == -1) {

        highlightedRect = targetSquare(event->pos());
        event->setDropAction(Qt::MoveAction);
        event->accept();
    } else {
        highlightedRect = QRect();
        event->ignore();
    }

    update();
}

void tileset::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasFormat(getMimeType())
        && findPiece(targetSquare(event->pos())) == -1) {

        QByteArray pieceData = event->mimeData()->data(getMimeType());
        QDataStream stream(&pieceData, QIODevice::ReadOnly);
        QRect square = targetSquare(event->pos());
        int objID;
        stream >> objID;

        //QPixmap scaledPix = pixmap.scaled(m_baseSize, m_baseSize,Qt::KeepAspectRatio);
        QPixmap scaledPix = getScaledPixmapById(objID);

        piecePixmaps.append(scaledPix);
        pieceRects.append(square);
        pieceID.append(objID);

        highlightedRect = QRect();
        update();

        event->setDropAction(Qt::MoveAction);
        event->accept();

    } else {
        highlightedRect = QRect();
        event->ignore();
    }
}

void tileset::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton){
        if(m_editMode){
            piecePixmaps.removeAt(findPiece(targetSquare(event->pos())));
            pieceID.removeAt(findPiece(targetSquare(event->pos())));
            pieceRects.removeAt(findPiece(targetSquare(event->pos())));
            update();
            return;
        }
        return;
    }

    QRect square = targetSquare(event->pos());
    int found = findPiece(square);

    if (found == -1)
        return;

    if (!m_editMode){
        emit clickedItem(m_type, pieceID[found]);
        return;
    }

    QPixmap pixmap = piecePixmaps[found];
    int objID = pieceID[found];
    piecePixmaps.removeAt(found);
    pieceRects.removeAt(found);
    pieceID.removeAt(found);
    update();

    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);

    dataStream << objID;

    QMimeData *mimeData = new QMimeData;
    mimeData->setData(getMimeType(), itemData);

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setHotSpot(event->pos() - square.topLeft());
    drag->setPixmap(pixmap);

    if (drag->start(Qt::MoveAction) == 0) {
        piecePixmaps.insert(found, pixmap);
        pieceRects.insert(found, square);
        pieceID.insert(found, objID);
        update();
    }
}

int tileset::findPiece(const QRect &pieceRect) const
{
    for (int i = 0; i < pieceRects.size(); ++i) {
        if (pieceRect == pieceRects[i])
            return i;
    }
    return -1;
}

const QRect tileset::targetSquare(const QPoint &position) const
{
    return QRect(position.x()/getBaseSize() * getBaseSize(), position.y()/getBaseSize() * getBaseSize(), getBaseSize(), getBaseSize());
}

QPixmap tileset::getScaledPixmapById(const unsigned int &id) const
{
    switch (m_type) {
    case ItemTypes::LVL_Block:
    {
        long tarIndex = m_conf->getBlockI(id);
        if(tarIndex==-1)
        {
            QPixmap xxx = QPixmap(m_baseSize, m_baseSize);
            xxx.fill(Qt::red);
            return xxx;
        }
        return GraphicsHelps::squareImage(
                    Items::getItemGFX(m_type, m_conf->main_block[tarIndex].id, false, NULL, scn),
                    QSize(m_baseSize,m_baseSize));
        break;
    }
    case ItemTypes::LVL_BGO:
    {
        long tarIndex = m_conf->getBgoI(id);
        if(tarIndex==-1)
            return QPixmap(m_baseSize, m_baseSize);
        return GraphicsHelps::squareImage(
                    Items::getItemGFX(m_type, m_conf->main_bgo[tarIndex].id, false, NULL, scn),
                    QSize(m_baseSize,m_baseSize));
        break;
    }
    case ItemTypes::LVL_NPC:
    {
        long tarIndex = m_conf->getNpcI(id);
        if(tarIndex==-1)
            return QPixmap(m_baseSize, m_baseSize);
        return GraphicsHelps::squareImage(
                    Items::getItemGFX(m_type, m_conf->main_npc[tarIndex].id, false, NULL, scn),
                    QSize(m_baseSize,m_baseSize));
        break;
    }
    case ItemTypes::WLD_Tile:
    {
        long tarIndex = m_conf->getTileI(id);
        if(tarIndex==-1)
            return QPixmap(m_baseSize, m_baseSize);
        return Items::getItemGFX(m_type, m_conf->main_wtiles[tarIndex].id, false, NULL, scn)
                .scaled(m_baseSize,m_baseSize,Qt::KeepAspectRatio);
        break;
    }
    case ItemTypes::WLD_Path:
    {
        long tarIndex = m_conf->getPathI(id);
        if(tarIndex==-1)
            return QPixmap(m_baseSize, m_baseSize);
        return Items::getItemGFX(m_type, m_conf->main_wpaths[tarIndex].id, false, NULL, scn)
                .scaled(m_baseSize,m_baseSize,Qt::KeepAspectRatio);
        break;
    }
    case ItemTypes::WLD_Scenery:
    {
        long tarIndex = m_conf->getSceneI(id);
        if(tarIndex==-1)
            return QPixmap(m_baseSize, m_baseSize);
        return GraphicsHelps::squareImage(
                    Items::getItemGFX(m_type, m_conf->main_wscene[tarIndex].id, false, NULL, scn),
                    QSize(m_baseSize,m_baseSize));
        break;
    }
    case ItemTypes::WLD_Level:
    {
        long tarIndex = m_conf->getWLevelI(id);
        if(tarIndex==-1)
            return QPixmap(m_baseSize, m_baseSize);
        return GraphicsHelps::squareImage(
                    Items::getItemGFX(m_type, m_conf->main_wlevels[tarIndex].id, false, NULL, scn),
                    QSize(m_baseSize,m_baseSize));
        break;
    }
    default:
        break;
    }
    return QPixmap(m_baseSize, m_baseSize);
}

QString tileset::getMimeType()
{
    switch (m_type) {
    case ItemTypes::LVL_Block: return QString("text/x-pge-piece-block");
    case ItemTypes::LVL_BGO: return QString("text/x-pge-piece-bgo");
    case ItemTypes::LVL_NPC: return QString("text/x-pge-piece-npc");
    case ItemTypes::WLD_Tile: return QString("text/x-pge-piece-tile");
    case ItemTypes::WLD_Path: return QString("text/x-pge-piece-path");
    case ItemTypes::WLD_Scenery: return QString("text/x-pge-piece-scenery");
    case ItemTypes::WLD_Level: return QString("text/x-pge-piece-level");
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
    for(int i = 0; i < pieceRects.size(); ++i){
        SimpleTilesetItem it;
        it.col = pieceRects[i].x() / m_baseSize;
        it.row = pieceRects[i].y() / m_baseSize;
        it.id = pieceID[i];
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
    for(int i = 0; i < tileset.items.size(); ++i){
        piecePixmaps.append(getScaledPixmapById(tileset.items[i].id));
        pieceRects.append(QRect(tileset.items[i].col*m_baseSize, tileset.items[i].row*m_baseSize, m_baseSize, m_baseSize));
        pieceID.append(tileset.items[i].id);
    }
}

void tileset::SaveSimpleTileset(const QString &path, const SimpleTileset &tileset)
{
    QString modifiedPath;
    modifiedPath = path;
    QSettings simpleTilesetINI(modifiedPath,QSettings::IniFormat);
    simpleTilesetINI.setIniCodec("UTF-8");
    simpleTilesetINI.clear();
    simpleTilesetINI.beginGroup("tileset"); //HEADER
    simpleTilesetINI.setValue("rows",tileset.rows);
    simpleTilesetINI.setValue("cols",tileset.cols);
    simpleTilesetINI.setValue("name",tileset.tileSetName);
    simpleTilesetINI.setValue("type",static_cast<int>(tileset.type));
    simpleTilesetINI.endGroup();
    for(int i = 0; i < tileset.items.size(); ++i){
        simpleTilesetINI.beginGroup(QString("item-%1-%2").arg(tileset.items[i].col).arg(tileset.items[i].row));
        simpleTilesetINI.setValue("id",tileset.items[i].id);
        simpleTilesetINI.endGroup();
    }
}

bool tileset::OpenSimpleTileset(const QString &path, SimpleTileset &tileset)
{
    QSettings simpleTilesetINI(path,QSettings::IniFormat);
    simpleTilesetINI.setIniCodec("UTF-8");
    QStringList groups = simpleTilesetINI.childGroups();
    int tilesetindex;
    if((tilesetindex = groups.indexOf("tileset"))!=-1){
        simpleTilesetINI.beginGroup("tileset");
        tileset.rows = (unsigned int)simpleTilesetINI.value("rows",3).toInt();
        tileset.cols = (unsigned int)simpleTilesetINI.value("cols",3).toInt();
        tileset.type = simpleTilesetINI.value("type",0).toInt();
        tileset.tileSetName = simpleTilesetINI.value("name", "").toString();
        tileset.fileName = QFileInfo(path).fileName();
        simpleTilesetINI.endGroup();
        groups.removeAt(tilesetindex);
        for(int i = 0; i < groups.size(); ++i){
            QString gr = groups[i];
            if(gr.startsWith("item-")){
                QStringList spData = gr.split("-");
                if(spData.size() == 3){
                    bool succ = false;
                    SimpleTilesetItem i;
                    i.col = spData[1].toInt(&succ);
                    if(!succ)
                        return false;
                    i.row = spData[2].toInt(&succ);
                    if(!succ)
                        return false;
                    simpleTilesetINI.beginGroup(gr);
                    i.id = (unsigned int)simpleTilesetINI.value("id",0).toInt();
                    simpleTilesetINI.endGroup();
                    tileset.items << i;
                }else{
                    return false;
                }
            }else{
                return false;
            }
        }
    }else{
        return false;
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
    setMaximumSize(QSize(m_baseSize*m_cols, m_baseSize*m_rows));
    setMinimumSize(QSize(m_baseSize*m_cols, m_baseSize*m_rows));
    removeOuterItems(QRect(0,0,m_baseSize*m_cols,m_baseSize*m_rows));
}

void tileset::removeOuterItems(QRect updatedRect)
{
    if(pieceRects.size()==0)
        return;

    QList<QRect> rmRects;
    QList<QPixmap> rmPixm;
    QList<int> rmId;

    for(int i = 0; i < pieceRects.size(); ++i){
        if(!((updatedRect+QMargins(1,1,1,1)).contains(pieceRects[i],true))){
            rmRects << pieceRects[i];
            rmPixm << piecePixmaps[i];
            rmId << pieceID[i];
        }
    }

    for(int i = 0; i < rmRects.size(); ++i){
        int l = pieceRects.indexOf(rmRects[i]);
        pieceRects.removeAt(l);
        piecePixmaps.removeAt(l);
        pieceID.removeAt(l);
    }
}
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

