
#include "content/ui/DragHandle.h"
#include "content/ui/WCDataObject.h"
#include "content/ui/WebDropSource.h"
#include "content/ui/ClipboardUtil.h"
#include "third_party/WebKit/public/web/WebLocalFrame.h"
#include "third_party/WebKit/Source/web/WebViewImpl.h"
#include "third_party/WebKit/public/platform/WebImage.h"
#include "third_party/WebKit/public/platform/WebPoint.h"

extern bool g_isSetDragDropEnable;

namespace content {

DragHandle::DragHandle(
    std::function<void(void)>&& notifOnEnterDrag,
    std::function<void(void)>&& notifOnLeaveDrag,
    std::function<void(void)>&& notifOnDragging
    )
    : m_notifOnEnterDrag(std::move(notifOnEnterDrag))
    , m_notifOnLeaveDrag(std::move(notifOnLeaveDrag))
    , m_notifOnDragging(std::move(notifOnDragging))
{
    m_refCount = 0;
    m_viewWindow = nullptr;
    m_webViewImpl = nullptr;
}

void DragHandle::setViewWindow(HWND viewWindow, blink::WebViewImpl* webViewImpl)
{
    m_viewWindow = viewWindow;
    m_webViewImpl = webViewImpl;
}

DWORD DragHandle::draggingSourceOperationMaskToDragCursors(blink::WebDragOperationsMask op)
{
    DWORD result = DROPEFFECT_NONE;
    if (op == blink::WebDragOperationEvery)
        return DROPEFFECT_COPY | DROPEFFECT_LINK | DROPEFFECT_MOVE;
    if (op & blink::WebDragOperationCopy)
        result |= DROPEFFECT_COPY;
    if (op & blink::WebDragOperationLink)
        result |= DROPEFFECT_LINK;
    if (op & blink::WebDragOperationMove)
        result |= DROPEFFECT_MOVE;
    if (op & blink::WebDragOperationGeneric)
        result |= DROPEFFECT_MOVE;
    return result;
}

blink::WebDragOperation DragHandle::keyStateToDragOperation(DWORD grfKeyState) const
{
    // Conforms to Microsoft's key combinations as documented for 
    // IDropTarget::DragOver. Note, grfKeyState is the current 
    // state of the keyboard modifier keys on the keyboard. See:
    // <http://msdn.microsoft.com/en-us/library/ms680129(VS.85).aspx>.
    blink::WebDragOperation operation = blink::WebDragOperationNone; // m_page->dragController().sourceDragOperation();

    if ((grfKeyState & (MK_CONTROL | MK_SHIFT)) == (MK_CONTROL | MK_SHIFT))
        operation = blink::WebDragOperationLink;
    else if ((grfKeyState & MK_CONTROL) == MK_CONTROL)
        operation = blink::WebDragOperationCopy;
    else if ((grfKeyState & MK_SHIFT) == MK_SHIFT)
        operation = blink::WebDragOperationGeneric;

    return operation;
}

bool DragHandle::containsPlainText(IDataObject* pDataObject)
{
    if (pDataObject)
        return SUCCEEDED(pDataObject->QueryGetData(ClipboardUtil::getPlainTextWFormatType())) ||
        SUCCEEDED(pDataObject->QueryGetData(ClipboardUtil::getPlainTextFormatType()));
    return false;
}

blink::WebDragData DragHandle::dropDataToWebDragData(IDataObject* pDataObject)
{
    blink::WebDragData result;
    result.initialize();

    std::vector<blink::WebDragData::Item> itemList;

    if (containsPlainText(pDataObject)) {
        blink::WebDragData::Item item;
        item.storageType = blink::WebDragData::Item::StorageTypeString;
        item.stringType = blink::WebString::fromUTF8(kMimeTypeText);
        item.stringData = blink::WebString::fromUTF8(ClipboardUtil::getPlainText(pDataObject));
        itemList.push_back(item);
    }

    result.setItems(itemList);
    //result.setFilesystemId(drop_data.filesystem_id);

    return result;
}

DWORD DragHandle::dragOperationToDragCursor(blink::WebDragOperation op)
{
    DWORD res = DROPEFFECT_NONE;
    if (op & blink::WebDragOperationCopy)
        res = DROPEFFECT_COPY;
    else if (op & blink::WebDragOperationLink)
        res = DROPEFFECT_LINK;
    else if (op & blink::WebDragOperationMove)
        res = DROPEFFECT_MOVE;
    else if (op & blink::WebDragOperationGeneric)
        res = DROPEFFECT_MOVE; //This appears to be the Firefox behaviour
    return res;
}

void DragHandle::startDragging(blink::WebLocalFrame* frame,
    const blink::WebDragData& data,
    const blink::WebDragOperationsMask mask,
    const blink::WebImage& image,
    const blink::WebPoint& dragImageOffset)
{
    if (!m_webViewImpl)
        return;

    blink::WebDragOperation op = doStartDragging(frame, data, mask, image, dragImageOffset);

    POINT screenPoint = { 0 };
    ::GetCursorPos(&screenPoint);

    POINT clientPoint = screenPoint;
    ::ScreenToClient(m_viewWindow, &clientPoint);

    m_webViewImpl->dragSourceEndedAt(blink::WebPoint(clientPoint.x, clientPoint.y), blink::WebPoint(screenPoint.x, screenPoint.y), op);
    m_webViewImpl->dragSourceSystemDragEnded();
}

void DragHandle::simulateDrag()
{
    m_notifOnEnterDrag();

    MSG msg = { 0 };
    while (WM_QUIT != msg.message) {
        if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            ::TranslateMessage(&msg);
            ::DispatchMessageW(&msg);

            m_notifOnDragging();

            if (WM_LBUTTONUP == msg.message || WM_MOUSELEAVE == msg.message)
                break;
        }
        ::Sleep(10);
    }
    m_notifOnLeaveDrag();
}

