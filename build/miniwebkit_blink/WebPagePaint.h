#ifndef WebPagePaint_h
#define WebPagePaint_h

#include "cc/trees/LayerTreeHost.h"
#include "base/rand_util.h"

namespace cc {
class LayerTreeHost;
}

namespace blink {

class WebPagePaint {
public:
    WebPagePaint()
    {
        m_pagePtr = 0;
        m_bdColor = RGB(199, 237, 204) | 0xff000000;
        m_memoryCanvas = nullptr;
        m_needsCommit = true;
        m_layerTreeHost = nullptr;
        m_lastFrameTimeMonotonic = 0;
        m_webViewImpl = nullptr;

        m_mouseInWindow = false;
        m_dragging = false;

        m_debugCount = 0;
    }

    ~WebPagePaint()
    {
        m_dirtyCanvas = nullptr;
        m_memoryCanvas = nullptr;

        m_pagePtr = 0;
    }

    void init(WebPage* pagePtr)
    {
        LONG windowStyle = GetWindowLongPtr(pagePtr->getHWND(), GWL_EXSTYLE);
        m_useLayeredBuffer = !!((windowStyle)& WS_EX_LAYERED);

        m_canScheduleResourceLoader = false;

        m_scheduleMessageCount = 0;
        m_postpaintMessageCount = 0;

        m_hasResize = false;
        m_postMouseLeave = false;

        m_pagePtr = pagePtr;

        m_bNeedCallXmlHaveFinished = false;

        m_layerTreeHost = new cc::LayerTreeHost(pagePtr);
    }

    void clearPaintWhenLayeredWindow(skia::PlatformCanvas* canvas, const IntRect& rect)
    {
        if (!m_useLayeredBuffer)
            return;

        // When using transparency mode clear the rectangle before painting.
        SkPaint clearPaint;
        clearPaint.setARGB(0, 0, 0, 0);
        clearPaint.setXfermodeMode(SkXfermode::kClear_Mode);

        SkRect skrc;
        skrc.set(rect.x(), rect.y(), rect.x() + rect.width(), rect.y() + rect.height());
        canvas->drawRect(skrc, clearPaint);
    }

    // Merge any areas that would reduce the total area
    void mergeDirtyList() { while (doMergeDirtyList(true)) {}; }

    bool doMergeDirtyList(bool forceMerge)
    {
        int nDirty = (int)m_paintMessageQueue.size();
        if (nDirty < 1) {
            return false;
        }

        int bestDelta = forceMerge ? 0x7FFFFFFF : 0;
        int mergeA = 0;
        int mergeB = 0;
        for (int i = 0; i < nDirty - 1; i++) {
            for (int j = i + 1; j < nDirty; j++) {
                int delta = intUnionArea(&m_paintMessageQueue[i], &m_paintMessageQueue[j]) -
                    intRectArea(&m_paintMessageQueue[i]) - intRectArea(&m_paintMessageQueue[j]);
                if (bestDelta > delta) {
                    mergeA = i;
                    mergeB = j;
                    bestDelta = delta;
                }
            }
        }

        if (mergeA != mergeB) {
            m_paintMessageQueue[mergeA].unite(m_paintMessageQueue[mergeB]);
            for (int i = mergeB + 1; i < nDirty; i++)
                m_paintMessageQueue[i - 1] = m_paintMessageQueue[i];

            m_paintMessageQueue.removeLast();
            return true;
        }

        return false;
    }

    void postPaintMessage(const IntRect* paintRect)
    {
        if (!paintRect || paintRect->isEmpty())
            return;

        m_postpaintMessageCount++;

        if (m_paintMessageQueue.size() > m_paintMessageQueueSize && 0 != m_scheduleMessageCount)
            return; // 从SchedulePaintEvent发送过来的

        if (m_paintMessageQueue.size() > m_paintMessageQueueSize) {
            IntRect destroyRect = m_paintMessageQueue[0];
            m_paintMessageQueue.remove(0);
        }

        // TODO 脏矩形合并
        for (int i = 0; i < (int)m_paintMessageQueue.size(); ++i) {
            IntRect* paintRectFromQueue = &m_paintMessageQueue[i];
            if (paintRectFromQueue == paintRect)
                paintRectFromQueue->setWidth(0);
        }

        m_paintMessageQueue.append(*paintRect);

        m_postpaintMessageCount--;
    }

    void testPaint()
    {
        for (size_t index = 0; index < m_paintMessageQueue.size(); ++index) {
            IntRect* paintRect = &m_paintMessageQueue[index];
            WCHAR msg[100] = { 0 };
            swprintf(msg, L"testPaint: %d %d %x\n", paintRect->y(), paintRect->height(), index);
            OutputDebugStringW(msg);
        }
    }

