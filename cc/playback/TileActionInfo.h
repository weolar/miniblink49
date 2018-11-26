
#ifndef cc_playback_TileActionInfo_h
#define cc_playback_TileActionInfo_h

#include "third_party/skia/include/core/SkColor.h"
#include "third_party/WebKit/Source/wtf/Vector.h"

class SkBitmap;

namespace cc {
    
struct TileActionInfo {
    TileActionInfo(int index, int xIndex, int yIndex);
    ~TileActionInfo();

    int index;
    int xIndex;
    int yIndex;

    bool m_isSolidColorCoverWholeTile;
    SkColor* m_solidColor;
    SkBitmap* m_bitmap;
};

class TileActionInfoVector {
public:
    void append(int index, int xIndex, int yIndex)
    {
        m_infos.append(new TileActionInfo(index, xIndex, yIndex));
    }

    size_t size() const { return m_infos.size(); }
    const WTF::Vector<TileActionInfo*>& infos() const { return m_infos; }

    ~TileActionInfoVector()
    {
        for (size_t i = 0; i < m_infos.size(); ++i) {
            TileActionInfo* info = m_infos[i];
            delete info;
        }
    }
private:
    WTF::Vector<TileActionInfo*> m_infos;
};

}

#endif // cc_playback_TileActionInfo_h
