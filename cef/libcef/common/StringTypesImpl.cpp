// Copyright (c) 2010 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
#if (defined ENABLE_CEF) && (ENABLE_CEF == 1)
#include "include/internal/cef_string_types.h"
#include <algorithm>
#include "wtf/FastMalloc.h"
#include "wtf/text/UTF8.h"
#include "wtf/text/WTFString.h"

namespace {

void string_wide_dtor(wchar_t* str) {
    //WTF::fastFree(str);
    free(str);
}

void string_utf8_dtor(char* str) {
    //WTF::fastFree(str);
    free(str);
}

void string_utf16_dtor(char16* str) {
    //WTF::fastFree(str);
    free(str);
}

void char16Dtor(char16* str) {
    //WTF::fastFree(str);
    free(str);
}

void utf8Dtor(char* str) {
    //WTF::fastFree(str);
    free(str);
}

void wideCharDtor(wchar_t* str) {
    //WTF::fastFree(str);
    free(str);
}

void freeCefStringUserfreeWide(cef_string_userfree_wide_t str) {
    //WTF::fastFree(str);
    free(str);
}

void freeCefStringUserfreeUtf8(cef_string_userfree_utf8_t str) {
    //WTF::fastFree(str);
    free(str);
}

void freeCefStringUserfreeUtf16(cef_string_userfree_utf16_t str) {
    //WTF::fastFree(str);
    free(str);
}

void* zeroedMalloc(size_t size) {
    //return WTF::fastZeroedMalloc(size);
    if (0 == size)
        return nullptr;
    void* ptr = malloc(size);
    memset(ptr, 0, size);
    return ptr;
}

}  // namespace

CEF_EXPORT int cef_string_wide_set(const wchar_t* src, size_t src_len, cef_string_wide_t* output, int copy) {
    cef_string_wide_clear(output);

    if (copy) {
        if (src && src_len > 0) {
            output->str = (wchar_t*)zeroedMalloc(sizeof(wchar_t) * (src_len + 1));
            if (!output->str)
                return 0;

            memcpy(output->str, src, src_len * sizeof(wchar_t));
            output->str[src_len] = 0;
            output->length = src_len;
            output->dtor = string_wide_dtor;
        }
    } else {
        output->str = const_cast<wchar_t*>(src);
        output->length = src_len;
        output->dtor = NULL;
    }
    return 1;
}

CEF_EXPORT int cef_string_utf8_set(const char* src, size_t src_len, cef_string_utf8_t* output, int copy) {
    cef_string_utf8_clear(output);
    if (copy) {
        if (src && src_len > 0) {
            output->str = (char*)zeroedMalloc(sizeof(char) * (src_len + 1));
            if (!output->str)
                return 0;

            memcpy(output->str, src, src_len * sizeof(char));
            output->str[src_len] = 0;
            output->length = src_len;
            output->dtor = string_utf8_dtor;
        }
    } else {
        output->str = const_cast<char*>(src);
        output->length = src_len;
        output->dtor = NULL;
    }
    return 1;
}

CEF_EXPORT int cef_string_utf16_set(const char16* src, size_t src_len, cef_string_utf16_t* output, int copy) {
    cef_string_utf16_clear(output);

    if (copy) {
        if (src && src_len > 0) {
            output->str = (char16*)zeroedMalloc(sizeof(char16) * (src_len + 1));
            
            if (!output->str)
                return 0;

            memcpy(output->str, src, src_len * sizeof(char16));
            output->str[src_len] = 0;
            output->length = src_len;
            output->dtor = string_utf16_dtor;
        }
    } else {
        output->str = const_cast<char16*>(src);
        output->length = src_len;
        output->dtor = NULL;
    }
    return 1;
}

CEF_EXPORT void cef_string_wide_clear(cef_string_wide_t* str) {
    ASSERT(str != NULL);
    if (str->dtor && str->str)
        str->dtor(str->str);

    str->str = NULL;
    str->length = 0;
    str->dtor = NULL;
}

CEF_EXPORT void cef_string_utf8_clear(cef_string_utf8_t* str) {
    ASSERT(str != NULL);
    if (str->dtor && str->str)
        str->dtor(str->str);

    str->str = NULL;
    str->length = 0;
    str->dtor = NULL;
}

CEF_EXPORT void cef_string_utf16_clear(cef_string_utf16_t* str) {
    ASSERT(str != NULL);
    if (str->dtor && str->str)
        str->dtor(str->str);

    str->str = NULL;
    str->length = 0;
    str->dtor = NULL;
}

CEF_EXPORT int cef_string_wide_cmp(const cef_string_wide_t* str1, const cef_string_wide_t* str2) {
    if (str1->length == 0 && str2->length == 0)
        return 0;
    int r = wcsncmp(str1->str, str2->str, std::min(str1->length, str2->length));
    if (r == 0) {
        if (str1->length > str2->length)
            return 1;
        else if (str1->length < str2->length)
            return -1;
    }
    return r;
}

