
#include "content/ui/DragHandle.h"
#include "content/ui/WCDataObject.h"
#include "content/ui/WebDropSource.h"
#include "content/ui/ClipboardUtil.h"
#include "content/browser/CheckReEnter.h"
#include "wke/wkedefine.h"
#include "wke/wkeGlobalVar.h"
#include "third_party/WebKit/public/web/WebLocalFrame.h"
#include "third_party/WebKit/Source/web/WebViewImpl.h"
#include "third_party/WebKit/public/platform/WebImage.h"
#include "third_party/WebKit/public/platform/WebPoint.h"
#include "third_party/WebKit/public/platform/Platform.h"
#include "base/values.h"
#include "base/json/json_writer.h"

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
    m_taskCount = 0;
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

// blink::WebDragOperation DragHandle::keyStateToDragOperation(DWORD grfKeyState) const
// {
//     // Conforms to Microsoft's key combinations as documented for 
//     // IDropTarget::DragOver. Note, grfKeyState is the current 
//     // state of the keyboard modifier keys on the keyboard. See:
//     // <http://msdn.microsoft.com/en-us/library/ms680129(VS.85).aspx>.
//     blink::WebDragOperation operation = blink::WebDragOperationNone; // m_page->dragController().sourceDragOperation();
// 
//     if ((grfKeyState & (MK_CONTROL | MK_SHIFT)) == (MK_CONTROL | MK_SHIFT))
//         operation = blink::WebDragOperationLink;
//     else if ((grfKeyState & MK_CONTROL) == MK_CONTROL)
//         operation = blink::WebDragOperationCopy;
//     else if ((grfKeyState & MK_SHIFT) == MK_SHIFT)
//         operation = blink::WebDragOperationGeneric;
// 
//     return operation;
// }

// int ConvertKeyStateToAuraEventFlags(DWORD key_state)
// int ConvertAuraEventFlagsToWebInputEventModifiers(int aura_event_flags)
static int keyStateToWebInputEventModifiers(DWORD keyState)
{
    int flags = 0;

    if (keyState & MK_CONTROL)
        flags |= blink::WebInputEvent::ControlKey;
    if (keyState & MK_ALT)
        flags |= blink::WebInputEvent::AltKey;
    if (keyState & MK_SHIFT)
        flags |= blink::WebInputEvent::ShiftKey;
    if (keyState & MK_LBUTTON)
        flags |= blink::WebInputEvent::LeftButtonDown;
    if (keyState & MK_MBUTTON)
        flags |= blink::WebInputEvent::MiddleButtonDown;
    if (keyState & MK_RBUTTON)
        flags |= blink::WebInputEvent::RightButtonDown;
    return flags;
}

bool DragHandle::containsPlainText(IDataObject* pDataObject)
{
    if (pDataObject)
        return SUCCEEDED(pDataObject->QueryGetData(ClipboardUtil::getPlainTextWFormatType())) ||
        SUCCEEDED(pDataObject->QueryGetData(ClipboardUtil::getPlainTextFormatType()));
    return false;
}

static void dropFilesToWebDragData(std::vector<blink::WebDragData::Item>* itemList, IDataObject* pDataObject)
{
    FORMATETC iFormat = { 0 };
    STGMEDIUM iMedium = { 0 };

    iFormat.cfFormat = CF_HDROP;
    iFormat.dwAspect = DVASPECT_CONTENT;
    iFormat.lindex = -1;
    iFormat.tymed = TYMED_HGLOBAL;

    HRESULT hRes = pDataObject->GetData(&iFormat, &iMedium);
    if (FAILED(hRes))
        return;

    HDROP hDrop = (HDROP)::GlobalLock(iMedium.hGlobal);
    DWORD iLen = ::GlobalSize(iMedium.hGlobal);

    Vector<WCHAR> filenames;
    filenames.resize(MAX_PATH);

    int count = ::DragQueryFile(hDrop, 0xFFFFFFFF, filenames.data(), MAX_PATH * sizeof(WCHAR));

    for (int i = 0; i < count; i++) {
        ::DragQueryFile(hDrop, i, filenames.data(), MAX_PATH * sizeof(WCHAR));
       
        blink::WebDragData::Item item;
        item.storageType = blink::WebDragData::Item::StorageTypeFilename;
        item.filenameData = blink::WebString(filenames.data(), wcslen(filenames.data()));
        itemList->push_back(item);
    }
}

