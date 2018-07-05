// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "content/web_impl_win/WebClipboardImpl.h"

#include "content/ui/ClipboardUtil.h"
#include "third_party/WebKit/public/platform/WebData.h"
#include "third_party/WebKit/public/platform/WebDragData.h"
#include "third_party/WebKit/public/platform/WebImage.h"
#include "third_party/WebKit/public/platform/WebSize.h"
#include "third_party/WebKit/public/platform/WebString.h"
#include "third_party/WebKit/public/platform/WebURL.h"
#include "third_party/WebKit/public/platform/WebVector.h"
#include "third_party/WebKit/Source/platform/geometry/IntSize.h"
#include "third_party/WebKit/Source/platform/image-encoders/gdiplus/GDIPlusImageEncoder.h"
#include "third_party/WebKit/Source/platform/image-encoders/skia/PNGImageEncoder.h"
#include "third_party/WebKit/Source/platform/clipboard/ClipboardMimeTypes.h"
#include "third_party/skia/include/core/SkBitmap.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "skia/ext/platform_canvas.h"
#include "skia/ext/bitmap_platform_device_win.h"
#include "wtf/text/WTFStringUtil.h"

#include <windows.h>

#if USING_VC6RT == 1
#define PURE = 0
#endif

#include <ShlObj.h>
#include <vector>

using blink::WebClipboard;
using blink::WebData;
using blink::WebDragData;
using blink::WebImage;
using blink::WebString;
using blink::WebURL;
using blink::WebVector;

namespace {

void freeData(unsigned int format, HANDLE data)
{
    if (format == CF_BITMAP)
        ::DeleteObject(static_cast<HBITMAP>(data));
    else
        ::GlobalFree(data);
}

bool bitmapHasInvalidPremultipliedColors(const SkBitmap& bitmap)
{
    for (int x = 0; x < bitmap.width(); ++x) {
        for (int y = 0; y < bitmap.height(); ++y) {
            uint32_t pixel = *bitmap.getAddr32(x, y);
            if (SkColorGetR(pixel) > SkColorGetA(pixel) ||
                SkColorGetG(pixel) > SkColorGetA(pixel) ||
                SkColorGetB(pixel) > SkColorGetA(pixel))
                return true;
        }
    }
    return false;
}

void makeBitmapOpaque(const SkBitmap& bitmap)
{
    for (int x = 0; x < bitmap.width(); ++x) {
        for (int y = 0; y < bitmap.height(); ++y) {
            *bitmap.getAddr32(x, y) = SkColorSetA(*bitmap.getAddr32(x, y), 0xFF);
        }
    }
}

template <class str>
void appendEscapedCharForHTMLImpl(typename str::value_type c, str* output)
{
    static const struct {
        char key;
        const char* replacement;
    } kCharsToEscape[] = {
        { '<', "&lt;" },
        { '>', "&gt;" },
        { '&', "&amp;" },
        { '"', "&quot;" },
        { '\'', "&#39;" },
    };
    size_t k;
    for (k = 0; k < arraysize(kCharsToEscape); ++k) {
        if (c == kCharsToEscape[k].key) {
            const char* p = kCharsToEscape[k].replacement;
            while (*p)
                //output->push_back(*p++);
                output += (*p++);
            break;
        }
    }
    if (k == arraysize(kCharsToEscape))
        //output->push_back(c);
        output += c;
}

template <class str>
str escapeForHTMLImpl(const str& input)
{
    str result;
    result.reserve(input.size());  // Optimize for no escaping.

    for (typename str::const_iterator i = input.begin(); i != input.end(); ++i)
        appendEscapedCharForHTMLImpl(*i, &result);

    return result;
}

std::string escapeForHTML(const Vector<char>& input)
{
    std::string inputStr(input.data(), input.size());
    return escapeForHTMLImpl(inputStr);
}

std::string escapeForHTML(const std::string input)
{
    return escapeForHTMLImpl(input);
}

std::string URLToMarkup(const blink::WebURL& url, const blink::WebString& title)
{
    std::string markup("<a href=\"");
    markup.append(WTF::ensureStringToUTF8(url.string(), true).data());
    markup.append("\">");
    // TODO(darin): HTML escape this
    markup.append(escapeForHTML(WTF::ensureStringToUTF8(title, true)));
    markup.append("</a>");
    return markup;
}

std::string URLToImageMarkup(const blink::WebURL& url, const blink::WebString& title)
{
    std::string markup("<img src=\"");
    markup.append(escapeForHTML(WTF::ensureStringToUTF8(url.string(), true).data()));
    markup.append("\"");
    if (!title.isEmpty()) {
        markup.append(" alt=\"");
        markup.append(escapeForHTML(WTF::ensureStringToUTF8(title, true)));
        markup.append("\"");
    }
    markup.append("/>");
    return markup;
}

// A scoper to impersonate the anonymous token and revert when leaving scope
class AnonymousImpersonator {
public:
    AnonymousImpersonator() {
        must_revert_ = ::ImpersonateAnonymousToken(::GetCurrentThread());
    }

