/*
 * Copyright (C) 2007, 2014 Apple Inc.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#include "config.h"
#include "content/ui/WCDataObject.h"
#include "content/ui/ClipboardUtil.h"
#include "third_party/WebKit/Source/platform/weborigin/KURL.h"
#include <wtf/text/WTFString.h>

namespace content {

class WCEnumFormatEtc : public IEnumFORMATETC
{
public:
    WCEnumFormatEtc(const Vector<FORMATETC*>& formats);
    WCEnumFormatEtc(const Vector<FORMATETC>& formats);

    //IUnknown members
    HRESULT __stdcall QueryInterface(REFIID, void**) override;
    ULONG __stdcall AddRef(void) override;
    ULONG __stdcall Release(void) override;

    //IEnumFORMATETC members
    HRESULT __stdcall Next(ULONG, LPFORMATETC, ULONG*) override;
    HRESULT __stdcall Skip(ULONG) override;
    HRESULT __stdcall Reset(void) override;
    HRESULT __stdcall Clone(IEnumFORMATETC**) override;

private:
    long m_ref;
    Vector<FORMATETC> m_formats;
    size_t m_current;
};

WCEnumFormatEtc::WCEnumFormatEtc(const Vector<FORMATETC>& formats)
    : m_ref(1)
    , m_current(0)
{
    for(size_t i = 0; i < formats.size(); ++i)
        m_formats.append(formats[i]);
}

WCEnumFormatEtc::WCEnumFormatEtc(const Vector<FORMATETC*>& formats)
    : m_ref(1)
    , m_current(0)
{
    for (size_t i = 0; i < formats.size(); ++i)
        m_formats.append(*(formats[i]));
}

HRESULT WCEnumFormatEtc::QueryInterface(REFIID riid, void** ppvObject)
{
    *ppvObject = 0;
    if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IEnumFORMATETC)) {
        *ppvObject = this;
        AddRef();
        return S_OK;
    }

    return E_NOINTERFACE;
}

ULONG WCEnumFormatEtc::AddRef(void)
{
    return InterlockedIncrement(&m_ref);
}

ULONG WCEnumFormatEtc::Release(void)
{
    long c = InterlockedDecrement(&m_ref);
    if (c == 0)
        delete this;
    return c;
}

HRESULT WCEnumFormatEtc::Next(ULONG celt, LPFORMATETC lpFormatEtc, ULONG* pceltFetched)
{
    if(pceltFetched != 0)
        *pceltFetched=0;

    ULONG cReturn = celt;

    if(celt <= 0 || lpFormatEtc == 0 || m_current >= m_formats.size())
        return S_FALSE;

    if(pceltFetched == 0 && celt != 1) // pceltFetched can be 0 only for 1 item request
        return S_FALSE;

    while (m_current < m_formats.size() && cReturn > 0) {
        *lpFormatEtc++ = m_formats[m_current++];
        --cReturn;
    }
    if (pceltFetched != 0)
        *pceltFetched = celt - cReturn;

    return (cReturn == 0) ? S_OK : S_FALSE;
}

HRESULT WCEnumFormatEtc::Skip(ULONG celt)
{
    if((m_current + int(celt)) >= m_formats.size())
        return S_FALSE;
    m_current += celt;
    return S_OK;
}

HRESULT WCEnumFormatEtc::Reset(void)
{
    m_current = 0;
    return S_OK;
}

HRESULT WCEnumFormatEtc::Clone(IEnumFORMATETC** ppCloneEnumFormatEtc)
{
    if(!ppCloneEnumFormatEtc)
        return E_POINTER;

    WCEnumFormatEtc *newEnum = new WCEnumFormatEtc(m_formats);
    if(!newEnum)
        return E_OUTOFMEMORY;

    newEnum->AddRef();
    newEnum->m_current = m_current;
    *ppCloneEnumFormatEtc = newEnum;
    return S_OK;
}

//////////////////////////////////////////////////////////////////////////

HRESULT WCDataObject::createInstance(WCDataObject** result)
{
    if (!result)
        return E_POINTER;
    *result = new WCDataObject();
    return S_OK;
}

HRESULT WCDataObject::createInstance(WCDataObject** result, const DragDataMap& dataMap)
{
//     if (!result)
//         return E_POINTER;
//     *result = new WCDataObject;
// 
//     for (DragDataMap::const_iterator it = dataMap.begin(); it != dataMap.end(); ++it)
//         setClipboardData(*result, it->key, it->value);
    DebugBreak();
    return S_OK;
}

WCDataObject::WCDataObject()
    : m_ref(0)
{
}

STDMETHODIMP WCDataObject::QueryInterface(REFIID riid,void** ppvObject)
{
    *ppvObject = 0;
    if (IsEqualIID(riid, IID_IUnknown) || 
        IsEqualIID(riid, IID_IDataObject)) {
        *ppvObject=this;
    }
    if (*ppvObject) {
        AddRef();
        return S_OK;
    }
    return E_NOINTERFACE;
}

ULONG WCDataObject::AddRef( void)
{
    return InterlockedIncrement(&m_ref);
}

ULONG WCDataObject::Release( void)
{
    long c = InterlockedDecrement(&m_ref);
    if (c == 0)
        delete this;
    return c;
}

HRESULT WCDataObject::GetData(FORMATETC* pformatetcIn, STGMEDIUM* pmedium)
{ 
    if(!pformatetcIn || !pmedium)
        return E_POINTER;
    pmedium->hGlobal = 0;

    for (size_t i = 0; i < m_formats.size(); ++i) {
        if(/*pformatetcIn->tymed & m_formats[i]->tymed &&*/     // tymed can be 0 (TYMED_NULL) - but it can have a medium that contains an pUnkForRelease
            pformatetcIn->lindex == m_formats[i]->lindex &&
            pformatetcIn->dwAspect == m_formats[i]->dwAspect &&
            pformatetcIn->cfFormat == m_formats[i]->cfFormat) {
            CopyMedium(pmedium, m_medium[i], m_formats[i]);
            return S_OK;
        }
    }
    return DV_E_FORMATETC;
}