blink::WebDragData DragHandle::dropDataToWebDragData(IDataObject* pDataObject)
{
    blink::WebDragData result;
    result.initialize();

    std::vector<blink::WebDragData::Item> itemList;

    dropFilesToWebDragData(&itemList, pDataObject);

    if (containsPlainText(pDataObject)) {
        blink::WebDragData::Item item;
        item.storageType = blink::WebDragData::Item::StorageTypeString;
        item.stringType = blink::WebString::fromUTF8(kMimeTypeText);
        item.stringData = blink::WebString::fromUTF8(ClipboardUtil::getPlainText(pDataObject));
        itemList.push_back(item);
    }

    base::DictionaryValue* dictVal = ClipboardUtil::getCustomPlainTexts(pDataObject);
    if (dictVal) {
        base::DictionaryValue::Iterator it(*dictVal);
        for (; !it.IsAtEnd(); it.Advance()) {
            std::string type = it.key();
            std::string text;

            const base::Value& val = it.value();
            if (!val.IsType(base::Value::TYPE_STRING))
                continue;
            if (!val.GetAsString(&text) || text.empty())
                continue;

            blink::WebDragData::Item item;
            item.storageType = blink::WebDragData::Item::StorageTypeString;
            item.stringType = blink::WebString::fromUTF8(type.c_str());
            item.stringData = blink::WebString::fromUTF8(text.c_str());
            itemList.push_back(item);
        }
        delete dictVal;
    }
        
    result.setItems(itemList);
    //result.setFilesystemId(drop_data.filesystem_id);

    return result;
}

DWORD DragHandle::dragOperationToDragCursor(blink::WebDragOperation op)
{
    DWORD result = DROPEFFECT_NONE;
    if (op & blink::WebDragOperationCopy)
        result = DROPEFFECT_COPY;
    else if (op & blink::WebDragOperationLink)
        result = DROPEFFECT_LINK;
    else if (op & blink::WebDragOperationMove)
        result = DROPEFFECT_MOVE;
    else if (op & blink::WebDragOperationGeneric)
        result = DROPEFFECT_MOVE; //This appears to be the Firefox behaviour
    return result;
}

blink::WebDragOperation DragHandle::dragCursorTodragOperation(DWORD op)
{
    blink::WebDragOperation result = blink::WebDragOperationNone;
    if (op & DROPEFFECT_COPY)
        result = blink::WebDragOperationCopy;
    else if (op & DROPEFFECT_LINK)
        result = blink::WebDragOperationLink;
    else if (op & DROPEFFECT_MOVE)
        result = blink::WebDragOperationMove;
    else if (op & DROPEFFECT_MOVE)
        result = blink::WebDragOperationGeneric;
    return result;
}