blink::WebDragOperation DragHandle::doStartDragging(blink::WebLocalFrame* frame,
    const blink::WebDragData& data,
    const blink::WebDragOperationsMask mask,
    const blink::WebImage& image,
    const blink::WebPoint& dragImageOffset)
{
    blink::WebDragOperation operation = blink::WebDragOperationNone;

    if (!m_viewWindow) // updataInOtherThreadEnabled下这个值为空
        return operation;

    //FIXME: Allow UIDelegate to override behaviour <rdar://problem/5015953>

    //We liberally protect everything, to protect against a load occurring mid-drag
    COMPtr<IDragSourceHelper> helper;
    COMPtr<IDropSource> source;
    if (FAILED(WebDropSource::createInstance(&source)))
        return operation;

    WCDataObject* dataObjectPtr = nullptr;
    WCDataObject::createInstance(&dataObjectPtr);

    if (!data.isNull()) {
        blink::WebVector<blink::WebDragData::Item> items = data.items();
        for (size_t i = 0; i < items.size(); ++i) {
            blink::WebDragData::Item it = items[i];
            if (blink::WebDragData::Item::StorageTypeString == it.storageType) {
                dataObjectPtr->writeString(it.stringType.utf8(), it.stringData.utf8());
            }
        }
    }

    m_dragData = dataObjectPtr;
    m_mask = mask;

    if (!g_isSetDragDropEnable) {
        simulateDrag();
        return blink::WebDragOperationCopy;
    }

    if (source && (!image.isNull() || m_dragData)) {
        if (!image.isNull()) {
            //                 if (SUCCEEDED(::CoCreateInstance(CLSID_DragDropHelper, 0, CLSCTX_INPROC_SERVER,
            //                     IID_IDragSourceHelper, (LPVOID*)&helper))) {
            //                     BITMAP b;
            //                     GetObject(image, sizeof(BITMAP), &b);
            //                     SHDRAGIMAGE sdi;
            //                     sdi.sizeDragImage.cx = b.bmWidth;
            //                     sdi.sizeDragImage.cy = b.bmHeight;
            //                     sdi.crColorKey = 0xffffffff;
            //                     sdi.hbmpDragImage = image;
            //                     sdi.ptOffset.x = dragImageOffset.x - imageOrigin.x();
            //                     sdi.ptOffset.y = dragImageOffset.y - imageOrigin.y();
            //                     if (isLink)
            //                         sdi.ptOffset.y = b.bmHeight - sdi.ptOffset.y;
            // 
            //                     helper->InitializeFromBitmap(&sdi, m_dragData.get());
            //                 }
        }

        DWORD okEffect = draggingSourceOperationMaskToDragCursors(mask);
        DWORD effect = DROPEFFECT_NONE;
        HRESULT hr = E_NOTIMPL;

        hr = ::DoDragDrop(m_dragData.get(), source.get(), okEffect, &effect);

        if (hr == DRAGDROP_S_DROP) {
            if (effect & DROPEFFECT_COPY)
                operation = blink::WebDragOperationCopy;
            else if (effect & DROPEFFECT_LINK)
                operation = blink::WebDragOperationLink;
            else if (effect & DROPEFFECT_MOVE)
                operation = blink::WebDragOperationMove;
        }
    }
    return operation;
}

