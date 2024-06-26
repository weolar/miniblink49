// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCRT_FX_CODEPAGE_H_
#define CORE_FXCRT_FX_CODEPAGE_H_

#include <stdint.h>

#define FX_CODEPAGE_DefANSI 0
#define FX_CODEPAGE_Symbol 42
#define FX_CODEPAGE_MSDOS_US 437
#define FX_CODEPAGE_Arabic_ASMO708 708
#define FX_CODEPAGE_MSDOS_Greek1 737
#define FX_CODEPAGE_MSDOS_Baltic 775
#define FX_CODEPAGE_MSDOS_WesternEuropean 850
#define FX_CODEPAGE_MSDOS_EasternEuropean 852
#define FX_CODEPAGE_MSDOS_Cyrillic 855
#define FX_CODEPAGE_MSDOS_Turkish 857
#define FX_CODEPAGE_MSDOS_Portuguese 860
#define FX_CODEPAGE_MSDOS_Icelandic 861
#define FX_CODEPAGE_MSDOS_Hebrew 862
#define FX_CODEPAGE_MSDOS_FrenchCanadian 863
#define FX_CODEPAGE_MSDOS_Arabic 864
#define FX_CODEPAGE_MSDOS_Norwegian 865
#define FX_CODEPAGE_MSDOS_Russian 866
#define FX_CODEPAGE_MSDOS_Greek2 869
#define FX_CODEPAGE_MSDOS_Thai 874
#define FX_CODEPAGE_ShiftJIS 932
#define FX_CODEPAGE_ChineseSimplified 936
#define FX_CODEPAGE_Hangul 949
#define FX_CODEPAGE_ChineseTraditional 950
#define FX_CODEPAGE_UTF16LE 1200
#define FX_CODEPAGE_UTF16BE 1201
#define FX_CODEPAGE_MSWin_EasternEuropean 1250
#define FX_CODEPAGE_MSWin_Cyrillic 1251
#define FX_CODEPAGE_MSWin_WesternEuropean 1252
#define FX_CODEPAGE_MSWin_Greek 1253
#define FX_CODEPAGE_MSWin_Turkish 1254
#define FX_CODEPAGE_MSWin_Hebrew 1255
#define FX_CODEPAGE_MSWin_Arabic 1256
#define FX_CODEPAGE_MSWin_Baltic 1257
#define FX_CODEPAGE_MSWin_Vietnamese 1258
#define FX_CODEPAGE_Johab 1361
#define FX_CODEPAGE_MAC_Roman 10000
#define FX_CODEPAGE_MAC_ShiftJIS 10001
#define FX_CODEPAGE_MAC_ChineseTraditional 10002
#define FX_CODEPAGE_MAC_Korean 10003
#define FX_CODEPAGE_MAC_Arabic 10004
#define FX_CODEPAGE_MAC_Hebrew 10005
#define FX_CODEPAGE_MAC_Greek 10006
#define FX_CODEPAGE_MAC_Cyrillic 10007
#define FX_CODEPAGE_MAC_ChineseSimplified 10008
#define FX_CODEPAGE_MAC_Thai 10021
#define FX_CODEPAGE_MAC_EasternEuropean 10029
#define FX_CODEPAGE_MAC_Turkish 10081
#define FX_CODEPAGE_UTF8 65001

#define FX_CHARSET_ANSI 0
#define FX_CHARSET_Default 1
#define FX_CHARSET_Symbol 2
#define FX_CHARSET_MAC_Roman 77
#define FX_CHARSET_MAC_ShiftJIS 78
#define FX_CHARSET_MAC_Korean 79
#define FX_CHARSET_MAC_ChineseSimplified 80
#define FX_CHARSET_MAC_ChineseTraditional 81
#define FX_CHARSET_MAC_Hebrew 83
#define FX_CHARSET_MAC_Arabic 84
#define FX_CHARSET_MAC_Greek 85
#define FX_CHARSET_MAC_Turkish 86
#define FX_CHARSET_MAC_Thai 87
#define FX_CHARSET_MAC_EasternEuropean 88
#define FX_CHARSET_MAC_Cyrillic 89
#define FX_CHARSET_ShiftJIS 128
#define FX_CHARSET_Hangul 129
#define FX_CHARSET_Johab 130
#define FX_CHARSET_ChineseSimplified 134
#define FX_CHARSET_ChineseTraditional 136
#define FX_CHARSET_MSWin_Greek 161
#define FX_CHARSET_MSWin_Turkish 162
#define FX_CHARSET_MSWin_Vietnamese 163
#define FX_CHARSET_MSWin_Hebrew 177
#define FX_CHARSET_MSWin_Arabic 178
#define FX_CHARSET_MSWin_Baltic 186
#define FX_CHARSET_MSWin_Cyrillic 204
#define FX_CHARSET_Thai 222
#define FX_CHARSET_MSWin_EasternEuropean 238
#define FX_CHARSET_US 254
#define FX_CHARSET_OEM 255

// Hi-bytes to unicode codepoint mapping for various code pages.
struct FX_CharsetUnicodes {
  uint8_t m_Charset;
  const uint16_t* m_pUnicodes;
};

extern const FX_CharsetUnicodes g_FX_CharsetUnicodes[8];

uint16_t FX_GetCodePageFromCharset(uint8_t charset);
uint8_t FX_GetCharsetFromCodePage(uint16_t codepage);
bool FX_CharSetIsCJK(uint8_t uCharset);

#endif  // CORE_FXCRT_FX_CODEPAGE_H_