HRESULT WCDataObject::GetDataHere(FORMATETC*, STGMEDIUM*)
{ 
    return E_NOTIMPL;
}

HRESULT WCDataObject::QueryGetData(FORMATETC* pformatetc)
{ 
    if(!pformatetc)
        return E_POINTER;

    if (!(DVASPECT_CONTENT & pformatetc->dwAspect))
        return (DV_E_DVASPECT);
    HRESULT  hr = DV_E_TYMED;
    for (auto& format : m_formats) {
        if (pformatetc->tymed & format->tymed) {
            if (pformatetc->cfFormat == format->cfFormat)
                return S_OK;

            hr = DV_E_CLIPFORMAT;
        }
        else
            hr = DV_E_TYMED;
    }
    return hr;
}

HRESULT WCDataObject::GetCanonicalFormatEtc(FORMATETC*, FORMATETC*)
{ 
    return DATA_S_SAMEFORMATETC;
}

HRESULT WCDataObject::SetData(FORMATETC* pformatetc, STGMEDIUM* pmedium, BOOL fRelease)
{ 
    if(!pformatetc || !pmedium)
        return E_POINTER;

    FORMATETC* fetc = new FORMATETC();
    STGMEDIUM* pStgMed = new STGMEDIUM();

    memset(fetc, 0, sizeof(FORMATETC));
    memset(pStgMed, 0, sizeof(STGMEDIUM));

    *fetc = *pformatetc;
    m_formats.append(fetc);

    if(fRelease)
        *pStgMed = *pmedium;
    else
        CopyMedium(pStgMed, pmedium, pformatetc);
    m_medium.append(pStgMed);

    return S_OK;
}

void WCDataObject::CopyMedium(STGMEDIUM* pMedDest, STGMEDIUM* pMedSrc, FORMATETC* pFmtSrc)
{
    switch(pMedSrc->tymed)
    {
    case TYMED_HGLOBAL:
        pMedDest->hGlobal = (HGLOBAL)OleDuplicateData(pMedSrc->hGlobal,pFmtSrc->cfFormat, 0);
        break;
    case TYMED_GDI:
        pMedDest->hBitmap = (HBITMAP)OleDuplicateData(pMedSrc->hBitmap,pFmtSrc->cfFormat, 0);
        break;
    case TYMED_MFPICT:
        pMedDest->hMetaFilePict = (HMETAFILEPICT)OleDuplicateData(pMedSrc->hMetaFilePict,pFmtSrc->cfFormat, 0);
        break;
    case TYMED_ENHMF:
        pMedDest->hEnhMetaFile = (HENHMETAFILE)OleDuplicateData(pMedSrc->hEnhMetaFile,pFmtSrc->cfFormat, 0);
        break;
    case TYMED_FILE:
        pMedSrc->lpszFileName = (LPOLESTR)OleDuplicateData(pMedSrc->lpszFileName,pFmtSrc->cfFormat, 0);
        break;
    case TYMED_ISTREAM:
        pMedDest->pstm = pMedSrc->pstm;
        pMedSrc->pstm->AddRef();
        break;
    case TYMED_ISTORAGE:
        pMedDest->pstg = pMedSrc->pstg;
        pMedSrc->pstg->AddRef();
        break;
    default:
        break;
    }
    pMedDest->tymed = pMedSrc->tymed;
    pMedDest->pUnkForRelease = 0;
    if(pMedSrc->pUnkForRelease != 0) {
        pMedDest->pUnkForRelease = pMedSrc->pUnkForRelease;
        pMedSrc->pUnkForRelease->AddRef();
    }
}

