
#include "content/browser/PageNavController.h"
//#include "content/browser/FrameNavController.h"

#include "content/browser/WebPage.h"
#include "content/browser/WebPageImpl.h"

#include "third_party/WebKit/Source/core/frame/Frame.h"
#include "third_party/WebKit/Source/platform/weborigin/ReferrerPolicy.h"
#include "third_party/WebKit/Source/platform/weborigin/Referrer.h"
#include "third_party/WebKit/Source/wtf/text/WTFStringUtil.h"
#include "third_party/WebKit/public/platform/Platform.h"
#include "third_party/WebKit/public/platform/WebTraceLocation.h"
#include "third_party/WebKit/public/web/WebSerializedScriptValue.h"
#include "third_party/WebKit/public/web/WebLocalFrame.h"

#include "wke/wkeGlobalVar.h"

namespace content {

extern bool g_isBackKeyDown;

PageNavController::PageNavController(WebPageImpl* page)
{
    m_currentOffset = -1;
    m_page = page;
    m_protectReentryBackForwardCommit = false;
}

PageNavController::~PageNavController()
{
}

int PageNavController::historyBackListCount()
{
    return m_currentOffset;
}

int PageNavController::historyForwardListCount()
{
    int result = m_frameHistoryStates.size() - m_currentOffset - 1;
    return result;
}

// We do same-document navigation if going to a different item and if either of the following is true:
// - The other item corresponds to the same document (for history entries created via pushState or fragment changes).
// - The other item corresponds to the same set of documents, including frames (for history entries created via regular navigation)
bool PageNavController::shouldDoSameDocumentNavigationTo(const PageNavController::FrameHistoryItem* curItem, const PageNavController::FrameHistoryItem* otherItem)
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
    if (!curItem->historyItem.stateObject().isNull())
        curStateObjectString = curItem->historyItem.stateObject().toString();

    String otherStateObjectString;
    if (!otherItem->historyItem.stateObject().isNull())
        otherStateObjectString = otherItem->historyItem.stateObject().toString();

    if ((!curStateObjectString.isNull() && !curStateObjectString.isEmpty()) || (!otherStateObjectString.isNull() && !otherStateObjectString.isEmpty()))
        return curItem->historyItem.documentSequenceNumber() == otherItem->historyItem.documentSequenceNumber();

    blink::KURL curUrl(blink::ParsedURLString, curItem->historyItem.urlString());
    blink::KURL otherUrl(blink::ParsedURLString, otherItem->historyItem.urlString());
    if ((curUrl.hasFragmentIdentifier() || otherUrl.hasFragmentIdentifier()) && equalIgnoringFragmentIdentifier(curUrl, otherUrl))
        return curItem->historyItem.documentSequenceNumber() == otherItem->historyItem.documentSequenceNumber();

    return false;
}

void PageNavController::navigate(int offset)
{
    m_protectReentryBackForwardCommit = true;

    int pos = m_currentOffset + offset;
    if (pos < 0 || pos > (int)(m_frameHistoryStates.size() - 1))
        return;
    AllFrameHistoryItemSet* newItems = m_frameHistoryStates[pos];
    AllFrameHistoryItemSet* curItems = nullptr;
    if (m_currentOffset >= 0 && m_currentOffset < (int)m_frameHistoryStates.size())
        curItems = m_frameHistoryStates[m_currentOffset];
    if (!curItems)
        return;

    for (size_t i = 0; i < newItems->m_frameHistoryItems.size(); ++i) {
        FrameHistoryItem* newItem = newItems->m_frameHistoryItems[i];
        for (size_t j = 0; j < curItems->m_frameHistoryItems.size(); ++j) {
            FrameHistoryItem* curItem = curItems->m_frameHistoryItems[j];
            long long newNum = newItem->historyItem.itemSequenceNumber();
            long long curNum = curItem->historyItem.itemSequenceNumber();

            // 一样itemSequenceNumber表示两者是克隆关系
            if (newItem->uniqueName == curItem->uniqueName && newItem->historyItem.itemSequenceNumber() != curItem->historyItem.itemSequenceNumber()) {

                blink::WebHistoryLoadType type = blink::WebHistoryDifferentDocumentLoad; // blink::WebFrameLoadType::BackForward;//
                if (shouldDoSameDocumentNavigationTo(curItem, newItem))
                    type = blink::WebHistorySameDocumentLoad;

                blink::WebFrame* frame = m_page->getWebFrameFromUniqueName(curItem->uniqueName);
                if (frame)
                    frame->loadHistoryItem(newItem->historyItem, type, blink::WebURLRequest::UseProtocolCachePolicy);
            }
        }
    }
}

