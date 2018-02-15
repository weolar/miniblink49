/*
 * Copyright (C) 2004, 2006, 2007, 2008, 2011 Apple Inc. All rights reserved.
 * Copyright (C) 2006 Alexey Proskuryakov <ap@nypop.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "wtf/text/TextCodecICU.h"

#include "wtf/Assertions.h"
#include "wtf/StringExtras.h"
#include "wtf/Threading.h"
#include "wtf/WTFThreadData.h"
#include "wtf/text/CString.h"
#include "wtf/text/CharacterNames.h"
#include "wtf/text/StringBuilder.h"
#include "wtf/text/StringBuffer.h"
#include "wtf/text/WTFStringUtil.h"

using std::min;

namespace WTF {

const size_t ConversionBufferSize = 16384;

ICUConverterWrapper::~ICUConverterWrapper()
{
#ifdef MINIBLINK_NOT_IMPLEMENTED
    if (converter)
        ucnv_close(converter);
#endif // MINIBLINK_NOT_IMPLEMENTED
    notImplemented();
}

static UConverter*& cachedConverterICU()
{
    return wtfThreadData().cachedConverterICU().converter;
}

PassOwnPtr<TextCodec> TextCodecICU::create(const TextEncoding& encoding, const void*)
{
    return adoptPtr(new TextCodecICU(encoding));
}

void TextCodecICU::registerEncodingNames(EncodingNameRegistrar registrar)
{
    // We register Hebrew with logical ordering using a separate name.
    // Otherwise, this would share the same canonical name as the
    // visual ordering case, and then TextEncoding could not tell them
    // apart; ICU treats these names as synonyms.
    registrar("ISO-8859-8-I", "ISO-8859-8-I");
#ifdef MINIBLINK_NOT_IMPLEMENTED
    int32_t numEncodings = ucnv_countAvailable();
    for (int32_t i = 0; i < numEncodings; ++i) {
        const char* name = ucnv_getAvailableName(i);
        UErrorCode error = U_ZERO_ERROR;
#if !defined(USING_SYSTEM_ICU)
        const char* primaryStandard = "HTML";
        const char* secondaryStandard = "MIME";
#else
        const char* primaryStandard = "MIME";
        const char* secondaryStandard = "IANA";
#endif
        const char* standardName = ucnv_getStandardName(name, primaryStandard, &error);
        if (U_FAILURE(error) || !standardName) {
            error = U_ZERO_ERROR;
            // Try IANA to pick up 'windows-12xx' and other names
            // which are not preferred MIME names but are widely used.
            standardName = ucnv_getStandardName(name, secondaryStandard, &error);
            if (U_FAILURE(error) || !standardName)
                continue;
        }

        // A number of these aliases are handled in Chrome's copy of ICU, but
        // Chromium can be compiled with the system ICU.

        // 1. Treat GB2312 encoding as GBK (its more modern superset), to match other browsers.
        // 2. On the Web, GB2312 is encoded as EUC-CN or HZ, while ICU provides a native encoding
        //    for encoding GB_2312-80 and several others. So, we need to override this behavior, too.
#if defined(USING_SYSTEM_ICU)
        if (!strcmp(standardName, "GB2312") || !strcmp(standardName, "GB_2312-80"))
            standardName = "GBK";
        // Similarly, EUC-KR encodings all map to an extended version, but
        // per HTML5, the canonical name still should be EUC-KR.
        else if (!strcmp(standardName, "EUC-KR") || !strcmp(standardName, "KSC_5601") || !strcmp(standardName, "cp1363"))
            standardName = "EUC-KR";
        // And so on.
        else if (!strcasecmp(standardName, "iso-8859-9")) // This name is returned in different case by ICU 3.2 and 3.6.
            standardName = "windows-1254";
        else if (!strcmp(standardName, "TIS-620"))
            standardName = "windows-874";
#endif

        registrar(standardName, standardName);

        uint16_t numAliases = ucnv_countAliases(name, &error);
        ASSERT(U_SUCCESS(error));
        if (U_SUCCESS(error))
            for (uint16_t j = 0; j < numAliases; ++j) {
                error = U_ZERO_ERROR;
                const char* alias = ucnv_getAlias(name, j, &error);
                ASSERT(U_SUCCESS(error));
                if (U_SUCCESS(error) && alias != standardName)
                    registrar(alias, standardName);
            }
    }

    // These two entries have to be added here because ICU's converter table
    // cannot have both ISO-8859-8-I and ISO-8859-8.
    registrar("csISO88598I", "ISO-8859-8-I");
    registrar("logical", "ISO-8859-8-I");

#if defined(USING_SYSTEM_ICU)
    // Additional alias for MacCyrillic not present in ICU.
    registrar("maccyrillic", "x-mac-cyrillic");

    // Additional aliases that historically were present in the encoding
    // table in WebKit on Macintosh that don't seem to be present in ICU.
    // Perhaps we can prove these are not used on the web and remove them.
    // Or perhaps we can get them added to ICU.
    registrar("x-mac-roman", "macintosh");
    registrar("x-mac-ukrainian", "x-mac-cyrillic");
    registrar("cn-big5", "Big5");
    registrar("x-x-big5", "Big5");
    registrar("cn-gb", "GBK");
    registrar("csgb231280", "GBK");
    registrar("x-euc-cn", "GBK");
    registrar("x-gbk", "GBK");
    registrar("koi", "KOI8-R");
    registrar("visual", "ISO-8859-8");
    registrar("winarabic", "windows-1256");
    registrar("winbaltic", "windows-1257");
    registrar("wincyrillic", "windows-1251");
    registrar("iso-8859-11", "windows-874");
    registrar("iso8859-11", "windows-874");
    registrar("dos-874", "windows-874");
    registrar("wingreek", "windows-1253");
    registrar("winhebrew", "windows-1255");
    registrar("winlatin2", "windows-1250");
    registrar("winturkish", "windows-1254");
    registrar("winvietnamese", "windows-1258");
    registrar("x-cp1250", "windows-1250");
    registrar("x-cp1251", "windows-1251");
    registrar("x-euc", "EUC-JP");
    registrar("x-windows-949", "EUC-KR");
    registrar("KSC5601", "EUC-KR");
    registrar("x-uhc", "EUC-KR");
    registrar("shift-jis", "Shift_JIS");

    // Alternative spelling of ISO encoding names.
    registrar("ISO8859-1", "ISO-8859-1");
    registrar("ISO8859-2", "ISO-8859-2");
    registrar("ISO8859-3", "ISO-8859-3");
    registrar("ISO8859-4", "ISO-8859-4");
    registrar("ISO8859-5", "ISO-8859-5");
    registrar("ISO8859-6", "ISO-8859-6");
    registrar("ISO8859-7", "ISO-8859-7");
    registrar("ISO8859-8", "ISO-8859-8");
    registrar("ISO8859-8-I", "ISO-8859-8-I");
    registrar("ISO8859-9", "ISO-8859-9");
    registrar("ISO8859-10", "ISO-8859-10");
    registrar("ISO8859-13", "ISO-8859-13");
    registrar("ISO8859-14", "ISO-8859-14");
    registrar("ISO8859-15", "ISO-8859-15");
    // No need to have an entry for ISO8859-16. ISO-8859-16 has just one label
    // listed in WHATWG Encoding Living Standard (http://encoding.spec.whatwg.org/ ).

    // Additional aliases present in the WHATWG Encoding Standard
    // and Firefox (as of Oct 2014), but not in the upstream ICU.
    // Three entries for windows-1252 need not be listed here because
    // TextCodecLatin1 registers them.
    registrar("csiso58gb231280", "GBK");
    registrar("csiso88596e", "ISO-8859-6");
    registrar("csiso88596i", "ISO-8859-6");
    registrar("csiso88598e", "ISO-8859-8");
    registrar("gb_2312", "GBK");
    registrar("iso88592", "ISO-8859-2");
    registrar("iso88593", "ISO-8859-3");
    registrar("iso88594", "ISO-8859-4");
    registrar("iso88595", "ISO-8859-5");
    registrar("iso88596", "ISO-8859-6");
    registrar("iso88597", "ISO-8859-7");
    registrar("iso88598", "ISO-8859-8");
    registrar("iso88599", "windows-1254");
    registrar("iso885910", "ISO-8859-10");
    registrar("iso885911", "windows-874");
    registrar("iso885913", "ISO-8859-13");
    registrar("iso885914", "ISO-8859-14");
    registrar("iso885915", "ISO-8859-15");
    registrar("iso_8859-2", "ISO-8859-2");
    registrar("iso_8859-3", "ISO-8859-3");
    registrar("iso_8859-4", "ISO-8859-4");
    registrar("iso_8859-5", "ISO-8859-5");
    registrar("iso_8859-6", "ISO-8859-6");
    registrar("iso_8859-7", "ISO-8859-7");
    registrar("iso_8859-8", "ISO-8859-8");
    registrar("iso_8859-9", "windows-1254");
    registrar("iso_8859-15", "ISO-8859-15");
    registrar("koi8_r", "KOI8-R");
    registrar("x-cp1253", "windows-1253");
    registrar("x-cp1254", "windows-1254");
    registrar("x-cp1255", "windows-1255");
    registrar("x-cp1256", "windows-1256");
    registrar("x-cp1257", "windows-1257");
    registrar("x-cp1258", "windows-1258");
#endif

#else
    registrar("gb2312", "gb2312");
    registrar("gb_2312", "gb_2312");
    registrar("GBK", "GBK");
#endif // MINIBLINK_NOT_IMPLEMENTED
}

void TextCodecICU::registerCodecs(TextCodecRegistrar registrar)
{
    // See comment above in registerEncodingNames.
    registrar("ISO-8859-8-I", create, 0);
#ifdef MINIBLINK_NOT_IMPLEMENTED
    int32_t numEncodings = ucnv_countAvailable();
    for (int32_t i = 0; i < numEncodings; ++i) {
        const char* name = ucnv_getAvailableName(i);
        UErrorCode error = U_ZERO_ERROR;
        const char* standardName = ucnv_getStandardName(name, "MIME", &error);
        if (!U_SUCCESS(error) || !standardName) {
            error = U_ZERO_ERROR;
            standardName = ucnv_getStandardName(name, "IANA", &error);
            if (!U_SUCCESS(error) || !standardName)
                continue;
        }
        registrar(standardName, create, 0);
    }
#else
    registrar("gb2312", create, 0);
    registrar("GBK", create, 0);
#endif // MINIBLINK_NOT_IMPLEMENTED
}

TextCodecICU::TextCodecICU(const TextEncoding& encoding)
    : m_encoding(encoding)
    , m_converterICU(0)
#if defined(USING_SYSTEM_ICU)
    , m_needsGBKFallbacks(false)
#endif
{
    memset(m_incrementalDataChunk, 0, kIncrementalDataChunkLength);
    m_incrementalDataChunkLength = 0;
}

TextCodecICU::~TextCodecICU()
{
    releaseICUConverter();
}

void TextCodecICU::releaseICUConverter() const
{
#ifdef MINIBLINK_NOT_IMPLEMENTED
    if (m_converterICU) {
        UConverter*& cachedConverter = cachedConverterICU();
        if (cachedConverter)
            ucnv_close(cachedConverter);
        cachedConverter = m_converterICU;
        m_converterICU = 0;
    }
#endif // MINIBLINK_NOT_IMPLEMENTED
}

void TextCodecICU::createICUConverter() const
{
#ifdef MINIBLINK_NOT_IMPLEMENTED
    ASSERT(!m_converterICU);

#if defined(USING_SYSTEM_ICU)
    const char* name = m_encoding.name();
    m_needsGBKFallbacks = name[0] == 'G' && name[1] == 'B' && name[2] == 'K' && !name[3];
#endif

    UErrorCode err;

    UConverter*& cachedConverter = cachedConverterICU();
    if (cachedConverter) {
        err = U_ZERO_ERROR;
        const char* cachedName = ucnv_getName(cachedConverter, &err);
        if (U_SUCCESS(err) && m_encoding == cachedName) {
            m_converterICU = cachedConverter;
            cachedConverter = 0;
            return;
        }
    }

    err = U_ZERO_ERROR;
    m_converterICU = ucnv_open(m_encoding.name(), &err);
#if !LOG_DISABLED
    if (err == U_AMBIGUOUS_ALIAS_WARNING)
        WTF_LOG_ERROR("ICU ambiguous alias warning for encoding: %s", m_encoding.name());
#endif
    if (m_converterICU)
        ucnv_setFallback(m_converterICU, TRUE);
#endif // MINIBLINK_NOT_IMPLEMENTED
    notImplemented();
}

int TextCodecICU::decodeToBuffer(UChar* target, UChar* targetLimit, const char*& source, const char* sourceLimit, int32_t* offsets, bool flush, UErrorCode& err)
{
#ifdef MINIBLINK_NOT_IMPLEMENTED
    UChar* targetStart = target;
    err = U_ZERO_ERROR;
    ucnv_toUnicode(m_converterICU, &target, targetLimit, &source, sourceLimit, offsets, flush, &err);
    return target - targetStart;
#endif // MINIBLINK_NOT_IMPLEMENTED
    notImplemented();
    return 0;
}

#ifdef MINIBLINK_NOT_IMPLEMENTED
class ErrorCallbackSetter {
public:
    ErrorCallbackSetter(UConverter* converter, bool stopOnError)
        :
        m_converter(converter),
        m_shouldStopOnEncodingErrors(stopOnError)
    {
        if (m_shouldStopOnEncodingErrors) {
            UErrorCode err = U_ZERO_ERROR;
            ucnv_setToUCallBack(m_converter, UCNV_TO_U_CALLBACK_SUBSTITUTE,
                           UCNV_SUB_STOP_ON_ILLEGAL, &m_savedAction,
                           &m_savedContext, &err);
            ASSERT(err == U_ZERO_ERROR);
        }
    }
    ~ErrorCallbackSetter()
    {
        if (m_shouldStopOnEncodingErrors) {
            UErrorCode err = U_ZERO_ERROR;
            const void* oldContext;
            UConverterToUCallback oldAction;
            ucnv_setToUCallBack(m_converter, m_savedAction,
                   m_savedContext, &oldAction,
                   &oldContext, &err);
            ASSERT(oldAction == UCNV_TO_U_CALLBACK_SUBSTITUTE);
            ASSERT(!strcmp(static_cast<const char*>(oldContext), UCNV_SUB_STOP_ON_ILLEGAL));
            ASSERT(err == U_ZERO_ERROR);
        }
    }

private:
    UConverter* m_converter;
    bool m_shouldStopOnEncodingErrors;
    const void* m_savedContext;
    UConverterToUCallback m_savedAction;
};
#endif // MINIBLINK_NOT_IMPLEMENTED


// 1）GB 18030 与 GB 2312 - 1980 和 GBK 兼容，共收录汉字70244个。
//   1，与 UTF - 8 相同，采用多字节编码，每个字可以由 1 个、2 个或 4 个字节组成。
//   2，编码空间庞大，最多可定义 161 万个字符。
//   3，支持中国国内少数民族的文字，不需要动用造字区。
//   4，汉字收录范围包含繁体汉字以及日韩汉字
// 2）GB 18030 编码是一二四字节变长编码。
//   1，单字节，其值从 0 到 0x7F，与 ASCII 编码兼容。
//   2，双字节，第一个字节的值从 0x81 到 0xFE，第二个字节的值从 0x40 到 0xFE（不包括0x7F），与 GBK 标准兼容。
//   四字节，第一个字节的值从 0x81 到 0xFE，第二个字节的值从 0x30 到 0x39，第三个字节从0x81 到 0xFE，第四个字节从 0x30 到 0x39。
bool isValideGB(const unsigned char* str, int length)
{
    if (1 == length) {
        unsigned char c = str[0];
        return c >= 0 && c <= 0x7f;
    }

    if (2 == length) {
        unsigned char c1 = str[0];
        unsigned char c2 = str[1];
        return (c1 >= 0x81 && c1 <= 0xFE) && (c2 >= 0x40 && c2 <= 0xFE);
    }

    if (4 == length) {
        unsigned char c1 = str[0];
        unsigned char c2 = str[1];
        unsigned char c3 = str[2];
        unsigned char c4 = str[3];
        return (c1 >= 0x81 && c1 <= 0xFE) && (c2 >= 0x30 && c2 <= 0x39) && (c3 >= 0x81 && c3 <= 0xFE) && (c4 >= 0x30 && c4 <= 0x39);
    }

    return false;
}

static Vector<UChar> decodeGbkWithLastData(char* data, int len, char* lastData, int* lastLength)
{
    std::string valideBytes;
    valideBytes.resize(len + *lastLength);

    char* buffer = &valideBytes[0];
    memcpy(buffer, lastData, *lastLength);
    memcpy(buffer + *lastLength, data, len);

    *lastLength = 0;
    int size = valideBytes.size();

    int i = 0;
    for (i = 0; i < size; i++) {
        unsigned char c = buffer[0];
        if (c < 0x80 || c == 0xA0) {
            continue;
        }
        if ((i + 1) < size) {
            i++;
        } else {
            break;
        }
    }

    // 考虑兼容4字节， 用 MB_ERR_INVALID_CHARS 标志是用来报告错误
    int size2 = MultiByteToWideChar(54936, MB_ERR_INVALID_CHARS, buffer, i, NULL, 0);
    if (size2 <= 0) {
        if (i < size) {
            // 失败， 末尾有可能是 4字节截断
            // 也有可能中间有乱码， 这样，我们再回退2字节不会更坏吧？
            if (i >= 2) {
                unsigned char c = buffer[i - 2];
                if (c >= 0x80 && c != 0xA0) {
                    i -= 2;
                }
            }
        }
        size2 = MultiByteToWideChar(54936, 0, buffer, i, NULL, 0);
    }
    *lastLength = size - i;
    memcpy(lastData, buffer + i, *lastLength);
    size2 = MultiByteToWideChar(54936, 0, buffer, i, NULL, 0);

    Vector<UChar> result;
    result.resize(size2);
    if (size2> 0)
        MultiByteToWideChar(54936, 0, buffer, i, &result[0], size2);
    return result;
}

String TextCodecICU::decode(const char* bytes, size_t length, FlushBehavior flush, bool stopOnError, bool& sawError)
{
#ifdef MINIBLINK_NOT_IMPLEMENTED
    // Get a converter for the passed-in encoding.
    if (!m_converterICU) {
        createICUConverter();
        ASSERT(m_converterICU);
        if (!m_converterICU) {
            WTF_LOG_ERROR("error creating ICU encoder even though encoding was in table");
            return String();
        }
    }

    ErrorCallbackSetter callbackSetter(m_converterICU, stopOnError);

    StringBuilder result;

    UChar buffer[ConversionBufferSize];
    UChar* bufferLimit = buffer + ConversionBufferSize;
    const char* source = reinterpret_cast<const char*>(bytes);
    const char* sourceLimit = source + length;
    int32_t* offsets = NULL;
    UErrorCode err = U_ZERO_ERROR;

    do {
        int ucharsDecoded = decodeToBuffer(buffer, bufferLimit, source, sourceLimit, offsets, flush != DoNotFlush, err);
        result.append(buffer, ucharsDecoded);
    } while (err == U_BUFFER_OVERFLOW_ERROR);

    if (U_FAILURE(err)) {
        // flush the converter so it can be reused, and not be bothered by this error.
        do {
            decodeToBuffer(buffer, bufferLimit, source, sourceLimit, offsets, true, err);
        } while (source < sourceLimit);
        sawError = true;
    }

#if !defined(USING_SYSTEM_ICU)
    // Chrome's copy of ICU does not have the issue described below.
    return result.toString();
#else
    String resultString = result.toString();

    // <http://bugs.webkit.org/show_bug.cgi?id=17014>
    // Simplified Chinese pages use the code A3A0 to mean "full-width space", but ICU decodes it as U+E5E5.
    if (!strcmp(m_encoding.name(), "GBK")) {
        if (!strcasecmp(m_encoding.name(), "gb18030"))
            resultString.replace(0xE5E5, ideographicSpaceCharacter);
        // Make GBK compliant to the encoding spec and align with GB18030
        resultString.replace(0x01F9, 0xE7C8);
        // FIXME: Once https://www.w3.org/Bugs/Public/show_bug.cgi?id=28740#c3
        // is resolved, add U+1E3F => 0xE7C7.
    }

    return resultString;
#endif

#else
    Vector<UChar> resultBuffer;
    if (strcasecmp(m_encoding.name(), "gb2312") && strcasecmp(m_encoding.name(), "GBK"))
        return String();

    if (0 == length)
        return String();

#if 0
    Vector<char> valideBytes;

    valideBytes.resize(length + m_incrementalDataChunkLength);
    memcpy(valideBytes.data(), m_incrementalDataChunk, m_incrementalDataChunkLength);
    memcpy(valideBytes.data() + m_incrementalDataChunkLength, bytes, length);

    m_incrementalDataChunkLength = 0;
    const unsigned char* lastInvalideChar = (const unsigned char*)bytes + length - 1;
#endif

#if 0
    if (length > 2 && !isValideGB(lastInvalideChar, 1) && !isValideGB(lastInvalideChar - 1, 2)) {
        m_incrementalDataChunkLength = 1; // 目前不支持GB-18030四字节编码
        m_incrementalDataChunk[0] = *lastInvalideChar;
    }

    WTF::MByteToWChar(valideBytes.data(), valideBytes.size() - m_incrementalDataChunkLength, &resultBuffer, CP_ACP);
    if (0 == resultBuffer.size())
        return String();
#endif

#if 0
    resultBuffer = decodeGbkWithLastData(valideBytes.data(), valideBytes.size(), m_incrementalDataChunk, &m_incrementalDataChunkLength);
    return String(&resultBuffer[0], resultBuffer.size());
#endif

    StringBuffer<UChar> buffer(m_incrementalDataChunkLength + length);
    const uint8_t* source = reinterpret_cast<const uint8_t*>(bytes);
    const uint8_t* end = source + length;
    UChar* destination = buffer.characters();

    UChar ch;
    while (source < end) {
        if (toUnicode(*source, ch)) {
            *destination = ch;
            destination++;
        }
        source++;
    }
    buffer.shrink(destination - buffer.characters());
    return String::adopt(buffer);

    
#endif // MINIBLINK_NOT_IMPLEMENTED
}

bool TextCodecICU::hasValidChar()
{
    if (m_incrementalDataChunk == 0)
        return false;

    m_incrementalDataChunk[m_incrementalDataChunkLength] = 'A';
    m_incrementalDataChunk[m_incrementalDataChunkLength + 1] = '\0';
    char* ptr = CharNextExA(CP_ACP, (LPCSTR)m_incrementalDataChunk, 0);
    if (ptr > ((char*)m_incrementalDataChunk + m_incrementalDataChunkLength))
        return false;

    return true;
}

bool TextCodecICU::toUnicode(unsigned char c, UChar& uc)
{
    m_incrementalDataChunk[m_incrementalDataChunkLength++] = c;
    if (m_incrementalDataChunkLength + 2 >= kIncrementalDataChunkLength || hasValidChar()) {
        int ret = MultiByteToWideChar(CP_ACP, 0, (LPSTR)m_incrementalDataChunk, m_incrementalDataChunkLength, &uc, 1);
        m_incrementalDataChunkLength = 0;

        return ret == 1 ? true : false;
    }

    return false;
}

#if defined(USING_SYSTEM_ICU)
// U+01F9 and U+1E3F have to be mapped to xA8xBF and xA8xBC per the encoding
// spec, but ICU converter does not have them.
static UChar fallbackForGBK(UChar32 character)
{
    switch (character) {
    case 0x01F9:
        return 0xE7C8; // mapped to xA8xBF by ICU.
    case 0x1E3F:
        return 0xE7C7; // mapped to xA8xBC by ICU.
    }
    return 0;
}
#endif

#ifdef MINIBLINK_NOT_IMPLEMENTED
// Invalid character handler when writing escaped entities for unrepresentable
// characters. See the declaration of TextCodec::encode for more.
static void urlEscapedEntityCallback(const void* context, UConverterFromUnicodeArgs* fromUArgs, const UChar* codeUnits, int32_t length,
    UChar32 codePoint, UConverterCallbackReason reason, UErrorCode* err)
{
    if (reason == UCNV_UNASSIGNED) {
        *err = U_ZERO_ERROR;

        UnencodableReplacementArray entity;
        int entityLen = TextCodec::getUnencodableReplacement(codePoint, URLEncodedEntitiesForUnencodables, entity);
        ucnv_cbFromUWriteBytes(fromUArgs, entity, entityLen, 0, err);
    } else
        UCNV_FROM_U_CALLBACK_ESCAPE(context, fromUArgs, codeUnits, length, codePoint, reason, err);
}

#if defined(USING_SYSTEM_ICU)
// Substitutes special GBK characters, escaping all other unassigned entities.
static void gbkCallbackEscape(const void* context, UConverterFromUnicodeArgs* fromUArgs, const UChar* codeUnits, int32_t length,
    UChar32 codePoint, UConverterCallbackReason reason, UErrorCode* err)
{
    UChar outChar;
    if (reason == UCNV_UNASSIGNED && (outChar = fallbackForGBK(codePoint))) {
        const UChar* source = &outChar;
        *err = U_ZERO_ERROR;
        ucnv_cbFromUWriteUChars(fromUArgs, &source, source + 1, 0, err);
        return;
    }
    UCNV_FROM_U_CALLBACK_ESCAPE(context, fromUArgs, codeUnits, length, codePoint, reason, err);
}

// Combines both gbkUrlEscapedEntityCallback and GBK character substitution.
static void gbkUrlEscapedEntityCallack(const void* context, UConverterFromUnicodeArgs* fromUArgs, const UChar* codeUnits, int32_t length,
    UChar32 codePoint, UConverterCallbackReason reason, UErrorCode* err)
{
    if (reason == UCNV_UNASSIGNED) {
        if (UChar outChar = fallbackForGBK(codePoint)) {
            const UChar* source = &outChar;
            *err = U_ZERO_ERROR;
            ucnv_cbFromUWriteUChars(fromUArgs, &source, source + 1, 0, err);
            return;
        }
        urlEscapedEntityCallback(context, fromUArgs, codeUnits, length, codePoint, reason, err);
        return;
    }
    UCNV_FROM_U_CALLBACK_ESCAPE(context, fromUArgs, codeUnits, length, codePoint, reason, err);
}

static void gbkCallbackSubstitute(const void* context, UConverterFromUnicodeArgs* fromUArgs, const UChar* codeUnits, int32_t length,
    UChar32 codePoint, UConverterCallbackReason reason, UErrorCode* err)
{
    UChar outChar;
    if (reason == UCNV_UNASSIGNED && (outChar = fallbackForGBK(codePoint))) {
        const UChar* source = &outChar;
        *err = U_ZERO_ERROR;
        ucnv_cbFromUWriteUChars(fromUArgs, &source, source + 1, 0, err);
        return;
    }
    UCNV_FROM_U_CALLBACK_SUBSTITUTE(context, fromUArgs, codeUnits, length, codePoint, reason, err);
}
#endif // USING_SYSTEM_ICU

#endif // MINIBLINK_NOT_IMPLEMENTED

class TextCodecInput {
public:
    TextCodecInput(const TextEncoding& encoding, const UChar* characters, size_t length)
        : m_begin(characters)
        , m_end(characters + length)
    { }

    TextCodecInput(const TextEncoding& encoding, const LChar* characters, size_t length)
    {
        m_buffer.reserveInitialCapacity(length);
        for (size_t i = 0; i < length; ++i)
            m_buffer.append(characters[i]);
        m_begin = m_buffer.data();
        m_end = m_begin + m_buffer.size();
    }

    const UChar* begin() const { return m_begin; }
    const UChar* end() const { return m_end; }

private:
    const UChar* m_begin;
    const UChar* m_end;
    Vector<UChar> m_buffer;
};

CString TextCodecICU::encodeInternal(const TextCodecInput& input, UnencodableHandling handling)
{
    const UChar* source = input.begin();
    const UChar* end = input.end();

    UErrorCode err = U_ZERO_ERROR;

#ifdef MINIBLINK_NOT_IMPLEMENTED
    switch (handling) {
        case QuestionMarksForUnencodables:
            ucnv_setSubstChars(m_converterICU, "?", 1, &err);
#if !defined(USING_SYSTEM_ICU)
            ucnv_setFromUCallBack(m_converterICU, UCNV_FROM_U_CALLBACK_SUBSTITUTE, 0, 0, 0, &err);
#else
            ucnv_setFromUCallBack(m_converterICU, m_needsGBKFallbacks ? gbkCallbackSubstitute : UCNV_FROM_U_CALLBACK_SUBSTITUTE, 0, 0, 0, &err);
#endif
            break;
        case EntitiesForUnencodables:
#if !defined(USING_SYSTEM_ICU)
            ucnv_setFromUCallBack(m_converterICU, UCNV_FROM_U_CALLBACK_ESCAPE, UCNV_ESCAPE_XML_DEC, 0, 0, &err);
#else
            ucnv_setFromUCallBack(m_converterICU, m_needsGBKFallbacks ? gbkCallbackEscape : UCNV_FROM_U_CALLBACK_ESCAPE, UCNV_ESCAPE_XML_DEC, 0, 0, &err);
#endif
            break;
        case URLEncodedEntitiesForUnencodables:
#if !defined(USING_SYSTEM_ICU)
            ucnv_setFromUCallBack(m_converterICU, urlEscapedEntityCallback, 0, 0, 0, &err);
#else
            ucnv_setFromUCallBack(m_converterICU, m_needsGBKFallbacks ? gbkUrlEscapedEntityCallack : urlEscapedEntityCallback, 0, 0, 0, &err);
#endif
            break;
    }

    ASSERT(U_SUCCESS(err));
    if (U_FAILURE(err))
        return CString();

    Vector<char> result;
    size_t size = 0;
    do {
        char buffer[ConversionBufferSize];
        char* target = buffer;
        char* targetLimit = target + ConversionBufferSize;
        err = U_ZERO_ERROR;
        ucnv_fromUnicode(m_converterICU, &target, targetLimit, &source, end, 0, true, &err);
        size_t count = target - buffer;
        result.grow(size + count);
        memcpy(result.data() + size, buffer, count);
        size += count;
    } while (err == U_BUFFER_OVERFLOW_ERROR);

    return CString(result.data(), size);
#endif // MINIBLINK_NOT_IMPLEMENTED
    notImplemented();
    return CString();
}

template<typename CharType>
CString TextCodecICU::encodeCommon(const CharType* characters, size_t length, UnencodableHandling handling)
{
    if (!length)
        return "";

    if (!m_converterICU)
        createICUConverter();
    if (!m_converterICU)
        return CString();

    TextCodecInput input(m_encoding, characters, length);
    return encodeInternal(input, handling);
}

CString TextCodecICU::encode(const UChar* characters, size_t length, UnencodableHandling handling)
{
#ifdef MINIBLINK_NOT_IMPLEMENTED
    return encodeCommon(characters, length, handling);
#else
    std::vector<char> resultBuffer;
    if (strcasecmp(m_encoding.name(), "gb2312") && strcasecmp(m_encoding.name(), "GBK"))
        return CString();

    WCharToMByte(characters, length, &resultBuffer, CP_ACP);
    if (0 == resultBuffer.size())
        return CString();
    return CString(&resultBuffer[0], resultBuffer.size());
#endif // MINIBLINK_NOT_IMPLEMENTED
}

CString TextCodecICU::encode(const LChar* characters, size_t length, UnencodableHandling handling)
{
#ifdef MINIBLINK_NOT_IMPLEMENTED
    return encodeCommon(characters, length, handling);
#else
    if (strcasecmp(m_encoding.name(), "gb2312") && strcasecmp(m_encoding.name(), "GBK"))
        return CString();

    bool sawError = false;
    String returnString = decode((const char*)characters, length, DoNotFlush, true, sawError);
    return encode(returnString.characters16(), returnString.length(), handling);
#endif // MINIBLINK_NOT_IMPLEMENTED
}

} // namespace WTF