    ~AnonymousImpersonator() {
        if (must_revert_)
            ::RevertToSelf();
    }

private:
    BOOL must_revert_;
    DISALLOW_COPY_AND_ASSIGN(AnonymousImpersonator);
};

// A scoper to manage acquiring and automatically releasing the clipboard.
class ScopedClipboard {
public:
    ScopedClipboard() : m_isOpened(false) { }

    ~ScopedClipboard()
    {
        if (m_isOpened)
            release();
    }

    bool acquire(HWND owner)
    {
        const int kMaxAttemptsToOpenClipboard = 5;

        if (m_isOpened) {
            notImplemented();
            return false;
        }

        // Attempt to open the clipboard, which will acquire the Windows clipboard
        // lock.  This may fail if another process currently holds this lock.
        // We're willing to try a few times in the hopes of acquiring it.
        //
        // This turns out to be an issue when using remote desktop because the
        // rdpclip.exe process likes to read what we've written to the clipboard and
        // send it to the RDP client.  If we open and close the clipboard in quick
        // succession, we might be trying to open it while rdpclip.exe has it open,
        // See Bug 815425.
        //
        // In fact, we believe we'll only spin this loop over remote desktop.  In
        // normal situations, the user is initiating clipboard operations and there
        // shouldn't be contention.

        for (int attempts = 0; attempts < kMaxAttemptsToOpenClipboard; ++attempts) {
            // If we didn't manage to open the clipboard, sleep a bit and be hopeful.
            if (attempts != 0)
                ::Sleep(5);

            if (::OpenClipboard(owner)) {
                m_isOpened = true;
                return true;
            }
        }

        // We failed to acquire the clipboard.
        return false;
    }

    void release() {
        if (m_isOpened) {
            // Impersonate the anonymous token during the call to CloseClipboard
            // This prevents Windows 8+ capturing the broker's access token which
            // could be accessed by lower-privileges chrome processes leading to
            // a risk of EoP
            AnonymousImpersonator impersonator;
            ::CloseClipboard();
            m_isOpened = false;
        }
        else {
            notImplemented();
        }
    }

private:
    bool m_isOpened;
};

}