void PageNavController::navigateBackForwardSoon(int offset)
{
    if (g_isBackKeyDown && !wke::g_backKeydownEnable)
        return;
    blink::Platform::current()->currentThread()->postTask(FROM_HERE, WTF::bind(&PageNavController::navigate, this, offset));
}

void PageNavController::navigateToIndex(int index)
{
    int offset = index - m_currentOffset;
    navigateBackForwardSoon(offset);
}

bool PageNavController::findEntryImpl(blink::WebLocalFrame* frame, const blink::WebHistoryItem& item, const AllFrameHistoryItemSet& itemSet) const
{
    // 可能会出现找不到的情况。比如sub frame后退了，然后blink会通知main frame也后退，但
    // 其实main frame只有一个item
    for (size_t i = 0; i < itemSet.m_frameHistoryItems.size(); ++i) {
        FrameHistoryItem* frameItem = itemSet.m_frameHistoryItems[i];
        long long s1 = frameItem->historyItem.itemSequenceNumber();
        long long s2 = item.itemSequenceNumber();
        if (s1 == s2)
            return true;
    }

    return false;
}

int PageNavController::findEntry(blink::WebLocalFrame* frame, const blink::WebHistoryItem& item) const
{
    if (m_frameHistoryStates.size() <= 1)
        return 0;

    const AllFrameHistoryItemSet* itemSet = nullptr;
//     if (m_currentOffset >= 1) {
//         itemSet = m_frameHistoryStates[m_currentOffset - 1];
//         if (findEntryImpl(frame, item, *itemSet))
//             return -1;
//     }
// 
//     if (m_currentOffset <= (int)m_frameHistoryStates.size() - 2) {
//         itemSet = m_frameHistoryStates[m_currentOffset + 1];
//         if (findEntryImpl(frame, item, *itemSet))
//             return 1;
//     }
    for (size_t i= 0; i < m_frameHistoryStates.size(); ++i) {
        itemSet = m_frameHistoryStates[i];
        if (findEntryImpl(frame, item, *itemSet))
            return i - m_currentOffset;
    }
    return 0;
}

