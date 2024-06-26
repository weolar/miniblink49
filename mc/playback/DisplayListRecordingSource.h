#ifndef DisplayListRecordingSource_h
#define DisplayListRecordingSource_h

namespace cc {

class RecordingSourceContainer {
public:
    SkPictureRecorder* recorder;
    IntRect area;
    bool dirty;

    RecordingSourceContainer(const IntRect& area)
        : recorder(0)
        , area(area)
        , dirty(true)
    {}

    ~RecordingSourceContainer();
};

// 和老版的PicturePile一个意思
class DisplayListRecordingSource {

public:
    DisplayListRecordingSource();

    // Used by WebViewCore
    void invalidate(const IntRect& dirtyRect);
    void applyDirtyRects();
    void setSize(const IntSize& size);

    void updateRecordingSourcesIfNeeded(WebLayerImplClient* client, canDrawRect);
    void updateRecordingSources(WebLayerImplClient* client);

private:
    Vector<IntRect> m_dirtyRects;
    blink::IntSize m_size; // 一般比屏幕大点，不完全是layer size，一定等于若干个tile的组合size
    Vector<RecordingSourceContainer> m_recordingSourcePile;
};

}

#endif // DisplayListRecordingSource_h