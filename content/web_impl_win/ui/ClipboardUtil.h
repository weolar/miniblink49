// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Some helper functions for working with the clipboard and IDataObjects.

#ifndef UI_BASE_CLIPBOARD_CLIPBOARD_UTIL_WIN_H_
#define UI_BASE_CLIPBOARD_CLIPBOARD_UTIL_WIN_H_

#include "base/strings/string_util.h"

#include <string>

namespace content {

class ClipboardUtil {

public:
    static WTF::String ClipboardUtil::HtmlToCFHtml(const std::string& html, const std::string& base_url) {
        if (html.empty())
            return WTF::String();

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

        WTF::String result = WTF::String::format(header, start_html_offset,
            end_html_offset, start_fragment_offset, end_fragment_offset);
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

    static void CFHtmlExtractMetadata(const std::string& cf_html,
        std::string* base_url,
        size_t* html_start,
        size_t* fragment_start,
        size_t* fragment_end) {
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
        }
        else {
            *fragment_start = cf_html.find('>', tag_start) + 1;
            size_t tag_end = cf_html.rfind("<!--EndFragment", std::string::npos);
            *fragment_end = cf_html.rfind('<', tag_end);
        }
    }

};

}

#endif // UI_BASE_CLIPBOARD_CLIPBOARD_UTIL_WIN_H_