// IDropTarget impl
HRESULT __stdcall DragHandle::DragEnter(IDataObject* pDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
    if (!m_webViewImpl)
        return S_OK;
    
    if (!m_dropTargetHelper)
        ::CoCreateInstance(CLSID_DragDropHelper, 0, CLSCTX_INPROC_SERVER, IID_IDropTargetHelper, (void**)&m_dropTargetHelper);

    if (m_dropTargetHelper)
        m_dropTargetHelper->DragEnter(m_viewWindow, pDataObject, (POINT*)&pt, *pdwEffect);

    POINT screenPoint = { 0 };
    ::GetCursorPos(&screenPoint);

    POINT clientPoint = screenPoint;
    ::ScreenToClient(m_viewWindow, &clientPoint);

    blink::WebDragOperation op = m_webViewImpl->dragTargetDragEnter(dropDataToWebDragData(pDataObject),
        blink::WebPoint(clientPoint.x, clientPoint.y),
        blink::WebPoint(screenPoint.x, screenPoint.y),
        keyStateToDragOperation(grfKeyState), keyStateToDragOperation(grfKeyState));

    *pdwEffect = dragOperationToDragCursor(op);

    m_lastDropEffect = *pdwEffect;
    ASSERT(m_dragData.get() == pDataObject);

    return S_OK;
}

HRESULT __stdcall DragHandle::DragOver(DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
    if (!m_webViewImpl)
        return S_OK;
    
    if (m_dropTargetHelper)
        m_dropTargetHelper->DragOver((POINT*)&pt, *pdwEffect);

    POINT screenPoint = { 0 };
    ::GetCursorPos(&screenPoint);

    POINT clientPoint = screenPoint;
    ::ScreenToClient(m_viewWindow, &clientPoint);

    blink::WebDragOperation op = m_webViewImpl->dragTargetDragOver(
        blink::WebPoint(clientPoint.x, clientPoint.y),
        blink::WebPoint(screenPoint.x, screenPoint.y),
        m_mask, keyStateToDragOperation(grfKeyState));

    *pdwEffect = DROPEFFECT_NONE;
    if (m_dragData)
        *pdwEffect = dragOperationToDragCursor(op);       

    m_lastDropEffect = *pdwEffect;
    return S_OK;
}

HRESULT __stdcall DragHandle::DragLeave() 
{
    if (m_dropTargetHelper)
        m_dropTargetHelper->DragLeave();

    if (m_dragData) {
        if (m_webViewImpl)
            m_webViewImpl->dragTargetDragLeave();
        m_dragData = nullptr;
    }
    return S_OK;
}

HRESULT __stdcall DragHandle::Drop(IDataObject* pDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
    if (!m_webViewImpl)
        return S_OK;

    if (m_dropTargetHelper)
        m_dropTargetHelper->Drop(pDataObject, (POINT*)&pt, *pdwEffect);

    m_dragData = nullptr;
    *pdwEffect = m_lastDropEffect;

    POINT screenPoint = { 0 };
    ::GetCursorPos(&screenPoint);

    POINT clientPoint = screenPoint;
    ::ScreenToClient(m_viewWindow, &clientPoint);

    m_webViewImpl->dragTargetDrop(blink::WebPoint(clientPoint.x, clientPoint.y),
        blink::WebPoint(screenPoint.x, screenPoint.y), keyStateToDragOperation(grfKeyState));
    
    return S_OK;
}

HRESULT __stdcall DragHandle::QueryInterface(REFIID riid, void** ppvObject)
{
    if (!ppvObject)
        return E_POINTER;
    *ppvObject = nullptr;

    if (IsEqualGUID(riid, IID_IDropTarget))
        *ppvObject = static_cast<IDropTarget*>(this);
    else
        return E_NOINTERFACE;

    AddRef();
    return S_OK;
}

ULONG __stdcall DragHandle::AddRef()
{
    return ++m_refCount;
}

ULONG __stdcall DragHandle::Release()
{
    return --m_refCount;
}

ULONG DragHandle::getRefCount() const
{
    return m_refCount;
}

}