namespace content {

WebClipboardImpl::WebClipboardImpl() 
    : m_clipboardOwner(NULL)
{
    m_clipboardOwner = getClipboardWindow();
}

WebClipboardImpl::~WebClipboardImpl()
{
}

uint64 WebClipboardImpl::sequenceNumber(Buffer buffer)
{
    return ::GetClipboardSequenceNumber();
}

bool WebClipboardImpl::isFormatAvailable(Format format, Buffer buffer)
{
    ClipboardType clipboardType = CLIPBOARD_TYPE_COPY_PASTE;
    if (!convertBufferType(buffer, &clipboardType))
        return false;
    ASSERT(clipboardType == CLIPBOARD_TYPE_COPY_PASTE);

    switch (format) {
    case FormatPlainText:
        return ::IsClipboardFormatAvailable(CF_UNICODETEXT) || ::IsClipboardFormatAvailable(CF_TEXT);
    case FormatHTML: {
        return ::IsClipboardFormatAvailable(ClipboardUtil::getHtmlFormatType());
    }
    case FormatSmartPaste:
        return ::IsClipboardFormatAvailable(ClipboardUtil::getWebKitSmartPasteFormatType());
    case FormatBookmark:
        return ::IsClipboardFormatAvailable(ClipboardUtil::getUrlWFormatType());
    default:
        notImplemented();
    }

    return false;
}

WebVector<WebString> WebClipboardImpl::readAvailableTypes(Buffer buffer, bool* containsFilenames) {
    ClipboardType clipboardType;
    Vector<WebString> types;
    if (convertBufferType(buffer, &clipboardType)) {
        readAvailableTypes(clipboardType, &types, containsFilenames);
    }
    return types;
}

void WebClipboardImpl::readAvailableTypes(ClipboardType type, Vector<WebString>* types, bool* containsFilenames) const
{
    if (!types || !containsFilenames) {
        notImplemented();
        return;
    }

    types->clear();
    if (::IsClipboardFormatAvailable(CF_TEXT))
        types->append(WebString::fromUTF8(kMimeTypeText));
    if (::IsClipboardFormatAvailable(ClipboardUtil::getHtmlFormatType()))
        types->append(WebString::fromUTF8(kMimeTypeHTML));
    if (::IsClipboardFormatAvailable(ClipboardUtil::getRtfFormatType()))
        types->append(WebString::fromUTF8(kMimeTypeRTF));
    if (::IsClipboardFormatAvailable(CF_DIB))
        types->append(WebString::fromUTF8(kMimeTypePNG));
    if (::IsClipboardFormatAvailable(CF_BITMAP))
        types->append(WebString::fromUTF8(kMimeTypeBMP));

    *containsFilenames = false;
}

WebString WebClipboardImpl::readPlainText(Buffer buffer) {
    ClipboardType clipboard_type;
    if (!convertBufferType(buffer, &clipboard_type))
        return WebString();

    // Acquire the clipboard.
    ScopedClipboard clipboard;
    if (!clipboard.acquire(getClipboardWindow()))
        return WebString();

    HANDLE data = ::GetClipboardData(CF_UNICODETEXT);
    if (!data)
        return WebString();

    LPCWSTR dataText = (LPCWSTR)::GlobalLock(data);
    String text(dataText, wcslen(dataText));
    ::GlobalUnlock(data);
    return text;
}

WebString WebClipboardImpl::readHTML(Buffer buffer, WebURL* source_url,
    unsigned* fragment_start,
    unsigned* fragment_end) {
    ClipboardType clipboardType;
    if (!convertBufferType(buffer, &clipboardType))
        return WebString();

    ASSERT(clipboardType == CLIPBOARD_TYPE_COPY_PASTE);

    std::string srcUrl;

    *fragment_start = 0;
    *fragment_end = 0;

    // Acquire the clipboard.
    ScopedClipboard clipboard;
    if (!clipboard.acquire(getClipboardWindow()))
        return WebString();

    HANDLE data = ::GetClipboardData(ClipboardUtil::getHtmlFormatType());
    if (!data)
        return WebString();

    std::string cf_html(static_cast<const char*>(::GlobalLock(data)));
    ::GlobalUnlock(data);

    size_t htmlStart = std::string::npos;
    size_t startIndex = std::string::npos;
    size_t endIndex = std::string::npos;
    ClipboardUtil::CFHtmlExtractMetadata(cf_html, &srcUrl, &htmlStart, &startIndex, &endIndex);

    // This might happen if the contents of the clipboard changed and CF_HTML is
    // no longer available.
    if (startIndex == std::string::npos || endIndex == std::string::npos || htmlStart == std::string::npos)
        return WebString();

    if (startIndex < htmlStart || endIndex < startIndex)
        return WebString();

    return WebString::fromUTF8(cf_html.data() + htmlStart);
}

WebData WebClipboardImpl::readImage(Buffer buffer)
{
    ClipboardType clipboardType;
    if (!convertBufferType(buffer, &clipboardType))
        return WebData();
        
    // Acquire the clipboard.
    ScopedClipboard clipboard;
    if (!clipboard.acquire(getClipboardWindow()))
        return WebData();

    // We use a DIB rather than a DDB here since ::GetObject() with the
    // HBITMAP returned from ::GetClipboardData(CF_BITMAP) always reports a color
    // depth of 32bpp.
    HANDLE hBitmap = ::GetClipboardData(CF_DIB);
    if (!hBitmap)
        return WebData();

    BITMAPINFO* bitmap = static_cast<BITMAPINFO*>(GlobalLock(hBitmap));
    if (!bitmap)
        return WebData();
    int colorTableLength = 0;
    switch (bitmap->bmiHeader.biBitCount) {
    case 1:
    case 4:
    case 8:
        colorTableLength = bitmap->bmiHeader.biClrUsed ? bitmap->bmiHeader.biClrUsed : 1 << bitmap->bmiHeader.biBitCount;
        break;
    case 16:
    case 32:
        if (bitmap->bmiHeader.biCompression == BI_BITFIELDS)
            colorTableLength = 3;
        break;
    case 24:
        break;
    default:
        notImplemented();
    }
    const void* bitmapBits = reinterpret_cast<const char*>(bitmap) + bitmap->bmiHeader.biSize + colorTableLength * sizeof(RGBQUAD);

    WTF::PassOwnPtr<SkCanvas> canvas = WTF::adoptPtr(skia::CreatePlatformCanvas(bitmap->bmiHeader.biWidth, bitmap->bmiHeader.biHeight, false));
    skia::BitmapPlatformDevice* device = (skia::BitmapPlatformDevice*)skia::GetPlatformDevice(skia::GetTopDevice(*canvas));
    if (!device) {
        GlobalUnlock(hBitmap);
        return WebData();
    }
    HDC dc = device->GetBitmapDCUgly(getClipboardWindow());
    ::SetDIBitsToDevice(dc, 0, 0, bitmap->bmiHeader.biWidth,
        bitmap->bmiHeader.biHeight, 0, 0, 0,
        bitmap->bmiHeader.biHeight, bitmapBits, bitmap,
        DIB_RGB_COLORS);
    const SkBitmap& skBitmap = device->accessBitmap(false);

    // Windows doesn't really handle alpha channels well in many situations. When
    // the source image is < 32 bpp, we force the bitmap to be opaque. When the
    // source image is 32 bpp, the alpha channel might still contain garbage data.
    // Since Windows uses premultiplied alpha, we scan for instances where
    // (R, G, B) > A. If there are any invalid premultiplied colors in the image,
    // we assume the alpha channel contains garbage and force the bitmap to be
    // opaque as well. Note that this  heuristic will fail on a transparent bitmap
    // containing only black pixels...
    {
        SkAutoLockPixels lock(skBitmap);
        bool hasInvalidAlphaChannel = bitmap->bmiHeader.biBitCount < 32 || bitmapHasInvalidPremultipliedColors(skBitmap);
        if (hasInvalidAlphaChannel)
            makeBitmapOpaque(skBitmap);
    }

    GlobalUnlock(hBitmap);

    Vector<unsigned char> output;
#if 0
    blink::GDIPlusImageEncoder::encode(skBitmap, blink::GDIPlusImageEncoder::PNG, &output);
#endif
    blink::PNGImageEncoder::encode(skBitmap, &output);
    return WebData((const char*)output.data(), output.size());
}

WebString WebClipboardImpl::readCustomData(Buffer buffer, const WebString& type)
{
    return WebString();
}

void WebClipboardImpl::writeToClipboard(unsigned int format, HANDLE handle)
{
    ScopedClipboard clipboard;
    if (!clipboard.acquire(getClipboardWindow()))
        return;

    ASSERT(m_clipboardOwner != NULL);
    if (handle && !::SetClipboardData(format, handle)) {
        ASSERT(ERROR_CLIPBOARD_NOT_OPEN != GetLastError());
        freeData(format, handle);
    }
}

void WebClipboardImpl::writeText(String string)
{
    HGLOBAL glob = ClipboardUtil::createGlobalData(WTF::WTFStringToStdString(string));
    writeToClipboard(CF_UNICODETEXT, glob);
}

void WebClipboardImpl::clearClipboard()
{
    ScopedClipboard clipboard;
    if (clipboard.acquire(getClipboardWindow())) {
        EmptyClipboard();
    }
}

void WebClipboardImpl::writePlainText(const WebString& plainText)
{
    clearClipboard();
    writeText(plainText);
}

void WebClipboardImpl::writeHTML(const WebString& htmlText, const WebURL& sourceUrl, const WebString& plainText, bool writeSmartPaste)
{
    clearClipboard();
    writeHTMLInternal(htmlText, sourceUrl, plainText, writeSmartPaste);
}

void WebClipboardImpl::writeHTMLInternal(const WebString& htmlText, const WebURL& sourceUrl, const WebString& plainText, bool writeSmartPaste)
{
    std::string markup = WTF::WTFStringToStdString(htmlText);
    std::string url;

    WTF::String urlString = sourceUrl.string();
    if (!urlString.isNull() && !urlString.isEmpty())
        url = WTFStringToStdString(urlString);

    std::string htmlFragment = ClipboardUtil::HtmlToCFHtml(markup, url);
    HGLOBAL glob = ClipboardUtil::createGlobalData(htmlFragment);
    // writeToClipboard(ClipboardUtil::getHtmlFormatType(), glob);
    writeText(plainText);

    if (writeSmartPaste) {
        ASSERT(m_clipboardOwner != NULL);
        ::SetClipboardData(ClipboardUtil::getWebKitSmartPasteFormatType(), NULL);
    }

    ::GlobalUnlock(glob);
}

void WebClipboardImpl::writeBitmapFromHandle(HBITMAP source_hbitmap, const blink::IntSize& size)
{
    // We would like to just call ::SetClipboardData on the source_hbitmap,
    // but that bitmap might not be of a sort we can write to the clipboard.
    // For this reason, we create a new bitmap, copy the bits over, and then
    // write that to the clipboard.

    HDC dc = ::GetDC(NULL);
    HDC compatible_dc = ::CreateCompatibleDC(NULL);
    HDC source_dc = ::CreateCompatibleDC(NULL);

    // This is the HBITMAP we will eventually write to the clipboard
    HBITMAP hbitmap = ::CreateCompatibleBitmap(dc, size.width(), size.height());
    if (!hbitmap) {
        // Failed to create the bitmap
        ::DeleteDC(compatible_dc);
        ::DeleteDC(source_dc);
        ::ReleaseDC(NULL, dc);
        return;
    }

    HBITMAP old_hbitmap = (HBITMAP)SelectObject(compatible_dc, hbitmap);
    HBITMAP old_source = (HBITMAP)SelectObject(source_dc, source_hbitmap);

    // Now we need to blend it into an HBITMAP we can place on the clipboard
    BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
    ::GdiAlphaBlend(compatible_dc,
        0,
        0,
        size.width(),
        size.height(),
        source_dc,
        0,
        0,
        size.width(),
        size.height(),
        bf);

    // Clean up all the handles we just opened
    ::SelectObject(compatible_dc, old_hbitmap);
    ::SelectObject(source_dc, old_source);
    ::DeleteObject(old_hbitmap);
    ::DeleteObject(old_source);
    ::DeleteDC(compatible_dc);
    ::DeleteDC(source_dc);
    ::ReleaseDC(NULL, dc);

    writeToClipboard(CF_BITMAP, hbitmap);
}

bool WebClipboardImpl::writeBitmap(const SkBitmap& bitmap)
{
    HDC dc = ::GetDC(NULL);

    // This doesn't actually cost us a memcpy when the bitmap comes from the
    // renderer as we load it into the bitmap using setPixels which just sets a
    // pointer.  Someone has to memcpy it into GDI, it might as well be us here.

    // TODO(darin): share data in gfx/bitmap_header.cc somehow
    BITMAPINFO bm_info = {};
    bm_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bm_info.bmiHeader.biWidth = bitmap.width();
    bm_info.bmiHeader.biHeight = -bitmap.height();  // sets vertical orientation
    bm_info.bmiHeader.biPlanes = 1;
    bm_info.bmiHeader.biBitCount = 32;
    bm_info.bmiHeader.biCompression = BI_RGB;

    // ::CreateDIBSection allocates memory for us to copy our bitmap into.
    // Unfortunately, we can't write the created bitmap to the clipboard,
    // (see http://msdn2.microsoft.com/en-us/library/ms532292.aspx)
    void* bits;
    HBITMAP source_hbitmap = ::CreateDIBSection(dc, &bm_info, DIB_RGB_COLORS, &bits, NULL, 0);

    if (bits && source_hbitmap) {
        {
            SkAutoLockPixels bitmap_lock(bitmap);
            // Copy the bitmap out of shared memory and into GDI
            memcpy(bits, bitmap.getPixels(), bitmap.getSize());
        }

        // Now we have an HBITMAP, we can write it to the clipboard
        writeBitmapFromHandle(source_hbitmap, blink::IntSize(bitmap.width(), bitmap.height()));
    }

    ::DeleteObject(source_hbitmap);
    ::ReleaseDC(NULL, dc);
    return true;
}

void WebClipboardImpl::writeBookmark(const String& titleData , const String& urlData)
{
    String bookmark = WTF::ensureUTF16String(titleData);
    bookmark.append(L'\n');
    bookmark.append(WTF::ensureUTF16String(urlData));

    std::string wideBookmark = WTF::WTFStringToStdString(bookmark);
    HGLOBAL glob = ClipboardUtil::createGlobalData(wideBookmark);

    writeToClipboard(ClipboardUtil::getUrlWFormatType(), glob);
}

void WebClipboardImpl::writeImage(const WebImage& image, const WebURL& url, const WebString& title)
{
    clearClipboard();

    ASSERT(!image.isNull());
    const SkBitmap& bitmap = image.getSkBitmap();
    if (!writeBitmap(bitmap))
        return;

    return; // weolar

    if (!url.isEmpty()) {
        writeBookmark(url.string(), title);
#if !defined(OS_MACOSX)
        // When writing the image, we also write the image markup so that pasting
        // into rich text editors, such as Gmail, reveals the image. We also don't
        // want to call writeText(), since some applications (WordPad) don't pick
        // the image if there is also a text format on the clipboard.
        // We also don't want to write HTML on a Mac, since Mail.app prefers to use
        // the image markup over attaching the actual image. See
        // http://crbug.com/33016 for details.
        writeHTML(WebString::fromUTF8(URLToImageMarkup(url, title)), WebURL(), WebString(), false);
#endif
    }
}

void WebClipboardImpl::writeDataObject(const WebDragData& data)
{
    // TODO(dcheng): Properly support text/uri-list here.
    // Avoid calling the WriteFoo functions if there is no data associated with a
    // type. This prevents stomping on clipboard contents that might have been
    // written by extension functions such as chrome.bookmarkManagerPrivate.copy.

    // DataObject::toWebDragData()
    clearClipboard();

    WebVector<WebDragData::Item> items = data.items();
    for (size_t i = 0; i < items.size(); ++i) {
        WebDragData::Item& it = items[i];

        if (WebDragData::Item::StorageTypeString == it.storageType) {
            String stringType = it.stringType;
            if (blink::mimeTypeTextPlain == stringType || blink::mimeTypeTextPlainEtc == stringType) {
                writeText(it.stringData);
            } else if (blink::mimeTypeTextHTML == stringType) {
                //writeHTMLInternal(it.stringData, it.baseURL, WebString(), false);
            }
        }
            
//         if (!data_object.html.is_null())
//             writeHTML(ui::CLIPBOARD_TYPE_COPY_PASTE, data_object.html.string(), GURL());
//         if (!data_object.custom_data.empty())
//             writeCustomData(ui::CLIPBOARD_TYPE_COPY_PASTE, data_object.custom_data);
    }
}

bool WebClipboardImpl::convertBufferType(Buffer buffer, ClipboardType* result) {
    *result = CLIPBOARD_TYPE_COPY_PASTE;
    switch (buffer) {
    case BufferStandard:
        break;
    case BufferSelection:
        // Chrome OS and non-X11 unix builds do not support
        // the X selection clipboad.
        // TODO: remove the need for this case, see http://crbug.com/361753
        return false;
    default:
        notImplemented();
        return false;
    }
    return true;
}

extern "C" LRESULT __stdcall clipboardOwnerWndProc(HWND hWnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    LRESULT result = 0;
    switch (message) {
    case WM_RENDERFORMAT:
        // This message comes when SetClipboardData was sent a null data handle
        // and now it's come time to put the data on the clipboard.
        // We always set data, so there isn't a need to actually do anything here.
        break;
    case WM_RENDERALLFORMATS:
        // This message comes when SetClipboardData was sent a null data handle
        // and now this application is about to quit, so it must put data on
        // the clipboard before it exits.
        // We always set data, so there isn't a need to actually do anything here.
        break;
    case WM_DRAWCLIPBOARD:
        break;
    case WM_DESTROY:
        break;
    case WM_CHANGECBCHAIN:
        break;
    default:
        return DefWindowProc(hWnd, message, wparam, lparam);
    }

    return result;
}

HWND WebClipboardImpl::getClipboardWindow()
{
    if (INVALID_HANDLE_VALUE != m_clipboardOwner && NULL != m_clipboardOwner)
        return m_clipboardOwner;

    WNDCLASSEX window_class;
    window_class.cbSize = sizeof(window_class);
    window_class.style = 0;
    window_class.lpfnWndProc = clipboardOwnerWndProc;
    window_class.cbClsExtra = 0;
    window_class.cbWndExtra = 0;
    window_class.hInstance = nullptr;
    window_class.hIcon = NULL;
    window_class.hCursor = NULL;
    window_class.hbrBackground = NULL;
    window_class.lpszMenuName = NULL;
    window_class.lpszClassName = L"Chrome_MessageWindow";
    window_class.hIconSm = NULL;
    ATOM atom = RegisterClassEx(&window_class);

    m_clipboardOwner = ::CreateWindow(MAKEINTATOM(atom), L"window_name", 0, 0, 0,
        0, 0, HWND_MESSAGE, 0, NULL, NULL);

    return m_clipboardOwner;

}

}  // namespace content
