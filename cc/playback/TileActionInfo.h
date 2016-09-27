
#ifndef cc_playback_TileActionInfo_h
#define cc_playback_TileActionInfo_h

namespace cc {
    
struct TileActionInfo {
    TileActionInfo(int index, int xIndex, int yIndex)
    {
        this->index = index;
        this->xIndex = xIndex;
        this->yIndex = yIndex;
    }
    int index;
    int xIndex;
    int yIndex;
};

class TileActionInfoVector {
public:
    void append(int index, int xIndex, int yIndex)
    {
        m_infos.append(new TileActionInfo(index, xIndex, yIndex));
    }

    size_t size() const { return m_infos.size(); }
    const Vector<TileActionInfo*>& infos() const { return m_infos; }

    ~TileActionInfoVector()
    {
        for (size_t i = 0; i < m_infos.size(); ++i) {
            TileActionInfo* info = m_infos[i];
            delete info;
        }
    }
private:
    Vector<TileActionInfo*> m_infos;
};


}

#endif // cc_playback_TileActionInfo_h
