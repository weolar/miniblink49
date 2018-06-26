// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Some helper functions for working with the clipboard and IDataObjects.

#include "content/ui/ClipboardUtil.h"

#include "base/strings/string_util.h"

// #if USING_VC6RT == 1
// #define PURE = 0
// #endif
// #include <ShlObj.h>

namespace content {

unsigned int ClipboardUtil::getHtmlFormatType()
{
    static unsigned int s_HtmlFormatType = ::RegisterClipboardFormat(L"HTML Format");
    return s_HtmlFormatType;
}


unsigned int ClipboardUtil::getWebKitSmartPasteFormatType()
{
    static unsigned int s_WebKitSmartPasteFormatType = ::RegisterClipboardFormat(L"WebKit Smart Paste Format");
    return s_WebKitSmartPasteFormatType;
}

unsigned int ClipboardUtil::getUrlWFormatType()
{
    static unsigned int s_UrlWFormatType = ::RegisterClipboardFormat(CFSTR_INETURLW);
    return s_UrlWFormatType;
}

unsigned int ClipboardUtil::getRtfFormatType()
{
    static unsigned int s_RtfFormatType = ::RegisterClipboardFormat(L"Rich Text Format");
    return s_RtfFormatType;
}

const unsigned int ClipboardUtil::getWebCustomDataFormatType()
{
    // TODO(dcheng): This name is temporary. See http://crbug.com/106449.
    static unsigned int s_WebCustomDataFormatType = ::RegisterClipboardFormat(L"Chromium Web Custom MIME Data Format");
    return s_WebCustomDataFormatType;
}

FORMATETC* ClipboardUtil::getPlainTextWFormatType()
{
    static FORMATETC textFormat = { CF_UNICODETEXT, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    return &textFormat;
}

FORMATETC* ClipboardUtil::getPlainTextFormatType()
{
    static FORMATETC textFormat = { CF_UNICODETEXT, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    return &textFormat;
}

FORMATETC* ClipboardUtil::urlWFormat()
{
    unsigned int cf = getUrlWFormatType();
    static FORMATETC urlFormat = { (CLIPFORMAT)cf, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    return &urlFormat;
}

FORMATETC* ClipboardUtil::urlFormat()
{
    static unsigned int cf = RegisterClipboardFormat(L"UniformResourceLocator");
    static FORMATETC urlFormat = { (CLIPFORMAT)cf, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    return &urlFormat;
}

bool ClipboardUtil::getWebLocData(IDataObject* dataObject, std::string& url, std::string* title)
{
    bool succeeded = false;
    return succeeded;
}

std::string ClipboardUtil::extractURL(const std::string& inURL, std::string* title)
{
    std::string url = inURL;
    int splitLoc = url.find('\n');
    if (splitLoc > 0) {
        if (title)
            *title = url.substr(splitLoc + 1);
        url = url.substr(0, splitLoc);
    } else if (title)
        *title = url;
    return url;
}

std::string ClipboardUtil::getURL(IDataObject* dataObject, std::string* title)
{
    STGMEDIUM store;
    std::string url;
    if (getWebLocData(dataObject, url, title))
        return url;

    if (dataObject->GetData(urlWFormat(), &store) >= 0) {
        // URL using Unicode
        wchar_t* data = static_cast<wchar_t*>(GlobalLock(store.hGlobal));

        std::string dataA = base::WideToUTF8(std::wstring(data, wcslen(data)));
        url = extractURL(dataA, title);
        GlobalUnlock(store.hGlobal);
        ReleaseStgMedium(&store);
    } else if (dataObject->GetData(urlFormat(), &store) >= 0) {
        // URL using ASCII
        char* data = static_cast<char*>(GlobalLock(store.hGlobal));
        url = extractURL(std::string(data), title);
        GlobalUnlock(store.hGlobal);
        ReleaseStgMedium(&store);
    }
    return url;
}

std::string ClipboardUtil::getPlainText(IDataObject* dataObject)
{
    STGMEDIUM store;
    std::string text;
    if (SUCCEEDED(dataObject->GetData(getPlainTextWFormatType(), &store))) {
        // Unicode text
        wchar_t* data = static_cast<wchar_t*>(::GlobalLock(store.hGlobal));
        text = base::WideToUTF8(std::wstring(data, wcslen(data)));
        GlobalUnlock(store.hGlobal);
        ReleaseStgMedium(&store);
    } else if (SUCCEEDED(dataObject->GetData(getPlainTextFormatType(), &store))) {
        // ASCII text
        char* data = static_cast<char*>(GlobalLock(store.hGlobal));
        text = (data);
        GlobalUnlock(store.hGlobal);
        ReleaseStgMedium(&store);
    } else {
        // FIXME: Originally, we called getURL() here because dragging and dropping files doesn't
        // populate the drag with text data. Per https://bugs.webkit.org/show_bug.cgi?id=38826, this
        // is undesirable, so maybe this line can be removed.
        text = getURL(dataObject, nullptr);
    }
    return text;
}

HGLOBAL ClipboardUtil::createGlobalData(const std::string& url, const std::string& title)
{
    std::wstring mutableURL(base::UTF8ToWide(url));
    std::wstring mutableTitle(base::UTF8ToWide(title));

    SIZE_T size = mutableURL.size() + mutableTitle.size() + 2; // +1 for "\n" and +1 for null terminator
    HGLOBAL cbData = ::GlobalAlloc(GPTR, size * sizeof(wchar_t));

    if (cbData) {
        PWSTR buffer = static_cast<PWSTR>(GlobalLock(cbData));
        _snwprintf(buffer, size, L"%s\n%s", &mutableURL[0], &mutableTitle[0]);
        GlobalUnlock(cbData);
    }
    return cbData;
}

HGLOBAL ClipboardUtil::createGlobalData(const std::string& str)
{
    std::wstring strW(base::UTF8ToWide(str));
    HGLOBAL data = ::GlobalAlloc(GMEM_MOVEABLE, ((strW.size() + 1) * sizeof(wchar_t)));
    if (data) {
        wchar_t* rawData = static_cast<wchar_t*>(::GlobalLock(data));
        memcpy(rawData, &strW[0], strW.size() * sizeof(wchar_t));
        rawData[strW.size()] = L'\0';
        ::GlobalUnlock(data);
    }
    return data;
}

std::string ClipboardUtil::HtmlToCFHtml(const std::string& html, const std::string& base_url)
{
    if (html.empty())
        return std::string();

#define MAX_DIGITS 10
#define MAKE_NUMBER_FORMAT_1(digits) MAKE_NUMBER_FORMAT_2(digits)
#define MAKE_NUMBER_FORMAT_2(digits) "%0" #digits "u"
#define NUMBER_FORMAT MAKE_NUMBER_FORMAT_1(MAX_DIGITS)

    static const char* header = "Version:0.9\r\n"
        "StartHTML:" NUMBER_FORMAT "\r\n"
        "EndHTML:" NUMBER_FORMAT "\r\n"
        "StartFragment:" NUMBER_FORMAT "\r\n"
        "EndFragment:" NUMBER_FORMAT "\r\n";
    static const char* source_url_prefix = "SourceURL:";

    static const char* start_markup =
        "<html>\r\n<body>\r\n<!--StartFragment-->";
    static const char* end_markup =
        "<!--EndFragment-->\r\n</body>\r\n</html>";

    // Calculate offsets
    size_t start_html_offset = strlen(header) - strlen(NUMBER_FORMAT) * 4 +
        MAX_DIGITS * 4;
    if (!base_url.empty()) {
        start_html_offset += strlen(source_url_prefix) +
            base_url.length() + 2;  // Add 2 for \r\n.
    }
    size_t start_fragment_offset = start_html_offset + strlen(start_markup);
    size_t end_fragment_offset = start_fragment_offset + html.length();
    size_t end_html_offset = end_fragment_offset + strlen(end_markup);

    std::vector<char> buffer;
    buffer.resize(2000);
    sprintf(&buffer[0], header, start_html_offset, end_html_offset, start_fragment_offset, end_fragment_offset);
    std::string result = &buffer[0];

    if (!base_url.empty()) {
        result.append(source_url_prefix);
        result.append(base_url.c_str());
        result.append("\r\n");
    }
    result.append(start_markup);
    result.append(html.c_str());
    result.append(end_markup);

#undef MAX_DIGITS
#undef MAKE_NUMBER_FORMAT_1
#undef MAKE_NUMBER_FORMAT_2
#undef NUMBER_FORMAT

    return result;
}

void ClipboardUtil::CFHtmlExtractMetadata(const std::string& cf_html,
    std::string* base_url,
    size_t* html_start,
    size_t* fragment_start,
    size_t* fragment_end)
{
    // Obtain base_url if present.
    if (base_url) {
        static std::string src_url_str("SourceURL:");
        size_t line_start = cf_html.find(src_url_str);
        if (line_start != std::string::npos) {
            size_t src_end = cf_html.find("\n", line_start);
            size_t src_start = line_start + src_url_str.length();
            if (src_end != std::string::npos && src_start != std::string::npos) {
                *base_url = cf_html.substr(src_start, src_end - src_start);
                base::TrimWhitespace(*base_url, base::TRIM_ALL, base_url);
            }
        }
    }

    // Find the markup between "<!--StartFragment-->" and "<!--EndFragment-->".
    // If the comments cannot be found, like copying from OpenOffice Writer,
    // we simply fall back to using StartFragment/EndFragment bytecount values
    // to determine the fragment indexes.
    std::string cf_html_lower = base::ToLowerASCII(cf_html);
    size_t markup_start = cf_html_lower.find("<html", 0);
    if (html_start) {
        *html_start = markup_start;
    }
    size_t tag_start = cf_html.find("<!--StartFragment", markup_start);
    if (tag_start == std::string::npos) {
        static std::string start_fragment_str("StartFragment:");
        size_t start_fragment_start = cf_html.find(start_fragment_str);
        if (start_fragment_start != std::string::npos) {
            *fragment_start = static_cast<size_t>(atoi(cf_html.c_str() +
                start_fragment_start + start_fragment_str.length()));
        }

        static std::string end_fragment_str("EndFragment:");
        size_t end_fragment_start = cf_html.find(end_fragment_str);
        if (end_fragment_start != std::string::npos) {
            *fragment_end = static_cast<size_t>(atoi(cf_html.c_str() +
                end_fragment_start + end_fragment_str.length()));
        }
    } else {
        *fragment_start = cf_html.find('>', tag_start) + 1;
        size_t tag_end = cf_html.rfind("<!--EndFragment", std::string::npos);
        *fragment_end = cf_html.rfind('<', tag_end);
    }
}

}