    void beginMainFrame()
    {
        double lastFrameTimeMonotonic = WTF::currentTime();

        if (needsCommit()) {
            clearNeedsCommit();

            double lastFrameTimeMonotonic = WTF::currentTime();

            WebBeginFrameArgs frameTime(m_lastFrameTimeMonotonic, 0, lastFrameTimeMonotonic - m_lastFrameTimeMonotonic);
            m_webViewImpl->beginFrame(frameTime);
            m_webViewImpl->layout();
        }

        schedulePaintEvent();

        m_lastFrameTimeMonotonic = lastFrameTimeMonotonic;
    }

    void schedulePaintEvent()
    {
        if (0 != m_scheduleMessageCount)
            notImplemented();

        if (!m_pagePtr->getHWND())
            return;

        m_scheduleMessageCount++;

        HDC hdc = ::GetDC(m_pagePtr->getHWND());

        IntRect lastTimeRect;
        bool loopAgain = false;
        do {
            loopAgain = false;
            lastTimeRect.setSize(IntSize());
            mergeDirtyList();
            int queueSize = (int)m_paintMessageQueue.size();
            for (int i = 0; i < queueSize; ++i) {
                IntRect* paintRect = &m_paintMessageQueue[i];
                if (lastTimeRect == *paintRect || lastTimeRect.contains(*paintRect))
                    continue;

                lastTimeRect = *paintRect;
                //fastFree(paintRect);
                if (!lastTimeRect.isEmpty()) { // 这里可能会重入postPaintMessage，所以需要重新合并脏矩形，并且小心一些bug
                    doPaint(hdc, &lastTimeRect);
                    
                    // 这里还有优化空间，可以把很短间隔的差距不大的矩形忽略掉
                    if (queueSize != (int)m_paintMessageQueue.size()) { // 如果在绘制的时候被请求了脏矩形，则重新合并
                        m_paintMessageQueue.remove(0, i + 1);
                        loopAgain = true;
                        break;
                    }
                }
            }

        } while (loopAgain);
        ::ReleaseDC(m_pagePtr->getHWND(), hdc);

        m_paintMessageQueue.clear();
        m_scheduleMessageCount--;
    }

#if 0
    void doPaintUseLayeredBuffer(HDC psHdc, const IntRect* paintRect)
    {
        RECT rcPaint = { 0, 0, 500, 500 };
        m_paintRect = *paintRect;

        if (!m_dirtyCanvas || m_hasResize) {
            m_paintRect = m_clientRect;
            m_dirtyCanvas = (new skia::PlatformCanvas(m_clientRect.width(), m_clientRect.height()/*, !m_useLayeredBuffer*/));
            m_memoryCanvas = (new skia::PlatformCanvas(m_clientRect.width(), m_clientRect.height()/*, !m_useLayeredBuffer*/));
            m_hasResize = false;
        }

        if (m_paintRect.isEmpty())
            return;

#if QueryPerformance
        LARGE_INTEGER now0;
        LARGE_INTEGER now1 = { 0 };
        QueryPerformanceCounter(&now0);
#endif

       clearPaintWhenLayeredWindow(m_dirtyCanvas, m_paintRect);
       clearPaintWhenLayeredWindow(m_memoryCanvas, m_paintRect);

       //layout();
       updateLayers(m_paintRect, m_dirtyCanvas);

       HDC hMemoryDC = skia::BeginPlatformPaint(m_memoryCanvas);

       bool bNeedContinue = true;
       void* pCallBackContext = 0;
       void* pKdGuiForeignPtr = m_pagePtr->getKdGuiObjPtr() ? m_pagePtr->getKdGuiObjPtr()->pForeignPtr : 0;
       if (m_pagePtr->m_callbacks.m_paint) {
           //             m_pagePtr->m_callbacks.m_paint(m_pagePtr,
           //                 pKdGuiForeignPtr, m_pagePtr->getForeignPtr(), m_pagePtr->getHWND(),
           //                 KDPPaintStepPrePaintToMemoryDC, &pCallBackContext, &bNeedContinue, &(RECT)m_paintRect, hMemoryDC, psHdc);
       }
       if (!bNeedContinue) {
           goto Exit0;
       }

       // 先把脏矩形绘制到内存dc上
       skia::DrawToNativeContext(m_dirtyCanvas, hMemoryDC, m_paintRect.x(), m_paintRect.y(), &intRectToWinRect(m_paintRect));

       if (!bNeedContinue) {
           goto Exit0;
       }

       if (m_useLayeredBuffer) { // 再把内存dc画到psHdc上
           RECT rtWnd;
           ::GetWindowRect(m_pagePtr->getHWND(), &rtWnd);
           m_winodwRect = winRectToIntRect(rtWnd);

           //skia::DrawToNativeLayeredContext(m_memoryCanvas.get(), psHdc, m_winodwRect.x(), m_winodwRect.y(), &((RECT)m_clientRect));
       }
       else {
           skia::DrawToNativeContext(m_memoryCanvas, psHdc, 0, 0, &intRectToWinRect(m_clientRect));
       }

       if (m_bNeedCallXmlHaveFinished && m_pagePtr->m_callbacks.m_xmlHaveFinished) {
           m_bNeedCallXmlHaveFinished = false;
           //m_pagePtr->m_callbacks.m_xmlHaveFinished(m_pagePtr, pKdGuiForeignPtr, m_pagePtr->getForeignPtr(), m_pagePtr->getHWND());
       }

       // QueryPerformanceCounter(&now1);
       WCHAR msg[200] = { 0 };
       //wsprintfW(msg, L"Rectangle : %d \n", now1.LowPart - now0.LowPart);
       wsprintfW(msg, L"Rectangle : %d %d %d %d\n", m_paintRect.x(), m_paintRect.y(), m_paintRect.width(), m_paintRect.height());
       OutputDebugStringW(msg);

#if 1
       //         HGDIOBJ hOldBr = SelectObject(psHdc, GetStockObject(NULL_BRUSH));
       //         Rectangle(psHdc, m_paintRect.x(), m_paintRect.y(), m_paintRect.maxX(), m_paintRect.maxY());
       //         SelectObject(psHdc, hOldBr);

       //         HBRUSH hbrush = CreateSolidBrush(RGB(19, 237, 04));
       //         FillRect(psHdc, &(RECT)m_paintRect, hbrush);
       //         DeleteObject(hbrush);
#endif
    Exit0:
        skia::EndPlatformPaint(m_memoryCanvas);
    }
#endif // #if 0

