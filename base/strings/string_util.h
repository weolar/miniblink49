// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// This file defines utility functions for working with strings.

#ifndef BASE_STRINGS_STRING_UTIL_H_
#define BASE_STRINGS_STRING_UTIL_H_

#include <string>
#include "cef/include/base/cef_string16.h"

namespace base {

enum TrimPositions {
    TRIM_NONE = 0,
    TRIM_LEADING = 1 << 0,
    TRIM_TRAILING = 1 << 1,
    TRIM_ALL = TRIM_LEADING | TRIM_TRAILING,
};

extern const wchar_t* kWhitespaceWide;
extern const char16* kWhitespaceUTF16;
extern const char* kWhitespaceASCII;

template <class Char> inline Char ToLowerASCII(Char c) {
    return (c >= 'A' && c <= 'Z') ? (c + ('a' - 'A')) : c;
}

template <class Char> inline Char ToUpperASCII(Char c) {
    return (c >= 'a' && c <= 'z') ? (c + ('A' - 'a')) : c;
}

template <class str> inline void StringToLowerASCII(str* s) {
    for (typename str::iterator i = s->begin(); i != s->end(); ++i)
        *i = ToLowerASCII(*i);
}

template <class str> inline str StringToLowerASCII(const str& s) {
    // for std::string and std::wstring  
    str output(s);
    StringToLowerASCII(&output);
    return output;
}

template<class STR>
inline bool DoIsStringASCII(const STR& str) {
    for (size_t i = 0; i < str.length(); i++) {
        unsigned char c = str[i];
        if (c > 0x7F)
            return false;
    }
    return true;
}

inline bool IsStringASCII(const std::string& str) {
    return DoIsStringASCII(str);
}

inline bool IsStringASCII(const std::wstring& str) {
    return DoIsStringASCII(str);
}

inline char ToUpperASCII(char c) {
    return (c >= 'a' && c <= 'z') ? (c + ('A' - 'a')) : c;
}

std::string ToLowerASCII(const std::string& str);

std::string ToUpperASCII(const std::string& str);

TrimPositions TrimWhitespace(const string16& input, TrimPositions positions, string16* output);

// Deprecated. This function is only for backward compatibility and calls
// TrimWhitespaceASCII().
TrimPositions TrimWhitespace(const std::string& input, TrimPositions positions, std::string* output);

std::wstring UTF8ToWide(const std::string& src);

std::wstring ASCIIToWide(const std::string& ascii);

std::string UTF16ToASCII(const string16& utf16);

}
#endif // BASE_STRINGS_STRING_UTIL_H_