// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Some helper functions for working with the clipboard and IDataObjects.

#ifndef UI_BASE_CLIPBOARD_CLIPBOARD_UTIL_WIN_H_
#define UI_BASE_CLIPBOARD_CLIPBOARD_UTIL_WIN_H_

#include "third_party/WebKit/Source/platform/weborigin/KURL.h"
#include "third_party/WebKit/Source/wtf/text/WTFString.h"
#include "third_party/WebKit/Source/wtf/text/WTFStringUtil.h"

#include <string>

#define kMimeTypeText "text/plain"
#define kMimeTypeURIList "text/uri-list"
#define kMimeTypeDownloadURL  "downloadurl"
#define kMimeTypeHTML "text/html"
#define kMimeTypeRTF "text/rtf"
#define kMimeTypePNG "image/png"

struct tagFORMATETC;
typedef tagFORMATETC FORMATETC;

namespace content {

class ClipboardUtil {
public:
    static UINT getHtmlFormatType();

    static UINT getWebKitSmartPasteFormatType();

    static UINT getUrlWFormatType();

    static UINT getRtfFormatType();

    static const UINT getWebCustomDataFormatType();

    static FORMATETC* getPlainTextWFormatType();

    static FORMATETC* getPlainTextFormatType();

    static FORMATETC* urlWFormat();

    static FORMATETC* urlFormat();

    static bool getWebLocData(IDataObject* dataObject, String& url, String* title);

    static String extractURL(const String &inURL, String* title);

    static String getURL(IDataObject* dataObject, String* title);

    static String getPlainText(IDataObject* dataObject);

    static HGLOBAL createGlobalData(const blink::KURL& url, const String& title);

    static HGLOBAL createGlobalData(const Vector<UChar>& str);

    static WTF::String ClipboardUtil::HtmlToCFHtml(const std::string& html, const std::string& base_url);

    static void CFHtmlExtractMetadata(const std::string& cf_html,
        std::string* base_url,
        size_t* html_start,
        size_t* fragment_start,
        size_t* fragment_end);
};

}

#endif // UI_BASE_CLIPBOARD_CLIPBOARD_UTIL_WIN_H_