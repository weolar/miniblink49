
#include "content/browser/NavigationController.h"

#include "content/browser/WebPage.h"
#include "content/browser/WebPageImpl.h"

#include "third_party/WebKit/Source/platform/weborigin/ReferrerPolicy.h"
#include "third_party/WebKit/Source/platform/weborigin/Referrer.h"
#include "third_party/WebKit/Source/wtf/text/WTFStringUtil.h"
#include "third_party/WebKit/public/platform/Platform.h"
#include "third_party/WebKit/public/platform/WebTraceLocation.h"
#include "third_party/WebKit/public/web/WebSerializedScriptValue.h"

namespace content {

class HistoryEntry : public blink::WebHistoryItem {
public:
    bool m_isSameDocument;
};

NavigationController::NavigationController(WebPageImpl* page)
{
    m_currentOffset = -1;
    m_lastNavDirection = 0;
    m_page = page;
}

NavigationController::~NavigationController()
{
}

int NavigationController::historyBackListCount()
{
    return m_currentOffset;
}

int NavigationController::historyForwardListCount()
{
    int result = m_items.size() - m_currentOffset - 1;
    return result;
}

// We do same-document navigation if going to a different item and if either of the following is true:
// - The other item corresponds to the same document (for history entries created via pushState or fragment changes).
// - The other item corresponds to the same set of documents, including frames (for history entries created via regular navigation)
static bool shouldDoSameDocumentNavigationTo(const HistoryEntry* curItem, const HistoryEntry* otherItem)
{
#if 0
    // 这里先注释调用。重现地址：usertest.sztaizhou.com
    // 一共三个item，如果第一个到第二个item是same，第三个不是，那么如果从
    // 第三个后退到第二个，就会因为same导致页面加载失败
    if (otherItem->m_isSameDocument)
        return true;
#endif

    if (curItem == otherItem)
        return false;

    String curStateObjectString;
    if (!curItem->stateObject().isNull())
        curStateObjectString = curItem->stateObject().toString();

    String otherStateObjectString;
    if (!otherItem->stateObject().isNull())
        otherStateObjectString = otherItem->stateObject().toString();

    if ((!curStateObjectString.isNull() && !curStateObjectString.isEmpty()) || (!otherStateObjectString.isNull() && !otherStateObjectString.isEmpty()))
        return curItem->documentSequenceNumber() == otherItem->documentSequenceNumber();

    blink::KURL curUrl(blink::ParsedURLString, curItem->urlString());
    blink::KURL otherUrl(blink::ParsedURLString, otherItem->urlString());
    if ((curUrl.hasFragmentIdentifier() || otherUrl.hasFragmentIdentifier()) && equalIgnoringFragmentIdentifier(curUrl, otherUrl))
        return curItem->documentSequenceNumber() == otherItem->documentSequenceNumber();

    return false;
}

void NavigationController::navigate(int offset)
{
    int pos = m_currentOffset + offset;
    if (pos < 0 || pos > (int)(m_items.size() - 1))
        return;
    HistoryEntry* item = m_items[pos];
    HistoryEntry* curItem = nullptr;
    if (m_currentOffset >= 0 && m_currentOffset < (int)m_items.size())
        curItem = m_items[m_currentOffset];
    if (!curItem)
        return;

    m_lastNavDirection = offset;

#if 0 // def DEBUG
    OutputDebugStringA("navigate:\n");
    for (size_t i = 0; i < m_items.size(); ++i) {
        HistoryEntry* it = m_items[i];
        String url = it->urlString();
        OutputDebugStringA(url.utf8().data());
        OutputDebugStringA("\n");
    }
    OutputDebugStringA("navigate end\n");
    
#endif // DEBUG
    blink::WebHistoryLoadType type = blink::WebHistoryDifferentDocumentLoad;
    if (shouldDoSameDocumentNavigationTo(curItem, item))
        type = blink::WebHistorySameDocumentLoad;
    m_page->loadHistoryItem(WebPage::kMainFrameId, *item, type, blink::WebURLRequest::UseProtocolCachePolicy);
}

void NavigationController::navigateBackForwardSoon(int offset)
{
    blink::Platform::current()->currentThread()->postTask(FROM_HERE, WTF::bind(&NavigationController::navigate, this, offset));
}

void NavigationController::navigateToIndex(int index)
{
    int offset = m_currentOffset - index;
    navigateBackForwardSoon(offset);
}

int NavigationController::findEntry(const blink::WebHistoryItem& item) const
{
    if (m_lastNavDirection > 0) {
        for (size_t i = m_currentOffset + 1; i < m_items.size(); ++i) {
            if (m_items[i]->urlString() == item.urlString())
                return i;
        }
    } else {
        for (size_t i = m_currentOffset - 1; i >= 0; --i) {
            if (m_items[i]->urlString() == item.urlString())
                return i;
        }
    }

    return -1;
}

void NavigationController::insertOrReplaceEntry(const blink::WebHistoryItem& item, blink::WebHistoryCommitType type, bool isSameDocument)
{
    HistoryEntry* historyItem = new HistoryEntry();
    historyItem->initialize();
    historyItem->setURLString(item.urlString());
    historyItem->setReferrer(item.referrer(), item.referrerPolicy());
    historyItem->setTarget(item.target());
    historyItem->setStateObject(item.stateObject());
    historyItem->setDocumentState(item.documentState());
    historyItem->setScrollRestorationType(item.scrollRestorationType());
    historyItem->setScrollOffset(item.scrollOffset());
    historyItem->setPageScaleFactor(item.pageScaleFactor());
    historyItem->setItemSequenceNumber(item.itemSequenceNumber());
    historyItem->setDocumentSequenceNumber(item.documentSequenceNumber());
    historyItem->setPinchViewportScrollOffset(item.pinchViewportScrollOffset());
    historyItem->setHTTPContentType(item.httpContentType());
    historyItem->setHTTPBody(item.httpBody());
    historyItem->m_isSameDocument = isSameDocument;

    //     |  
    switch (type) {
    case blink::WebStandardCommit:
        ++m_currentOffset;
        ASSERT(0 <= m_currentOffset && m_currentOffset <= (int)m_items.size());
        if (m_currentOffset == m_items.size()) {
            m_items.append(historyItem);
        } else {
            m_items[m_currentOffset] = historyItem;
            for (size_t i = m_currentOffset + 1; i < m_items.size(); ++i)
                delete m_items[i];
            m_items.resize(m_currentOffset + 1);
        }
        break;
    case blink::WebBackForwardCommit: {
        int entryIndex = findEntry(item);
        if (-1 == entryIndex) {
            ASSERT(false);
            break;
        }
        m_items[entryIndex] = historyItem;
        m_currentOffset = entryIndex;
        break;
    }
    case blink::WebInitialCommitInChildFrame:
        break;
    case blink::WebHistoryInertCommit: // reload，或replaceState
//         if (0 != m_items.size()) {
//             delete m_items[m_items.size() - 1];
//             m_items.removeLast();
//         }
//         m_items.append(historyItem);
        if (m_items.size() != 0 && m_currentOffset < (int)m_items.size())
            m_items[m_currentOffset] = historyItem;
        break;
    default:
        break;
    }
}

using namespace blink;

DEFINE_TRACE(NavigationController)
{
}

}