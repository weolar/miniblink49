#ifndef PlatformMouseEventWin_h
#define PlatformMouseEventWin_h

#include "platform/PlatformMouseEvent.h"

namespace blink {

class PlatformMouseEventWin : public PlatformMouseEvent {
public:
    PlatformMouseEventWin(HWND, UINT, WPARAM, LPARAM, bool didActivateWebView = false);
    void setClickCount(int count) { m_clickCount = count; }
    bool didActivateWebView() const { return m_didActivateWebView; }

protected:
    bool m_didActivateWebView;
    PlatformEvent::Type m_eventType;

    bool m_shiftKey;
    bool m_ctrlKey;
    bool m_altKey;
    bool m_metaKey;
    double m_timestamp; // unit: seconds
    unsigned m_modifierFlags;
};

} // namespace blink

#endif // PlatformMouseEventWin_h