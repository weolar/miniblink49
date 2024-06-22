/*
 * Copyright (C) 2008, 2009 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "platform/text/TextEncodingDetector.h"
#include "wtf/text/TextEncoding.h"

#include <unicode/ucnv.h>
#include <unicode/utypes.h>
//#include <unicode/ucsdet.h>
#include "platform/text/IcuCharsetDetector.h"
//#include <mlang.h>

// #define UNICODE 1
// 
// #include <objbase.h>
// #include <comip.h>
// #pragma comment(lib, "comsupp.lib")

namespace blink {

#ifdef MINIBLINK_NOT_IMPLEMENTED

// bool detectTextEncoding(const char* data, size_t length,
//     const char* hintEncodingName, WTF::TextEncoding* detectedEncoding)
// {
//     *detectedEncoding = WTF::TextEncoding();
//     int matchesCount = 0;
//     UErrorCode status = U_ZERO_ERROR;
//     UCharsetDetector* detector = ucsdet_open(&status);
//     if (U_FAILURE(status))
//         return false;
//     ucsdet_enableInputFilter(detector, true);
//     ucsdet_setText(detector, data, static_cast<int32_t>(length), &status);
//     if (U_FAILURE(status))
//         return false;
// 
//     // FIXME: A few things we can do other than improving
//     // the ICU detector itself.
//     // 1. Use ucsdet_detectAll and pick the most likely one given
//     // "the context" (parent-encoding, referrer encoding, etc).
//     // 2. 'Emulate' Firefox/IE's non-Universal detectors (e.g.
//     // Chinese, Japanese, Russian, Korean and Hebrew) by picking the
//     // encoding with a highest confidence among the detector-specific
//     // limited set of candidate encodings.
//     // Below is a partial implementation of the first part of what's outlined
//     // above.
//     const UCharsetMatch** matches = ucsdet_detectAll(detector, &matchesCount, &status);
//     if (U_FAILURE(status)) {
//         ucsdet_close(detector);
//         return false;
//     }
// 
//     const char* encoding = 0;
//     if (hintEncodingName) {
//         WTF::TextEncoding hintEncoding(hintEncodingName);
//         // 10 is the minimum confidence value consistent with the codepoint
//         // allocation in a given encoding. The size of a chunk passed to
//         // us varies even for the same html file (apparently depending on
//         // the network load). When we're given a rather short chunk, we
//         // don't have a sufficiently reliable signal other than the fact that
//         // the chunk is consistent with a set of encodings. So, instead of
//         // setting an arbitrary threshold, we have to scan all the encodings
//         // consistent with the data.
//         const int32_t kThresold = 10;
//         for (int i = 0; i < matchesCount; ++i) {
//             int32_t confidence = ucsdet_getConfidence(matches[i], &status);
//             if (U_FAILURE(status)) {
//                 status = U_ZERO_ERROR;
//                 continue;
//             }
//             if (confidence < kThresold)
//                 break;
//             const char* matchEncoding = ucsdet_getName(matches[i], &status);
//             if (U_FAILURE(status)) {
//                 status = U_ZERO_ERROR;
//                 continue;
//             }
//             if (WTF::TextEncoding(matchEncoding) == hintEncoding) {
//                 encoding = hintEncodingName;
//                 break;
//             }
//         }
//     }
//     // If no match is found so far, just pick the top match.
//     // This can happen, say, when a parent frame in EUC-JP refers to
//     // a child frame in Shift_JIS and both frames do NOT specify the encoding
//     // making us resort to auto-detection (when it IS turned on).
//     if (!encoding && matchesCount > 0)
//         encoding = ucsdet_getName(matches[0], &status);
//     if (U_SUCCESS(status)) {
//         *detectedEncoding = WTF::TextEncoding(encoding);
//         ucsdet_close(detector);
//         return true;
//     }
//     ucsdet_close(detector);
// 
// 	notImplemented();
//     return false;
// }

#else

// bool detectTextEncodingByMLang(const char* data, size_t length)
// {
// //     const size_t length = 20;
// //     unsigned char data[20] = {
// //         0x61, 0x61, 0x73, 0x64, 0x61, 0x73, 0x64, 0x61, 0x73, 0x64, 0x61, 0x73, 0x64, 0xB9, 0xFE, 0xB9,
// //         0xFE, 0, 0, 0
// //     };
// 
//     char * pBuf = new char[length];
//     memcpy(pBuf, data, length);
// 
//     IMultiLanguage2* pIMultiLanguage2 = NULL;
// 
//     HRESULT hr = S_OK;
//     hr = ::CoCreateInstance(CLSID_CMultiLanguage, NULL, CLSCTX_INPROC_SERVER, IID_IMultiLanguage2, (void**)&pIMultiLanguage2);
// 
//     DetectEncodingInfo encoding[10];
//     
//     int nScores = 10;
//     int nBufLen = length;
//     memset(encoding, 0, sizeof(DetectEncodingInfo) * 10);
//     DebugBreak();
//     hr = pIMultiLanguage2->DetectInputCodepage(MLDETECTCP_NONE, 0, pBuf, &nBufLen, encoding, &nScores);
//     if (0 > hr)
//         return false;
// 
//     // 20127 9 "US-ASCII"
//     // 936 4294967295 "gb2312"
// 
//     MIMECPINFO codePageInfo = { 0 };
//     hr = pIMultiLanguage2->GetCodePageInfo(encoding[0].nCodePage, encoding[0].nLangID, &codePageInfo);
// 
//     pIMultiLanguage2->Release();
// 
//     return true;
// }

bool detectTextEncoding(const char* data, size_t length, const char* hintEncodingName, WTF::TextEncoding* detectedEncoding)
{
    *detectedEncoding = WTF::TextEncoding();
    int matchesCount = 0;
    UErrorCode status = U_ZERO_ERROR;
    CharsetDetector* detector = new CharsetDetector(status);
    if (U_ZERO_ERROR != (status))
        return false;
    //ucsdet_enableInputFilter(detector, true);
    detector->setText(data, static_cast<int32_t>(length));

    const CharsetMatch*const* matches = detector->detectAll(matchesCount, status);
    if (U_ZERO_ERROR != (status)) {
        delete (detector);
        return false;
    }

    const char* encoding = 0;
    if (hintEncodingName) {
        WTF::TextEncoding hintEncoding(hintEncodingName);
        const int32_t kThresold = 10;
        for (int i = 0; i < matchesCount; ++i) {
            int32_t confidence = matches[i]->getConfidence();
            if (confidence < kThresold)
                break;
            const char* matchEncoding = matches[i]->getName();
            if (WTF::TextEncoding(matchEncoding) == hintEncoding) {
                encoding = hintEncodingName;
                break;
            }
        }
    }

    // If no match is found so far, just pick the top match.
    // This can happen, say, when a parent frame in EUC-JP refers to
    // a child frame in Shift_JIS and both frames do NOT specify the encoding
    // making us resort to auto-detection (when it IS turned on).
    if (!encoding && matchesCount > 0)
        encoding = matches[0]->getName();

    //detectTextEncodingByMLang(data, length);

    if (!encoding)
        encoding = "GBK";

    *detectedEncoding = WTF::TextEncoding(encoding);

    delete (detector);
    return true;
}

#endif // MINIBLINK_NOT_IMPLEMENTED

}