    void doPaint(HDC hdc, const IntRect* paintRect)
    {
        RECT rcPaint = { 0, 0, 500, 500 };
        m_paintRect = *paintRect;

        bool needsFullTreeSync = false;
        if (!m_memoryCanvas || m_hasResize) {
            m_hasResize = false;
            needsFullTreeSync = true;
            m_paintRect = m_clientRect;

            if (m_memoryCanvas)
                delete m_memoryCanvas;
            m_memoryCanvas = skia::CreatePlatformCanvas(m_clientRect.width(), m_clientRect.height(), !m_useLayeredBuffer);
        }

        m_paintRect.intersect(m_clientRect);
        if (m_paintRect.isEmpty())
            return;

#if QueryPerformance
        LARGE_INTEGER now0;
        LARGE_INTEGER now1 = { 0 };
        QueryPerformanceCounter(&now0);
#endif
        clearPaintWhenLayeredWindow(m_memoryCanvas, m_paintRect);

        HDC hMemoryDC = skia::BeginPlatformPaint(m_memoryCanvas);

        updateLayers(m_paintRect, m_memoryCanvas, needsFullTreeSync); // 绘制脏矩形

        if (m_useLayeredBuffer) { // 再把内存dc画到hdc上
            RECT rtWnd;
            ::GetWindowRect(m_pagePtr->getHWND(), &rtWnd);
            m_winodwRect = winRectToIntRect(rtWnd);
            //skia::DrawToNativeLayeredContext(m_memoryCanvas.get(), hdc, m_winodwRect.x(), m_winodwRect.y(), &((RECT)m_clientRect));
        } else {
            //drawDebugLine(m_memoryCanvas, m_paintRect);

            //////////////////////////////////////////////////////////////////////////
            //const SkBitmap& bitmap = m_memoryCanvas->getTopDevice()->accessBitmap(false);
            //SkColor color = bitmap.getColor(17, 35);
            //////////////////////////////////////////////////////////////////////////
            skia::DrawToNativeContext(m_memoryCanvas, hdc, m_paintRect.x(), m_paintRect.y(), &intRectToWinRect(m_paintRect));
        }

        //::Rectangle(hdc, m_paintRect.x(), m_paintRect.y(), m_paintRect.maxX(), m_paintRect.maxY());
        //::Rectangle(hdc, 220, 40, 366, 266);
       
        skia::EndPlatformPaint(m_memoryCanvas);
    }

    int m_debugCount;

