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

namespace base {
class DictionaryValue;
}

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
    static FORMATETC* getCustomTextsType();

    static bool getWebLocData(IDataObject* dataObject, std::string& url, std::string* title);

    static std::string extractURL(const std::string& inURL, std::string* title);

    static std::string getURL(IDataObject* dataObject, std::string* title);

    static std::string getPlainText(IDataObject* dataObject);
    static base::DictionaryValue* getCustomPlainTexts(IDataObject* dataObject);

    static HGLOBAL createGlobalData(const std::string& url, const std::string& title);
    template <typename charT> static HGLOBAL createGlobalData(const std::basic_string<charT>& str)
    {
        if (str.size() == 0)
            return nullptr;

        HGLOBAL data = ::GlobalAlloc(GMEM_MOVEABLE, ((str.size() + 1) * sizeof(charT)));
        if (data) {
            charT* rawData = static_cast<charT*>(::GlobalLock(data));
            memcpy(rawData, &str[0], str.size() * sizeof(charT));
            rawData[str.size()] = '\0';
            ::GlobalUnlock(data);
        }
        return data;
    }

    static std::string htmlToCFHtml(const std::string& html, const std::string& base_url);

    static void cfHtmlExtractMetadata(const std::string& cfHtml, std::string* baseUrl, size_t* htmlStart, size_t* fragmentStart, size_t* fragmentEnd);
};

}

#endif // UI_BASE_CLIPBOARD_CLIPBOARD_UTIL_WIN_H_