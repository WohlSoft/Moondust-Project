#ifndef WLD_TILEBOX_H
#define WLD_TILEBOX_H

#include <QHash>
#include <QList>
#include <unordered_map>
#include <PGE_File_Formats/wld_filedata.h>

class WorldNode
{
public:
    enum nodeType
    {
        unknown=0,
        tile,
        scenery,
        path,
        level,
        musicbox
    };

    WorldNode()
    {
        x=0;
        y=0;
        w=32;
        h=32;
        r=0.f;
        g=0.f;
        b=1.f;
        Z=0.0;
        type=unknown;
    }

    virtual ~WorldNode() {}

    WorldNode(const WorldNode &xx)
    {
        x=xx.x;
        y=xx.y;
        w=xx.w;
        h=xx.h;
        r=xx.r;
        g=xx.g;
        b=xx.b;
        Z=xx.Z;
        type=xx.type;
    }
    int type;
    long x;
    long y;
    long w;
    long h;
    float r;
    float g;
    float b;
    double Z;
};

class WldTileItem: public WorldNode
{
public:
    WldTileItem(WorldTiles _data): WorldNode()
    {
        data = _data;
        x=data.x;
        y=data.y;
        Z=0.0+(double(data.array_id)*0.0000001);
        type=tile;
    }
    WldTileItem(const WldTileItem &x): WorldNode(x)
    {
        data = x.data;
        type=tile;
    }
    ~WldTileItem()
    {}

    WorldTiles data;
};

class WldSceneryItem: public WorldNode
{
public:
    WldSceneryItem(WorldScenery _data): WorldNode()
    {
        data = _data;
        x=data.x;
        y=data.y;
        w = 16;
        h = 16;
        Z=10.0+(double(data.array_id)*0.0000001);
        vizible=true;
        type=scenery;
    }
    WldSceneryItem(const WldSceneryItem &x): WorldNode(x)
    {
        data = x.data;
        vizible=x.vizible;
        type=scenery;
    }
    ~WldSceneryItem()
    {}
    WorldScenery data;
    bool vizible;
};

class WldPathItem: public WorldNode
{
public:
    WldPathItem(WorldPaths _data): WorldNode()
    {
        data = _data;
        x=data.x;
        y=data.y;
        Z=20.0+(double(data.array_id)*0.0000001);
        vizible=true;
        type=path;
    }
    WldPathItem(const WldPathItem &x): WorldNode(x)
    {
        data = x.data;
        vizible=x.vizible;
        type=path;
    }
    ~WldPathItem()
    {}
    WorldPaths data;
    bool vizible;
};

class WldLevelItem: public WorldNode
{
public:
    WldLevelItem(WorldLevels _data): WorldNode()
    {
        data = _data;
        x=data.x;
        y=data.y;
        Z=30.0+(double(data.array_id)*0.0000001);
        vizible=true;
        type=level;
    }
    WldLevelItem(const WldLevelItem &x): WorldNode(x)
    {
        data = x.data;
        vizible=x.vizible;
        type=level;
    }
    ~WldLevelItem()
    {}
    WorldLevels data;
    bool vizible;
};

class WldMusicBoxItem: public WorldNode
{
public:
    WldMusicBoxItem(WorldMusic _data): WorldNode()
    {
        data = _data;
        x=data.x;
        y=data.y;
        Z=-10.0;
        type=musicbox;
    }
    WldMusicBoxItem(const WldMusicBoxItem &x): WorldNode(x)
    {
        data = x.data;
        type=musicbox;
    }
    ~WldMusicBoxItem()
    {}
    WorldMusic data;
};

class TileBox
{
public:
    TileBox()
    {
        gridSize=32;
        gridSize_h=16;
    }

    TileBox(unsigned long size)
    {
        gridSize=size;
        gridSize_h=size/2;
    }

    ~TileBox()
    {
        clean();
    }

    void addNode(long X, long Y, long W, long H, WorldNode* item)
    {
        for(long i=X; i<X+W; i+=gridSize)
        {
            for(long j=Y; j<Y+H; j+=gridSize)
            {
                QPoint t = applyGrid(i,j);
                map[t.x()][t.y()].push_back(item);
            }
        }
    }

    QList<WorldNode* > query(long X, long Y)
    {
        QList<WorldNode * > list;

        QPoint t = applyGrid(X,Y);
        long listI = t.x();
        long listJ = t.y();

        const typename std::unordered_map<long, std::unordered_map<long, QList<WorldNode* > > >::const_iterator got = map.find(listI);
        if(got != map.end())
        {
            const typename std::unordered_map<long, QList<WorldNode* > >::const_iterator got2 = map[listI].find(listJ);
            if(got2 != map[listI].end())
                list.append(map[listI][listJ]);
        }
        return list;
    }

    QList<WorldNode* > query(long Left, long Top, long Right, long Bottom, bool z_sort=false)
    {
        QList<WorldNode * > list;
        for(long i=Left-gridSize; i<Right+gridSize*2; i+=gridSize)
        {
            for(long j=Top-gridSize; j<Bottom+gridSize*2; j+=gridSize)
            {
                QPoint t = applyGrid(i,j);
                long listI = t.x();
                long listJ = t.y();

                const typename std::unordered_map<long, std::unordered_map<long, QList<WorldNode* > > >::const_iterator got = map.find(listI);
                if(got != map.end())
                {
                    const typename std::unordered_map<long, QList<WorldNode* > >::const_iterator got2 = map[listI].find(listJ);
                    if(got2 != map[listI].end())
                        list.append(map[listI][listJ]);
                }
            }
        }

        if(z_sort)
        {
            //Sort array
            int total = list.size();
            long i;
            double ymin;
            long ymini;
            long sorted = 0;

            while(sorted < list.size())
            {
                ymin = list[sorted]->Z;
                ymini = sorted;

                for(i = sorted; i < total; i++)
                {
                    if( list[i]->Z < ymin )
                    {
                        ymin = list[i]->Z; ymini = i;
                    }
                }
                list.swap(ymini, sorted);
                sorted++;
            }
        }

        return list;
    }

    void clean()
    {
        map.clear();
    }

    QPoint applyGrid(long x, long y)
    {
        QPoint source;
        source.setX(x);
        source.setY(y);
        int gridX, gridY;
        if(gridSize>0)
        { //ATTACH TO GRID

            if((int)source.x()<0)
            {
                gridX=(int)source.x()+1+(abs((int)source.x()+1)%gridSize)-gridSize;
            }
            else
            {
                gridX=((int)source.x()-(int)source.x() % gridSize);
            }

            if((int)source.y()<0)
            {
                gridY = (int)source.y()+1+(abs((int)source.y()+1) % gridSize)-gridSize;
            }
            else
            {
                gridY = ((int)source.y() - (int)source.y() % gridSize);
            }

            return QPoint(gridX, gridY);
        }
        else
            return source;
    }

    std::unordered_map<long, std::unordered_map<long, QList<WorldNode* > > > map;
    long gridSize;
    long gridSize_h;
};


#endif // WLD_TILEBOX_H

