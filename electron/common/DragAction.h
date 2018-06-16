
#ifndef common_DragAction_h
#define common_DragAction_h

#include "content/ui/WebDropSource.h"
#include "content/ui/WCDataObject.h"

#include "base/COMPtr.h"
#include <shobjidl.h>
#include <shlguid.h>
#include <ShellAPI.h>
#include <shlobj.h>

namespace atom {

class DragAction : public IDropTarget {
public:
    DragAction(wkeWebView webview, HWND viewWindow, int id) {
        m_id = id;
        m_refCount = 0;
        m_lastDropEffect = 0;
        m_mask = wkeWebDragOperationEvery;
        m_webview = webview;
        m_viewWindow = viewWindow;
    }

    static FORMATETC* getPlainTextWFormatType() {
        static FORMATETC textFormat = { CF_UNICODETEXT, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
        return &textFormat;
    }

    static FORMATETC* getPlainTextFormatType() {
        static FORMATETC textFormat = { CF_UNICODETEXT, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
        return &textFormat;
    }

    static wkeString getPlainText(IDataObject* dataObject) {
        STGMEDIUM store;

        wkeString text;
        if (SUCCEEDED(dataObject->GetData(getPlainTextWFormatType(), &store))) {
            // Unicode text
            wchar_t* data = static_cast<wchar_t*>(::GlobalLock(store.hGlobal));
            text = wkeCreateStringW(data, wcslen(data));
            GlobalUnlock(store.hGlobal);
            ReleaseStgMedium(&store);
        } else if (SUCCEEDED(dataObject->GetData(getPlainTextFormatType(), &store))) {
            // ASCII text
            char* data = static_cast<char*>(GlobalLock(store.hGlobal));
            text = wkeCreateStringW(L"", 0);
            wkeSetString(text, data, strlen(data));
            ::GlobalUnlock(store.hGlobal);
            ReleaseStgMedium(&store);
        } else {
            // FIXME: Originally, we called getURL() here because dragging and dropping files doesn't
            // populate the drag with text data. Per https://bugs.webkit.org/show_bug.cgi?id=38826, this
            // is undesirable, so maybe this line can be removed.
            text = wkeCreateStringW(L"", 0); // getURL(dataObject, nullptr);
        }
        return text;
    }

    static bool containsPlainText(IDataObject* pDataObject) {
        if (pDataObject) {
            HRESULT hr1 = pDataObject->QueryGetData(getPlainTextWFormatType());
            HRESULT hr2 = pDataObject->QueryGetData(getPlainTextFormatType());
            if (hr1 == S_OK || hr2 == S_OK)
                return true;
        }
        return false;
    }

    static FORMATETC* cfHDropFormat() {
        static FORMATETC urlFormat = { CF_HDROP, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };

        return &urlFormat;
    }

    static bool containsFiles(IDataObject* pDataObject) {
        if (pDataObject) {

            HRESULT hr = pDataObject->QueryGetData(cfHDropFormat());
            return hr == S_OK;
        }
        return false;
    }

    static DWORD dragOperationToDragCursor(wkeWebDragOperation op) {
        DWORD res = DROPEFFECT_NONE;

        if (op & wkeWebDragOperationCopy)
            res = DROPEFFECT_COPY;
        else if (op & wkeWebDragOperationLink)
            res = DROPEFFECT_LINK;
        else if (op & wkeWebDragOperationMove)
            res = DROPEFFECT_MOVE;
        else if (op & wkeWebDragOperationGeneric)
            res = DROPEFFECT_MOVE; //This appears to be the Firefox behaviour
        return res;
    }


    static void initWkeWebDragDataItem(wkeWebDragData::Item* item) {
        item->storageType = wkeWebDragData::Item::StorageTypeString;
        item->stringType = nullptr; // wkeCreateStringW(L"", 0);
        item->stringData = nullptr; // wkeCreateStringW(L"", 0);
        item->filenameData = nullptr; // wkeCreateStringW(L"", 0);
        item->displayNameData = nullptr; // wkeCreateStringW(L"", 0);
        item->binaryData = nullptr;
        item->binaryDataLength = 0;
        item->title = nullptr; // wkeCreateStringW(L"", 0);
        item->fileSystemURL = nullptr; // wkeCreateStringW(L"", 0);
        item->fileSystemFileSize = 0;
        item->baseURL = nullptr; // wkeCreateStringW(L"", 0);
    }

    static void releaseWkeWebDragData(wkeWebDragData* data) {
        wkeDeleteString(data->m_filesystemId);
        for (int i = 0; i < data->m_itemListLength; ++i) {
            wkeWebDragData::Item* item = &data->m_itemList[i];
            wkeDeleteString(item->stringType);
            wkeDeleteString(item->stringData);
            wkeDeleteString(item->filenameData);
            wkeDeleteString(item->displayNameData);
            if (item->binaryData)
                free(item->binaryData);
            wkeDeleteString(item->title);
            wkeDeleteString(item->fileSystemURL);
            wkeDeleteString(item->baseURL);
        }
    }

    void onStartDragging(
        wkeWebView webView,
        void* param,
        wkeWebFrameHandle frame,
        const wkeWebDragData* data,
        wkeWebDragOperationsMask mask,
        const void* image,
        const wkePoint* dragImageOffset
        ) {

        HRESULT hr = E_NOTIMPL;
       

        DWORD okEffect = draggingSourceOperationMaskToDragCursors(mask);
        DWORD effect = DROPEFFECT_NONE;

        //We liberally protect everything, to protect against a load occurring mid-drag
        COMPtr<IDragSourceHelper> helper;
        COMPtr<IDropSource> source;
        if ((content::WebDropSource::createInstance(&source)) < 0)
            return;

        content::WCDataObject* dataObjectPtr = nullptr;
        content::WCDataObject::createInstance(&dataObjectPtr);

        if (data) {
            wkeWebDragData::Item* items = data->m_itemList;
            for (int i = 0; i < data->m_itemListLength; ++i) {
                wkeWebDragData::Item* it = &items[i];
                if (wkeWebDragData::Item::StorageTypeString == it->storageType) {
                    dataObjectPtr->writeString(wkeGetString(it->stringType), wkeGetString(it->stringData));
                }
            }
        }

        m_dragData = dataObjectPtr;
        hr = ::DoDragDrop(m_dragData.get(), source.get(), okEffect, &effect);

        hr = S_OK;
    }

    static wkeWebDragData* dropDataToWebDragData(IDataObject* pDataObject) {
        wkeWebDragData* result = new wkeWebDragData();

        result->m_filesystemId = nullptr;
        result->m_itemListLength = 0;

        if (containsFiles(pDataObject)) {
            STGMEDIUM medium;
            if ((pDataObject->GetData(cfHDropFormat(), &medium)) < 0)
                return result;

            HDROP hDrop = static_cast<HDROP>(GlobalLock(medium.hGlobal));
            int count = ::DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);

            result->m_itemList = new wkeWebDragData::Item[count];

            for (int i = 0; i < count; i++) {
                initWkeWebDragDataItem(&result->m_itemList[i]);
                int pathlength = ::DragQueryFile(hDrop, i, NULL, 0) + 1;
                if (pathlength >= MAX_PATH || pathlength <= 1)
                    continue;

                std::vector<wchar_t> fileName;
                fileName.resize(pathlength);
                ::DragQueryFile(hDrop, i, &(fileName.at(0)), pathlength);

                result->m_itemList[i].storageType = wkeWebDragData::Item::StorageTypeFileSystemFile;
                result->m_itemList[i].fileSystemURL = wkeCreateStringW(&fileName.at(0), pathlength);
                result->m_itemListLength++;
            }

            ::DragFinish(hDrop);
        } else if (containsPlainText(pDataObject)) {
            result->m_itemList = new wkeWebDragData::Item();
            initWkeWebDragDataItem(result->m_itemList);
            result->m_itemList->storageType = wkeWebDragData::Item::StorageTypeString;
            result->m_itemList->stringType = wkeCreateStringW(L"text/plain", wcslen(L"text/plain"));
            result->m_itemList->stringData = getPlainText(pDataObject);
            result->m_itemListLength = 1;
        }

        return result;
    }

    static DWORD draggingSourceOperationMaskToDragCursors(wkeWebDragOperationsMask op) {

        DWORD result = DROPEFFECT_NONE;
        if (op == wkeWebDragOperationEvery)
            return DROPEFFECT_COPY | DROPEFFECT_LINK | DROPEFFECT_MOVE;
        if (op & wkeWebDragOperationCopy)
            result |= DROPEFFECT_COPY;
        if (op & wkeWebDragOperationLink)
            result |= DROPEFFECT_LINK;
        if (op & wkeWebDragOperationMove)
            result |= DROPEFFECT_MOVE;
        if (op & wkeWebDragOperationGeneric)
            result |= DROPEFFECT_MOVE;
        return result;
    }

    static wkeWebDragOperation keyStateToDragOperation(DWORD grfKeyState) {
        // Conforms to Microsoft's key combinations as documented for 
        // IDropTarget::DragOver. Note, grfKeyState is the current 
        // state of the keyboard modifier keys on the keyboard. See:
        // <http://msdn.microsoft.com/en-us/library/ms680129(VS.85).aspx>.
        wkeWebDragOperation operation = wkeWebDragOperationNone; // m_page->dragController().sourceDragOperation();

        if ((grfKeyState & (MK_CONTROL | MK_SHIFT)) == (MK_CONTROL | MK_SHIFT))
            operation = wkeWebDragOperationLink;
        else if ((grfKeyState & MK_CONTROL) == MK_CONTROL)
            operation = wkeWebDragOperationCopy;
        else if ((grfKeyState & MK_SHIFT) == MK_SHIFT)
            operation = wkeWebDragOperationGeneric;

        return operation;
    }

    // IDropTarget impl
    HRESULT __stdcall DragEnter(IDataObject* pDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect) {
        if (!m_webview)
            return S_OK;

        m_dragData = nullptr;

        if (!m_dropTargetHelper)
            ::CoCreateInstance(CLSID_DragDropHelper, 0, CLSCTX_INPROC_SERVER, IID_IDropTargetHelper, (void**)&m_dropTargetHelper);

        if (m_dropTargetHelper)
            m_dropTargetHelper->DragEnter(m_viewWindow, pDataObject, (POINT*)&pt, *pdwEffect);

        POINT* screenPoint = new POINT();

        ::GetCursorPos(screenPoint);

        POINT* clientPoint = new POINT();
        *clientPoint = *screenPoint;
        ::ScreenToClient(m_viewWindow, clientPoint);

        int id = m_id;
        wkeWebView webview = m_webview;

        wkeWebDragData* data = dropDataToWebDragData(pDataObject);
        ThreadCall::callBlinkThreadAsync([data, id, webview, screenPoint, clientPoint, grfKeyState] {
            if (IdLiveDetect::get()->isLive(id)) {

                wkeWebDragOperation op = wkeDragTargetDragEnter(webview, data,
                    clientPoint, screenPoint,
                    keyStateToDragOperation(grfKeyState), keyStateToDragOperation(grfKeyState));
            }
            releaseWkeWebDragData(data);
            delete screenPoint;
            delete clientPoint;
        });

        *pdwEffect = DROPEFFECT_MOVE;
        //dragOperationToDragCursor(op);

        m_lastDropEffect = *pdwEffect;
        m_dragData = pDataObject;

        return S_OK;
    }

    HRESULT __stdcall DragOver(DWORD grfKeyState, POINTL pt, DWORD* pdwEffect) {
        if (!m_webview)
            return S_OK;

        if (m_dropTargetHelper)
            m_dropTargetHelper->DragOver((POINT*)&pt, *pdwEffect);

        if (m_dragData) {
            POINT* screenPoint = new POINT();

            ::GetCursorPos(screenPoint);

            POINT* clientPoint = new POINT();
            *clientPoint = *screenPoint;
            ::ScreenToClient(m_viewWindow, clientPoint);

            int id = m_id;
            wkeWebView webview = m_webview;
            ThreadCall::callBlinkThreadAsync([id, webview, screenPoint, clientPoint, grfKeyState] {
                if (IdLiveDetect::get()->isLive(id)) {
                    wkeWebDragOperation op = wkeDragTargetDragOver(webview,
                        clientPoint, screenPoint,
                        wkeWebDragOperationEvery, keyStateToDragOperation(grfKeyState));
                }
                delete screenPoint;
                delete clientPoint;
            });

            *pdwEffect = DROPEFFECT_MOVE; 
            //dragOperationToDragCursor(op);
        } else
            *pdwEffect = DROPEFFECT_NONE;

        m_lastDropEffect = *pdwEffect;
        return S_OK;
    }

    HRESULT __stdcall DragLeave() {
        if (m_dropTargetHelper)
            m_dropTargetHelper->DragLeave();

        if (m_dragData) {
            int id = m_id;
            wkeWebView webview = m_webview;
            ThreadCall::callBlinkThreadAsync([id, webview] {
                if (!IdLiveDetect::get()->isLive(id) || !webview)
                    return;
                wkeDragTargetDragLeave(webview);
            });
            m_dragData = nullptr;
        }
        return S_OK;
    }

    HRESULT __stdcall Drop(IDataObject* pDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect) {
        OutputDebugStringA("Drop\n");
        if (!m_webview)
            return S_OK;

        if (m_dropTargetHelper)
            m_dropTargetHelper->Drop(pDataObject, (POINT*)&pt, *pdwEffect);

        m_dragData = 0;
        *pdwEffect = m_lastDropEffect;

        POINT* screenPoint = new POINT();

        ::GetCursorPos(screenPoint);

        POINT* clientPoint = new POINT();
        *clientPoint = *screenPoint;
        ::ScreenToClient(m_viewWindow, clientPoint);

        int id = m_id;
        wkeWebView webview = m_webview;
        ThreadCall::callBlinkThreadAsync([id, webview, screenPoint, clientPoint, grfKeyState] {
            if (IdLiveDetect::get()->isLive(id) && webview)
                wkeDragTargetDrop(webview, clientPoint, screenPoint, keyStateToDragOperation(grfKeyState));

            delete screenPoint;
            delete clientPoint;
        });

        return S_OK;
    }

    HRESULT __stdcall QueryInterface(REFIID riid, void** ppvObject) {
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

    ULONG __stdcall AddRef() {
        return ++m_refCount;
    }

    ULONG __stdcall Release() {
        return --m_refCount;
    }

    ULONG getRefCount() const {
        return m_refCount;
    }

private:
    long m_refCount;
    int m_id;
    wkeWebView m_webview;
    HWND m_viewWindow;
    DWORD m_lastDropEffect;
    wkeWebDragOperationsMask m_mask;
    COMPtr<IDataObject> m_dragData;
    COMPtr<IDropTargetHelper> m_dropTargetHelper;
};

}

#endif // common_DragAction_h