void PageNavController::insertOrReplaceEntry(blink::WebLocalFrame* frame, const blink::WebHistoryItem& item, blink::WebHistoryCommitType type, bool isSameDocument)
{
    AllFrameHistoryItemSet* frameEntrys = nullptr;
    switch (type) {
    case blink::WebStandardCommit:
    {
        if (m_currentOffset < 0 || m_currentOffset >= (int)m_frameHistoryStates.size())
            frameEntrys = new AllFrameHistoryItemSet();
        else
            frameEntrys = m_frameHistoryStates[m_currentOffset]->clone();

//         if (1) {
//             for (size_t i = 0; i < frameEntrys->m_frameHistoryItems.size(); ++i) {
//                 FrameHistoryItem* it = frameEntrys->m_frameHistoryItems[i];
// 
//                 char* output = (char*)malloc(0x100);
//                 sprintf_s(output, 0x99, "insertOrReplaceEntry, WebStandardCommit --: %I64d\n", it->historyItem.itemSequenceNumber());
//                 OutputDebugStringA(output);
//                 free(output);
//             }
//         }

        frameEntrys->insertOrReplaceEntry(frame, item, type, isSameDocument);
        frameEntrys->clearRemovedFrame(frame);

//         char* output = (char*)malloc(0x100);
//         sprintf_s(output, 0x99, "insertOrReplaceEntry, WebStandardCommit: %I64d\n", item.itemSequenceNumber());
//         OutputDebugStringA(output);
//         free(output);

//         if (0) {
//             for (size_t i = 0; i < frameEntrys->m_frameHistoryItems.size(); ++i) {
//                 FrameHistoryItem* it = frameEntrys->m_frameHistoryItems[i];
// 
//                 output = (char*)malloc(0x100);
//                 sprintf_s(output, 0x99, "insertOrReplaceEntry, WebStandardCommit ~~~: %I64d\n", it->historyItem.itemSequenceNumber());
//                 OutputDebugStringA(output);
//                 free(output);
//             }
//         }

        ++m_currentOffset;
        ASSERT(0 <= m_currentOffset && m_currentOffset <= (int)m_frameHistoryStates.size());
        if (m_currentOffset == m_frameHistoryStates.size()) {
            m_frameHistoryStates.append(frameEntrys);
        } else {
            m_frameHistoryStates[m_currentOffset] = frameEntrys;
            m_frameHistoryStates.resize(m_currentOffset + 1);
        }
        break;
    }
    case blink::WebBackForwardCommit:
    {
        if (m_protectReentryBackForwardCommit) {
            m_protectReentryBackForwardCommit = false;
            int offset = findEntry(frame, item);
            if (0 == offset) {
                ASSERT(false);
                break;
            }
            int entryIndex = m_currentOffset + offset;
            ASSERT(entryIndex < (int)m_frameHistoryStates.size());
            if (-1 == entryIndex) {
                ASSERT(false);
                break;
            }
            frameEntrys = m_frameHistoryStates[entryIndex];
            frameEntrys->insertOrReplaceEntry(frame, item, type, isSameDocument);
            m_frameHistoryStates[entryIndex] = frameEntrys;

            m_currentOffset = entryIndex;
        } else if (m_currentOffset >= 0 && m_currentOffset < (int)m_frameHistoryStates.size()) {
            frameEntrys = m_frameHistoryStates[m_currentOffset];
            frameEntrys->insertOrReplaceEntry(frame, item, type, isSameDocument);
            m_frameHistoryStates[m_currentOffset] = frameEntrys;
        }

        break;
    }
    case blink::WebInitialCommitInChildFrame: // 子frame初始化时
    {
//         char* output = (char*)malloc(0x100);
//         sprintf_s(output, 0x99, "insertOrReplaceEntry, WebInitialCommitInChildFrame: %I64d\n", item.itemSequenceNumber());
//         OutputDebugStringA(output);
//         free(output);
    }
    case blink::WebHistoryInertCommit: // reload，或replaceState
        if (m_frameHistoryStates.size() != 0 && m_currentOffset < (int)m_frameHistoryStates.size()) {
            frameEntrys = m_frameHistoryStates[m_currentOffset];
            frameEntrys->insertOrReplaceEntry(frame, item, type, isSameDocument);
            m_frameHistoryStates[m_currentOffset] = frameEntrys;
        }
        break;
    default:
        break;
    }

//     char* output = (char*)malloc(0x100);
//     sprintf_s(output, 0x99, "PageNavController::insertOrReplaceEntry begin: %d\n", m_frameHistoryStates.size());
//     OutputDebugStringA(output);
//     free(output);
// 
//     for (size_t i = 0; i < m_frameHistoryStates.size(); ++i) {
//         AllFrameHistoryItemSet* states = m_frameHistoryStates[i];
// 
//         char* output = (char*)malloc(0x100);
//         sprintf_s(output, 0x99, "PageNavController::insertOrReplaceEntry         i: %d, size:%d\n", i, states->m_frameOffsets.size());
//         OutputDebugStringA(output);
// 
//         for (size_t j = 0; j < states->m_frameOffsets.size(); ++j) {
//             FrameHistoryItemOffset* oneFrame = states->m_frameOffsets[j];
// 
//             sprintf_s(output, 0x99, "PageNavController::insertOrReplaceEntry           : %s, %d, %d\n", oneFrame->uniqueName.utf8().data(), oneFrame->curNavOffset, type);
//             OutputDebugStringA(output);
//         }
//         free(output);
//     }
// 
//     OutputDebugStringA("----end\n");
}

blink::WebHistoryItem PageNavController::historyItemForNewChildFrame(blink::WebFrame* frame)
{
    AllFrameHistoryItemSet* curItem = nullptr;
    if (m_currentOffset >= 0 && m_currentOffset < (int)m_frameHistoryStates.size())
        curItem = m_frameHistoryStates[m_currentOffset];
    if (!curItem)
        return blink::WebHistoryItem();

    for (size_t i = 0; i < curItem->m_frameHistoryItems.size(); ++i) {
        FrameHistoryItem* frameHistory = curItem->m_frameHistoryItems[i];
        if (frameHistory->uniqueName != (String)frame->uniqueName())
            continue;
        return frameHistory->historyItem;
    }
    return blink::WebHistoryItem();
}