blink::WebDragOperation DragHandle::startDraggingInUiThread(blink::WebLocalFrame* frame,
    const wkeWebDragData* data,
    const blink::WebDragOperationsMask mask,
    const blink::WebImage* image,
    const blink::WebPoint* dragImageOffset)
{
    //blink::WebDragOperation op = doStartDragging(frame, *data, mask, *image, *dragImageOffset);

    blink::WebDragOperation operation = blink::WebDragOperationNone;

    if (!m_viewWindow && !wke::g_wkeUiThreadPostTaskCallback) // updataInOtherThreadEnabled下这个值为空，除非设置了ui post回调
        return operation;

    //FIXME: Allow UIDelegate to override behaviour <rdar://problem/5015953>

    //We liberally protect everything, to protect against a load occurring mid-drag
    COMPtr<IDragSourceHelper> helper;
    COMPtr<IDropSource> source;
    if (FAILED(WebDropSource::createInstance(&source)))
        return operation;

    WCDataObject* dataObjectPtr = nullptr;
    WCDataObject::createInstance(&dataObjectPtr);

    wkeWebDragData::Item* items = data->m_itemList;

    base::DictionaryValue* dictVal = nullptr;

    for (int i = 0; i < data->m_itemListLength; ++i) {
        const wkeWebDragData::Item& it = items[i];
        if (wkeWebDragData::Item::StorageTypeString == it.storageType) {
            std::string typeStr((const char*)(it.stringType->data ? it.stringType->data : ""), it.stringType->length != 0 ? it.stringType->length : 0);
            std::string dataStr((const char*)(it.stringData->data ? it.stringData->data : ""), it.stringData->length != 0 ? it.stringData->length : 0);
            
            WCDataObject::ClipboardDataType typeEnum = WCDataObject::clipboardTypeFromMIMEType(typeStr);
            if (WCDataObject::kClipboardDataTypeNone == typeEnum) {
                if (!dictVal)
                    dictVal = new base::DictionaryValue();

                dictVal->SetString(typeStr, dataStr);
            } else
                dataObjectPtr->writeString(typeStr, dataStr);
        }
    }

    if (dictVal) {
        std::string json;
        base::JSONWriter::Write(*dictVal, &json);
        if (json.size() > 0)
            dataObjectPtr->writeCustomPlainText(json);

        delete dictVal;
    }

    m_dragData = dataObjectPtr;
    m_mask = mask;

    if (!wke::g_isSetDragDropEnable) {
        simulateDrag();
        return blink::WebDragOperationCopy;
    }

    if (source && (!image->isNull() || m_dragData)) {
        if (!image->isNull()) {
            // if (SUCCEEDED(::CoCreateInstance(CLSID_DragDropHelper, 0, CLSCTX_INPROC_SERVER,
            //     IID_IDragSourceHelper, (LPVOID*)&helper))) {
            //     BITMAP b;
            //     GetObject(image, sizeof(BITMAP), &b);
            //     SHDRAGIMAGE sdi;
            //     sdi.sizeDragImage.cx = b.bmWidth;
            //     sdi.sizeDragImage.cy = b.bmHeight;
            //     sdi.crColorKey = 0xffffffff;
            //     sdi.hbmpDragImage = image;
            //     sdi.ptOffset.x = dragImageOffset.x - imageOrigin.x();
            //     sdi.ptOffset.y = dragImageOffset.y - imageOrigin.y();
            //     if (isLink)
            //         sdi.ptOffset.y = b.bmHeight - sdi.ptOffset.y;
            // 
            //     helper->InitializeFromBitmap(&sdi, m_dragData.get());
            // }
        }

        DWORD okEffect = draggingSourceOperationMaskToDragCursors(mask);
        DWORD effect = DROPEFFECT_NONE;
        HRESULT hr = E_NOTIMPL;

        //         m_notifOnEnterDrag();
        //         CheckReEnter::decrementEnterCount();
        hr = ::DoDragDrop(m_dragData.get(), source.get(), okEffect, &effect);
        //         CheckReEnter::incrementEnterCount();
        //         m_notifOnLeaveDrag();

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

class StartDraggingTask {
public:
    StartDraggingTask(
        DragHandle* dragHandle,
        int* count,
        blink::WebLocalFrame* frame,
        const wkeWebDragData* data,
        const blink::WebDragOperationsMask mask,
        const blink::WebImage& image,
        const blink::WebPoint& dragImageOffset)
    {
        m_dragHandle = dragHandle;
        m_count = count;
        m_frame = frame;
        m_data = data;
        m_mask = mask;
        m_image = &image;
        m_dragImageOffset = &dragImageOffset;
        m_op = blink::WebDragOperationNone;

        *m_count += 1;
    }

    static void WKE_CALL_TYPE runInUiThread(HWND hWnd, void* param)
    {
        StartDraggingTask* self = (StartDraggingTask*)param;
        self->runInUiThreadImpl();
    }

    void runInUiThreadImpl()
    {
        m_op = m_dragHandle->startDraggingInUiThread(m_frame, m_data, m_mask, m_image, m_dragImageOffset);
        *m_count -= 1;
    }

    blink::WebDragOperation getOp() const { return m_op; }

private:
    blink::WebLocalFrame* m_frame;
    const wkeWebDragData* m_data;
    blink::WebDragOperationsMask m_mask;
    const blink::WebImage* m_image;
    const blink::WebPoint* m_dragImageOffset;

    blink::WebDragOperation m_op;

    DragHandle* m_dragHandle;
    int* m_count;
};

void DragHandle::startDragging(blink::WebLocalFrame* frame,
    const wkeWebDragData* dragDate,
    const blink::WebDragOperationsMask mask,
    const blink::WebImage& image,
    const blink::WebPoint& dragImageOffset)
{
    if (!m_webViewImpl || !wke::g_isSetDragDropEnable)
        return;

    m_notifOnEnterDrag();
    CheckReEnter::decrementEnterCount();

    StartDraggingTask* task = new StartDraggingTask(this, &m_taskCount, frame, dragDate, mask, image, dragImageOffset);
    if (wke::g_wkeUiThreadPostTaskCallback) {
        wke::g_wkeUiThreadPostTaskCallback(m_viewWindow, StartDraggingTask::runInUiThread, task);

        MSG msg = { 0 };
        while (0 != m_taskCount) {
            if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
                ::TranslateMessage(&msg);
                ::DispatchMessageW(&msg);
            }
            ::Sleep(10);
        }
    } else {
        StartDraggingTask::runInUiThread(m_viewWindow, (void*)task);
    }
    CheckReEnter::incrementEnterCount();
    m_notifOnLeaveDrag();

    POINT screenPoint = { 0 };
    ::GetCursorPos(&screenPoint);

    POINT clientPoint = screenPoint;
    ::ScreenToClient(m_viewWindow, &clientPoint);

    m_webViewImpl->dragSourceEndedAt(blink::WebPoint(clientPoint.x, clientPoint.y), blink::WebPoint(screenPoint.x, screenPoint.y), task->getOp());
    m_webViewImpl->dragSourceSystemDragEnded();

    delete task;
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

struct DragEnterInfo {
    blink::WebViewImpl* webViewImpl;
    int* taskCount;
    blink::WebDragData data;
    blink::WebPoint clientPoint;
    blink::WebPoint screenPoint;
    blink::WebDragOperationsMask opAllowed;
    int modifiers;
};

static void dragTargetDragEnter(DragEnterInfo* info)
{
    info->webViewImpl->dragTargetDragEnter(info->data, info->clientPoint, info->screenPoint, info->opAllowed, info->modifiers);
    *(info->taskCount) -= 1;
    delete info;
}

static blink::WebDragOperationsMask dropEffectToDragOperation(DWORD effect)
{
    int op = blink::WebDragOperationNone;
    if (effect & DROPEFFECT_LINK)
        op |= blink::WebDragOperationLink;
    if (effect & DROPEFFECT_COPY)
        op |= blink::WebDragOperationCopy;
    if (effect & DROPEFFECT_MOVE)
        op |= blink::WebDragOperationMove;
    return (blink::WebDragOperationsMask)op;
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

    DWORD effect = 0;
    if (pdwEffect)
        effect = *pdwEffect;

    m_mask = dropEffectToDragOperation(effect);
    int modifiers = keyStateToWebInputEventModifiers(grfKeyState);

    if (WTF::isMainThread()) {
        blink::WebDragOperation op = m_webViewImpl->dragTargetDragEnter(dropDataToWebDragData(pDataObject),
            blink::WebPoint(clientPoint.x, clientPoint.y),
            blink::WebPoint(screenPoint.x, screenPoint.y),
            m_mask, modifiers);

        *pdwEffect = dragOperationToDragCursor(op);
    } else {
        m_taskCount++;

        DragEnterInfo* info = new DragEnterInfo();
        info->webViewImpl = m_webViewImpl;
        info->taskCount = &m_taskCount;
        info->data = dropDataToWebDragData(pDataObject);
        info->clientPoint = blink::WebPoint(clientPoint.x, clientPoint.y);
        info->screenPoint = blink::WebPoint(screenPoint.x, screenPoint.y);
        info->opAllowed = m_mask;
        info->modifiers = modifiers;
        blink::Platform::current()->mainThread()->postTask(FROM_HERE, WTF::bind(&dragTargetDragEnter, info));
        *pdwEffect = DROPEFFECT_MOVE;
    }

    m_lastDropEffect = *pdwEffect;
    //ASSERT(m_dragData.get() == pDataObject); // 外部拖入会触发这个断言

    return S_OK;
}

static void dragTargetDragOver(blink::WebViewImpl* m_webViewImpl, int* taskCount, const blink::WebPoint& clientPoint, const blink::WebPoint& screenPoint, blink::WebDragOperationsMask operationsAllowed, int modifiers)
{
    m_webViewImpl->dragTargetDragOver(clientPoint, screenPoint, operationsAllowed, modifiers);
    *taskCount -= 1;
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

    blink::WebDragOperation op = blink::WebDragOperationNone;
    int modifiers = keyStateToWebInputEventModifiers(grfKeyState);

    if (WTF::isMainThread()) {
        op = m_webViewImpl->dragTargetDragOver(
            blink::WebPoint(clientPoint.x, clientPoint.y),
            blink::WebPoint(screenPoint.x, screenPoint.y),
            m_mask, modifiers);

        *pdwEffect = DROPEFFECT_NONE;
        if (m_dragData)
            *pdwEffect = dragOperationToDragCursor(op);
        else
            *pdwEffect = DROPEFFECT_MOVE;
    } else {
        m_taskCount++;
        blink::Platform::current()->mainThread()->postTask(FROM_HERE, WTF::bind(&dragTargetDragOver, m_webViewImpl, &m_taskCount,
            blink::WebPoint(clientPoint.x, clientPoint.y),
            blink::WebPoint(screenPoint.x, screenPoint.y),
            m_mask, modifiers
            ));
        *pdwEffect = DROPEFFECT_MOVE;
    }
    m_lastDropEffect = *pdwEffect;
    return S_OK;
}

static void dragTargetDragLeave(blink::WebViewImpl* webViewImpl, int* taskCount)
{
    webViewImpl->dragTargetDragLeave();
    *taskCount -= 1;
}

HRESULT __stdcall DragHandle::DragLeave() 
{
    if (m_dropTargetHelper)
        m_dropTargetHelper->DragLeave();

    if (m_webViewImpl) {
        if (WTF::isMainThread()) {
            m_webViewImpl->dragTargetDragLeave();
        } else {
            m_taskCount++;
            blink::Platform::current()->mainThread()->postTask(FROM_HERE, WTF::bind(&dragTargetDragLeave, m_webViewImpl, &m_taskCount));
        }
        m_dragData = nullptr;
    }
    return S_OK;
}

static void dragTargetDrop(blink::WebViewImpl* webViewImpl, int* taskCount, const blink::WebPoint& clientPoint, const blink::WebPoint& screenPoint, int modifiers)
{
    webViewImpl->dragTargetDrop(clientPoint, screenPoint, modifiers);
    *taskCount -= 1;
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

    if (WTF::isMainThread()) {
        m_webViewImpl->dragTargetDrop(blink::WebPoint(clientPoint.x, clientPoint.y),
            blink::WebPoint(screenPoint.x, screenPoint.y), keyStateToWebInputEventModifiers(grfKeyState));
    } else {
        m_taskCount++;
        blink::Platform::current()->mainThread()->postTask(FROM_HERE, WTF::bind(&dragTargetDrop, m_webViewImpl, &m_taskCount,
            blink::WebPoint(clientPoint.x, clientPoint.y),
            blink::WebPoint(screenPoint.x, screenPoint.y), keyStateToWebInputEventModifiers(grfKeyState)));
    }
    
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