// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Some helper functions for working with the clipboard and IDataObjects.

#ifndef UI_BASE_CLIPBOARD_CLIPBOARD_UTIL_WIN_H_
#define UI_BASE_CLIPBOARD_CLIPBOARD_UTIL_WIN_H_

#include <string>
#include <vector>

#include <ShlObj.h>

#define kMimeTypeText "text/plain"
#define kMimeTypeURIList "text/uri-list"
#define kMimeTypeDownloadURL  "downloadurl"
#define kMimeTypeHTML "text/html"
#define kMimeTypeRTF "text/rtf"
#define kMimeTypePNG "image/png"
#define kMimeTypeBMP "image/bmp"

struct tagFORMATETC;
typedef tagFORMATETC FORMATETC;

namespace content {

class ClipboardUtil {
public:
    static unsigned int getHtmlFormatType();

    static unsigned int getWebKitSmartPasteFormatType();

    static unsigned int getUrlWFormatType();

    static unsigned int getRtfFormatType();

    static const unsigned int getWebCustomDataFormatType();

    static FORMATETC* getPlainTextWFormatType();

    static FORMATETC* getPlainTextFormatType();

    static FORMATETC* urlWFormat();

    static FORMATETC* urlFormat();

    static bool getWebLocData(IDataObject* dataObject, std::string& url, std::string* title);

    static std::string extractURL(const std::string& inURL, std::string* title);

    static std::string getURL(IDataObject* dataObject, std::string* title);

    static std::string getPlainText(IDataObject* dataObject);

    static HGLOBAL createGlobalData(const std::string& url, const std::string& title);
    static HGLOBAL createGlobalData(const std::string& str);

    static std::string ClipboardUtil::HtmlToCFHtml(const std::string& html, const std::string& base_url);

    static void CFHtmlExtractMetadata(const std::string& cf_html,
        std::string* base_url,
        size_t* html_start,
        size_t* fragment_start,
        size_t* fragment_end);
};

}

#endif // UI_BASE_CLIPBOARD_CLIPBOARD_UTIL_WIN_H_