void PageNavController::AllFrameHistoryItemSet::clearRemovedFrame(const blink::WebLocalFrame* frame)
{
    WTF::Vector<const blink::WebFrame*> newFrames;
    const blink::WebFrame* cur = frame;
    const blink::WebFrame* rootFrame = frame;
    do {
        rootFrame = cur;
        cur = cur->parent();
    } while (cur);

    cur = rootFrame;
    do {
        newFrames.append(cur);
        cur = cur->traverseNext(false);
    } while (cur);

    for (size_t i = 0; i < m_frameHistoryItems.size(); ++i) {
        bool find = false;
        FrameHistoryItem* oldItem = m_frameHistoryItems[i];

        for (size_t j = 0; j < newFrames.size(); ++j) {
            const blink::WebFrame* newItem = newFrames[j];
            if (oldItem->uniqueName == (String)newItem->uniqueName()) {
                find = true;
                break;
            }
        }        
        
        if (!find) {
            m_frameHistoryItems.remove(i);
            --i;
        }
    }    
}

void PageNavController::FrameHistoryItem::copyHistoryItem(const blink::WebHistoryItem& item)
{
    historyItem.initialize();
    historyItem.setURLString(item.urlString());
    historyItem.setReferrer(item.referrer(), item.referrerPolicy());
    historyItem.setTarget(item.target());
    historyItem.setStateObject(item.stateObject());
    historyItem.setDocumentState(item.documentState());
    historyItem.setScrollRestorationType(item.scrollRestorationType());
    historyItem.setScrollOffset(item.scrollOffset());
    historyItem.setPageScaleFactor(item.pageScaleFactor());
    historyItem.setItemSequenceNumber(item.itemSequenceNumber());
    historyItem.setDocumentSequenceNumber(item.documentSequenceNumber());
    historyItem.setPinchViewportScrollOffset(item.pinchViewportScrollOffset());
    historyItem.setHTTPContentType(item.httpContentType());
    historyItem.setHTTPBody(item.httpBody());
}

void PageNavController::AllFrameHistoryItemSet::insertOrReplaceEntry(blink::WebLocalFrame* frame, const blink::WebHistoryItem& item, blink::WebHistoryCommitType type, bool isSameDocument)
{
    for (size_t i = 0; i < m_frameHistoryItems.size(); ++i) {
        // 找到同名的frame，插入，再把新frame所属的tree里老tree不存在的节点全删了
        FrameHistoryItem* frameItem = m_frameHistoryItems[i];
        if (frameItem->uniqueName != (String)frame->uniqueName())
            continue;
        frameItem->copyHistoryItem(item);
        frameItem->isSameDocument = isSameDocument;
        m_frameHistoryItems[i] = frameItem;
        return;
    }
    m_frameHistoryItems.append(new FrameHistoryItem(frame->uniqueName(), item));
}

PageNavController::AllFrameHistoryItemSet* PageNavController::AllFrameHistoryItemSet::clone() const
{
    AllFrameHistoryItemSet* result = new AllFrameHistoryItemSet();
    for (size_t i = 0; i < m_frameHistoryItems.size(); ++i) {
        FrameHistoryItem* newItem = new FrameHistoryItem(*(m_frameHistoryItems[i]));
        result->m_frameHistoryItems.append(newItem);
    }
    return result;
}

using namespace blink;

DEFINE_TRACE(PageNavController)
{
#if ENABLE(OILPAN)
    for (size_t i = 0; i < m_frameHistoryStates.size(); ++i) {
        AllFrameHistoryItemSet* entrys = m_frameHistoryStates[i];
        visitor->trace(entrys);
    }
#endif
}

DEFINE_TRACE(PageNavController::FrameHistoryItem)
{
#if ENABLE(OILPAN)
    //visitor->trace(historyItem);
#endif
}

DEFINE_TRACE(PageNavController::AllFrameHistoryItemSet)
{
#if ENABLE(OILPAN)
    for (size_t i = 0; i < m_frameHistoryItems.size(); ++i) {
        FrameHistoryItem* item = m_frameHistoryItems[i];
        visitor->trace(item);
    }
#endif
}


}