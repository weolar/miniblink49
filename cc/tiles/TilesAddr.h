
#ifndef cc_TilesAddr_h
#define cc_TilesAddr_h

#include "third_party/WebKit/Source/wtf/HashMap.h"
#include "cc/tiles/TileBase.h"

#include <functional>

namespace cc {

class TileGrid;

class TilesAddr {
public:
    typedef HashMap<uint, TileBase*>::iterator iterator;
    iterator begin()
    {
        return m_indexTiles.begin();
    }

    iterator end()
    {
        return m_indexTiles.end();
    }

    TilesAddr(void* parent)
    {
        m_numTileX = 0;
        m_numTileY = 0;
        m_parent = parent;
    }

    ~TilesAddr()
    {
        for (TilesAddr::iterator it = begin(); it != end(); ++it) {
            TileBase* tile = it->value;
            tile->unref(FROM_HERE);
        }
    }

    int getSize() const
    {
        return m_numTileX * m_numTileY;
    }

    bool isInit() const
    {
        return m_numTileX * m_numTileY != 0;
    }

    TileBase* getTileByIndex(int index) const
    {
        if (!isInit())
            return nullptr;

        HashMap<uint, TileBase*>::const_iterator it = m_indexTiles.find(index + m_avoidHashMapEmptyValue);
        if (it == m_indexTiles.end())
            return nullptr;

        return it->value;
    }

    int getIndexByTile(const TileBase* tile) const
    {
        if (!isInit() || m_numTileX <= tile->xIndex() || m_numTileY <= tile->yIndex())
            return -1;
        return tile->xIndex() + m_numTileX * tile->yIndex();
    }

    TileBase* getTileByXY(int xIndex, int yIndex, std::function<TileBase*(void)>&& constructor)
    {
        if (!isInit() || m_numTileX <= xIndex || m_numTileY <= yIndex)
            return nullptr;

        int index = xIndex + m_numTileX * yIndex;

        TileBase* tile = getTileByIndex(index);
        if (tile)
            return tile;

        tile = (constructor())->init(m_parent, xIndex, yIndex);
        bool ok = add(tile);
        ASSERT(ok);
        return tile;
    }

    bool add(TileBase* tile)
    {
        if (!isInit()) {
            DebugBreak();
            return false;
        }
        int index = getIndexByTile(tile);
        if (-1 == index)
            return false;
        m_indexTiles.add(index + m_avoidHashMapEmptyValue, tile);
        return true;
    }

    bool remove(TileBase* tile)
    {
        int index = getIndexByTile(tile);
        if (-1 == index)
            return false;
        m_indexTiles.remove(index + m_avoidHashMapEmptyValue);
        tile->unref(FROM_HERE);
        return true;
    }

    static void realloByNewXY(TilesAddr** selfPtr, int numTileX, int numTileY)
    {
        TilesAddr* self = *selfPtr;
        if (self->m_numTileX == numTileX && self->m_numTileY == numTileY)
            return;

        //         static int xxxxx = 0;
        //         xxxxx++;
        //         String outString = String::format("TileGrid::realloByNewXY: %d\n", xxxxx);
        //         OutputDebugStringW(outString.charactersWithNullTermination().data());
        //         if (7 == xxxxx)
        //             OutputDebugStringW(L"");

        TilesAddr* newTilesAddr = new TilesAddr(self->m_parent);
        newTilesAddr->init(numTileX, numTileY);
        if (0 != numTileX * numTileY) {
            for (TilesAddr::iterator it = self->begin(); it != self->end(); ++it) {
                TileBase* tile = it->value;
                if (newTilesAddr->add(tile))
                    tile->ref(FROM_HERE);
            }
        }

        //copyFrom(*newTilesAddr);
        delete self;
        *selfPtr = newTilesAddr;
        self = *selfPtr;

//         if (self->m_tileGrid->getRegisterTilesSize() != self->m_indexTiles.size()) {
//             for (TilesAddr::iterator it = self->begin(); it != self->end(); ++it) {
//                 TileBase* tile = it->value;
//                 tile = tile;
//             }
//             DebugBreak();
//         }
    }

private:
    void init(int numTileX, int numTileY)
    {
        m_numTileX = numTileX;
        m_numTileY = numTileY;
    }

    static const int m_avoidHashMapEmptyValue = 1;
    int m_numTileX;
    int m_numTileY;
    void* m_parent; // TileGrid or CompositingLayer
    HashMap<uint, TileBase*> m_indexTiles; // 把二维数组转成一维数组后，index表示一维数组的序号
};

}

#endif // cc_TilesAddr_h