
#ifndef content_browser_NavigationController_h
#define content_browser_NavigationController_h

#include "third_party/WebKit/public/web/WebHistoryItem.h"
#include "third_party/WebKit/public/web/WebHistoryCommitType.h"

#include "third_party/WebKit/Source/wtf/text/WTFString.h"
#include "third_party/WebKit/Source/wtf/Vector.h"

namespace blink {
class WebLocalFrame;
}

namespace content {

class WebPageImpl;

class NavigationController {
public:
    NavigationController(WebPageImpl* page);
    ~NavigationController();

    int historyBackListCount();
    int historyForwardListCount();
    void navigateBackForwardSoon(int offset);
    
    void insertOrReplaceEntry(const blink::WebHistoryItem& item, blink::WebHistoryCommitType type);
    
private:
    void navigate(int offset);
    int findEntry(const blink::WebHistoryItem& item) const;

    WebPageImpl* m_page;
    int m_currentOffset;
    Vector<blink::WebHistoryItem> m_items;
};

}

#endif // content_browser_NavigationController_h