HRESULT WCDataObject::EnumFormatEtc(DWORD dwDirection, IEnumFORMATETC** ppenumFormatEtc)
{ 
    if(!ppenumFormatEtc)
        return E_POINTER;

    *ppenumFormatEtc = nullptr;
    switch (dwDirection)
    {
    case DATADIR_GET:
        *ppenumFormatEtc = new WCEnumFormatEtc(m_formats);
        if(!(*ppenumFormatEtc))
            return E_OUTOFMEMORY;
        break;

    case DATADIR_SET:
    default:
        return E_NOTIMPL;
        break;
    }

    return S_OK;
}

HRESULT WCDataObject::DAdvise(FORMATETC*, DWORD, IAdviseSink*,DWORD*)
{ 
    return OLE_E_ADVISENOTSUPPORTED;
}

HRESULT WCDataObject::DUnadvise(DWORD)
{
    return E_NOTIMPL;
}

HRESULT WCDataObject::EnumDAdvise(IEnumSTATDATA**)
{
    return OLE_E_ADVISENOTSUPPORTED;
}

void WCDataObject::clearData(CLIPFORMAT format)
{
    size_t ptr = 0;
    while (ptr < m_formats.size()) {
        if (m_formats[ptr]->cfFormat == format) {
            delete m_formats[ptr];
            m_formats[ptr] = m_formats.last();
            m_formats.removeLast();

            ::ReleaseStgMedium(m_medium[ptr]);
            m_medium[ptr] = m_medium.last();
            m_medium.removeLast();
            continue;
        }
        ptr++;
    }
}


enum ClipboardDataType { ClipboardDataTypeNone, ClipboardDataTypeURL, ClipboardDataTypeText, ClipboardDataTypeTextHTML };

static ClipboardDataType clipboardTypeFromMIMEType(const String& type)
{
    String qType = type.stripWhiteSpace().lower();

    // two special cases for IE compatibility
    if (qType == "text" || qType == "text/plain" || qType.startsWith("text/plain;"))
        return ClipboardDataTypeText;
    if (qType == "url" || qType == "text/uri-list")
        return ClipboardDataTypeURL;
    if (qType == "text/html")
        return ClipboardDataTypeTextHTML;

    return ClipboardDataTypeNone;
}

static bool writeURL(WCDataObject *data, const blink::KURL& url, String title, bool withPlainText, bool withHTML)
{
    ASSERT(data);

    if (url.isEmpty())
        return false;

    if (title.isEmpty()) {
        title = url.lastPathComponent();
        if (title.isEmpty())
            title = url.host();
    }

    STGMEDIUM medium = { 0 };
    medium.tymed = TYMED_HGLOBAL;

    medium.hGlobal = ClipboardUtil::createGlobalData(url, title);
    bool success = false;
    if (medium.hGlobal && (data->SetData(ClipboardUtil::urlWFormat(), &medium, 1)) < 0)
        ::GlobalFree(medium.hGlobal);
    else
        success = true;

    if (withHTML) {
//         Vector<char> cfhtmlData;
//         ClipboardUtil::HtmlToCFHtml(ClipboardUtil::HtmlToCFHtml(title.utf8().data(), url.getUTF8String().utf8().data()), "", cfhtmlData);
//         medium.hGlobal = ClipboardUtil::createGlobalData(cfhtmlData);
//         if (medium.hGlobal && FAILED(data->SetData(ClipboardUtil::getHtmlFormatType(), &medium, TRUE)))
//             ::GlobalFree(medium.hGlobal);
//         else
//             success = true;
    }

    if (withPlainText) {
        Vector<UChar> url16 = WTF::ensureUTF16UChar(url.getUTF8String(), false);
        medium.hGlobal = ClipboardUtil::createGlobalData(url16);
        if (medium.hGlobal && (data->SetData(ClipboardUtil::getPlainTextWFormatType(), &medium, 1)) < 0)
            ::GlobalFree(medium.hGlobal);
        else
            success = true;
    }

    return success;
}

void WCDataObject::writeString(const String& type, const String& data)
{
    ClipboardDataType winType = clipboardTypeFromMIMEType(type);

    if (winType == ClipboardDataTypeURL) {
        writeURL(this, blink::KURL(blink::ParsedURLString, data), String(), false, true);
        return;
    }

    if (winType == ClipboardDataTypeText) {
        STGMEDIUM medium = { 0 };
        medium.tymed = TYMED_HGLOBAL;
        medium.hGlobal = ClipboardUtil::createGlobalData(WTF::ensureUTF16UChar(data, false));
        if (!medium.hGlobal)
            return;

        if ((SetData(ClipboardUtil::getPlainTextWFormatType(), &medium, 1)) < 0)
            ::GlobalFree(medium.hGlobal);
    }
}

}