CEF_EXPORT int cef_string_utf8_cmp(const cef_string_utf8_t* str1, const cef_string_utf8_t* str2) {
    if (str1->length == 0 && str2->length == 0)
        return 0;
    int r = strncmp(str1->str, str2->str, std::min(str1->length, str2->length));
    if (r == 0) {
        if (str1->length > str2->length)
            return 1;
        else if (str1->length < str2->length)
            return -1;
    }
    return r;
}

CEF_EXPORT int cef_string_utf16_cmp(const cef_string_utf16_t* str1, const cef_string_utf16_t* str2) {
  if (str1->length == 0 && str2->length == 0)
    return 0;

  int r = wcsncmp(str1->str, str2->str, std::min(str1->length, str2->length));
  if (r == 0) {
    if (str1->length > str2->length)
      return 1;
    else if (str1->length < str2->length)
      return -1;
  }
  return r;
}

CEF_EXPORT int cef_string_wide_to_utf8(const wchar_t* src, size_t src_len, cef_string_utf8_t* output) {
    return cef_string_utf16_to_utf8((const char16*)src, src_len, output);
}

CEF_EXPORT int cef_string_utf8_to_wide(const char* src, size_t src_len, cef_string_wide_t* output) {
    return cef_string_utf8_to_utf16(src, src_len, (cef_string_utf16_t*)output);
}

CEF_EXPORT int cef_string_wide_to_utf16(const wchar_t* src, size_t src_len, cef_string_utf16_t* output) {
    return cef_string_utf16_set(src, src_len, output, true);
}

CEF_EXPORT int cef_string_utf16_to_wide(const char16* src, size_t src_len, cef_string_wide_t* output) {
    return cef_string_wide_set(src, src_len, output, true);
}

CEF_EXPORT int cef_string_utf8_to_utf16(const char* src, size_t length, cef_string_utf16_t* output) {
    int sizeInByte = (length + 1) * sizeof(UChar);
    UChar* buffer = (UChar*)zeroedMalloc(sizeInByte);
    UChar* bufferStart = buffer;

    UChar* bufferCurrent = bufferStart;
    const char* stringCurrent = reinterpret_cast<const char*>(src);
    if (WTF::Unicode::convertUTF8ToUTF16(
        &stringCurrent, reinterpret_cast<const char *>(src + length), &bufferCurrent, bufferCurrent + length) != WTF::Unicode::conversionOK)
        return 0;

    const unsigned utf16Length = bufferCurrent - bufferStart;
    ASSERT(utf16Length <= length);

    output->dtor = char16Dtor;
    output->str = (char16*)bufferStart;
    output->length = utf16Length;

    return 1;
}

CEF_EXPORT int cef_string_utf16_to_utf8(const char16* src, size_t length, cef_string_utf8_t* output) {
    WTF::String str((UChar*)src, length);
    WTF::CString utf8Str = str.utf8();
    char* buffer = (char*)zeroedMalloc((utf8Str.length() + 1) * sizeof(char));
    memcpy(buffer, utf8Str.data(), utf8Str.length());
    output->dtor = utf8Dtor;
    output->str = buffer;
    output->length = utf8Str.length();

    return 1;
}

CEF_EXPORT int cef_string_ascii_to_wide(const char* src, size_t src_len, cef_string_wide_t* output) {
    return cef_string_utf8_to_utf16(src, src_len, (cef_string_utf16_t *)output);
}

CEF_EXPORT int cef_string_ascii_to_utf16(const char* src, size_t src_len, cef_string_utf16_t* output) {
    return cef_string_utf8_to_utf16(src, src_len, output);
}

CEF_EXPORT cef_string_userfree_wide_t cef_string_userfree_wide_alloc() {
    cef_string_wide_t* s = (cef_string_wide_t*)zeroedMalloc(sizeof(cef_string_wide_t));
    return s;
}

CEF_EXPORT cef_string_userfree_utf8_t cef_string_userfree_utf8_alloc() {
    cef_string_utf8_t* s = (cef_string_utf8_t*)zeroedMalloc(sizeof(cef_string_utf8_t));
    return s;
}

CEF_EXPORT cef_string_userfree_utf16_t cef_string_userfree_utf16_alloc() {
    cef_string_utf16_t* s = (cef_string_utf16_t*)zeroedMalloc(sizeof(cef_string_utf16_t));
    return s;
}

CEF_EXPORT void cef_string_userfree_wide_free(cef_string_userfree_wide_t str) {
    cef_string_wide_clear(str);
    freeCefStringUserfreeWide(str);
}

CEF_EXPORT void cef_string_userfree_utf8_free(cef_string_userfree_utf8_t str) {
    cef_string_utf8_clear(str);
    freeCefStringUserfreeUtf8(str);
}

CEF_EXPORT void cef_string_userfree_utf16_free(cef_string_userfree_utf16_t str) {
    cef_string_utf16_clear(str);
    freeCefStringUserfreeUtf16(str);
}
#endif