    void drawDebugLine(skia::PlatformCanvas* memoryCanvas, const IntRect& paintRect)
    {
        m_debugCount++;

        WCHAR msg[200] = { 0 };
        wsprintfW(msg, L"drawDebugLine : %d %d %d %d   | %d\n\n", m_paintRect.x(), m_paintRect.y(), m_paintRect.width(), m_paintRect.height(), m_debugCount);
        OutputDebugStringW(msg);

        OwnPtr<GraphicsContext> context = GraphicsContext::deprecatedCreateWithCanvas(memoryCanvas, GraphicsContext::NothingDisabled);

        context->setStrokeStyle(SolidStroke);
        context->setStrokeColor(0xff000000 | (::GetTickCount() + base::RandInt(0, 0x1223345)));
        context->drawLine(IntPoint(paintRect.x(), paintRect.y()), IntPoint(paintRect.maxX(), paintRect.maxY()));
        context->drawLine(IntPoint(paintRect.maxX(), paintRect.y()), IntPoint(paintRect.x(), paintRect.maxY()));

        context->strokeRect(paintRect, 2);
    }

    void updateLayers2(const IntRect& dirtyRect, skia::PlatformCanvas* canvas, bool needsFullTreeSync)
    {
        if (dirtyRect.isEmpty() || !m_layerTreeHost)
            return;

        setPainting(true);

        skia::PlatformCanvas* dirtyCanvas = skia::CreatePlatformCanvas(dirtyRect.width(), dirtyRect.height(), true);
        m_layerTreeHost->updateLayers(dirtyCanvas, dirtyRect, needsFullTreeSync);
        const SkBitmap& bitmap = dirtyCanvas->getTopDevice()->accessBitmap(false);

        if (m_debugCount < 15)
            canvas->drawBitmap(bitmap, dirtyRect.x(), dirtyRect.y());

        setPainting(false);
    }

    void updateLayers(const IntRect& dirtyRect, skia::PlatformCanvas* canvas, bool needsFullTreeSync)
    {
        if (dirtyRect.isEmpty() || !m_layerTreeHost)
            return;

        setPainting(true);

        SkPaint paint;
        canvas->save();
        canvas->clipRect(dirtyRect);

        m_layerTreeHost->updateLayers(canvas, dirtyRect, needsFullTreeSync);

        canvas->restore();
        setPainting(false);
    }

    void paintEvent(HDC hdc, const RECT* paint_rect)
    {
        if (!m_memoryCanvas || m_clientRect.isEmpty())
            return;

//         SkPaint myPaint;
//         myPaint.setColor(0xff11ff33);
//         myPaint.setXfermodeMode(SkXfermode::kSrc_Mode);
//         m_memoryCanvas->drawRect(blink::IntRect(150, 100, 100, 100), myPaint); // weolar

        skia::DrawToNativeContext(m_memoryCanvas, hdc, 0, 0, &intRectToWinRect(m_clientRect));
        //::Rectangle(hdc, 20, 20, 266, 166);
    }

    void setPainting(bool value) { m_painting = value; }

    FrameView* frameView() const
    {
        return m_pagePtr->localFrame() ? m_pagePtr->localFrame()->view() : NULL;
    }

    bool needsCommit() { return m_needsCommit; }
    void setNeedsCommit() { m_needsCommit = true; }
    void clearNeedsCommit() { m_needsCommit = false; }

    cc::LayerTreeHost* layerTreeHost() { return m_layerTreeHost; }

    bool m_useLayeredBuffer;

    IntRect m_winodwRect;
    IntRect m_clientRect; // 方便WebPage修改

    bool m_hasResize;

    bool m_postMouseLeave; // 系统的MouseLeave获取到的鼠标位置不太准确，自己在定时器里再抛一次

    bool m_bNeedCallXmlHaveFinished; // 如果xml已经解析完毕，则需要在渲染完成一张整图后调用，而不是立马调用这个回调

    RGBA32 m_bdColor;

    double m_lastFrameTimeMonotonic;
    WebPage* m_pagePtr;
    WebViewImpl* m_webViewImpl;
    cc::LayerTreeHost* m_layerTreeHost;

    bool m_mouseInWindow;
    bool m_dragging;

protected:
    IntRect m_paintRect;
    skia::PlatformCanvas* m_dirtyCanvas;
    skia::PlatformCanvas* m_memoryCanvas;

    bool m_painting;
    bool m_canScheduleResourceLoader;

    Vector<IntRect> m_paintMessageQueue;
    static const int m_paintMessageQueueSize = 200;

    int m_postpaintMessageCount;
    int m_scheduleMessageCount;

    bool m_needsCommit;
};

} // blink

#endif // WebPagePaint_h