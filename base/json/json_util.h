#ifndef BASE_JSON_JSON_UTIL_H_
#define BASE_JSON_JSON_UTIL_H_

#include <string>
#include "base/strings/string_piece.h"

namespace base {

std::string IntToString(int value);
std::string UintToString(unsigned int value);
std::string Int64ToString(int64 value);
std::string DoubleToString(double value);

bool EscapeSpecialCodePoint(uint32 code_point, std::string* dest);
bool EscapeJSONString(const std::string& str, bool put_in_quotes, std::string* dest);

#define CBU8_IS_LEAD(c) ((uint8)((c)-0xc0)<0x3e)
#define CBU_SENTINEL (-1)
#define CBU8_COUNT_TRAIL_BYTES(leadByte) (utf8_countTrailBytes[(uint8)leadByte])
#define CBUTF8_ERROR_VALUE_2 0x9f
#define CBUTF_ERROR_VALUE 0xffff
#define CBU8_IS_TRAIL(c) (((c)&0xc0)==0x80)

#define CBU8_MASK_LEAD_BYTE(leadByte, countTrailBytes) ((leadByte)&=(1<<(6-(countTrailBytes)))-1)
#define CBU_IS_SURROGATE(c) (((c)&0xfffff800)==0xd800)
#define CBU16_IS_SURROGATE_LEAD(c) (((c)&0x400)==0)
#define CBU16_IS_TRAIL(c) (((c)&0xfffffc00)==0xdc00)

#define CBU_IS_UNICODE_NONCHAR(c) \
    ((c)>=0xfdd0 && \
     ((uint32)(c)<=0xfdef || ((c)&0xfffe)==0xfffe) && \
     (uint32)(c)<=0x10ffff)

#define CBUTF8_ERROR_VALUE_1 0x15

#define CBU8_NEXT(s, i, length, c) { \
    (c)=(s)[(i)++]; \
    if(((uint8)(c))>=0x80) { \
        if(CBU8_IS_LEAD(c)) { \
            (c)=utf8_nextCharSafeBody((const uint8 *)s, &(i), (int32)(length), c, -1); \
        } else { \
            (c)=CBU_SENTINEL; \
        } \
    } \
}

#define CBU8_MAX_LENGTH 4

#define CBU8_APPEND_UNSAFE(s, i, c) { \
    if((uint32)(c)<=0x7f) { \
        (s)[(i)++]=(uint8)(c); \
    } else { \
        if((uint32)(c)<=0x7ff) { \
            (s)[(i)++]=(uint8)(((c)>>6)|0xc0); \
        } else { \
            if((uint32)(c)<=0xffff) { \
                (s)[(i)++]=(uint8)(((c)>>12)|0xe0); \
            } else { \
                (s)[(i)++]=(uint8)(((c)>>18)|0xf0); \
                (s)[(i)++]=(uint8)((((c)>>12)&0x3f)|0x80); \
            } \
            (s)[(i)++]=(uint8)((((c)>>6)&0x3f)|0x80); \
        } \
        (s)[(i)++]=(uint8)(((c)&0x3f)|0x80); \
    } \
}

#define CBU8_NEXT(s, i, length, c) { \
    (c)=(s)[(i)++]; \
    if(((uint8)(c))>=0x80) { \
        if(CBU8_IS_LEAD(c)) { \
            (c)=utf8_nextCharSafeBody((const uint8 *)s, &(i), (int32)(length), c, -1); \
        } else { \
            (c)=CBU_SENTINEL; \
        } \
    } \
}

#define CBU16_SURROGATE_OFFSET ((0xd800<<10UL)+0xdc00-0x10000)
#define CBU16_GET_SUPPLEMENTARY(lead, trail) \
    (((int32_t)(lead)<<10UL)+(int32_t)(trail)-CBU16_SURROGATE_OFFSET)


#define CBU16_IS_SURROGATE(c) CBU_IS_SURROGATE(c)

inline bool IsValidCharacter(uint32 code_point)
{
    // Excludes non-characters (U+FDD0..U+FDEF, and all codepoints ending in
    // 0xFFFE or 0xFFFF) from the set of valid code points.
    return code_point < 0xD800u || (code_point >= 0xE000u &&
        code_point < 0xFDD0u) || (code_point > 0xFDEFu && code_point <= 0x10FFFFu && (code_point & 0xFFFEu) != 0xFFFEu);
}

inline bool IsValidCodepoint(uint32 code_point)
{
    // Excludes the surrogate code points ([0xD800, 0xDFFF]) and
    // codepoints larger than 0x10FFFF (the highest codepoint allowed).
    // Non-characters and unassigned codepoints are allowed.
    return code_point < 0xD800u || (code_point >= 0xE000u && code_point <= 0x10FFFFu);
}

int32_t utf8_nextCharSafeBody(const uint8 *s, int32 *pi, int32 length, int32_t c, int strict);

size_t WriteUnicodeCharacter(uint32 code_point, std::string* output);
bool EscapeJSONString(const std::string& str, bool put_in_quotes, std::string* dest);

bool HexStringToInt(const StringPiece& input, int* output);
bool StringToInt(const StringPiece& input, int* output);
bool StringToDouble(const std::string& input, double* output);
template <typename Char> inline bool IsAsciiDigit(Char c)
{
    return c >= '0' && c <= '9';
}

}

#endif // BASE_JSON_JSON_UTIL_H_