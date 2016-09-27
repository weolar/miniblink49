/*
 * Copyright (C) 2011 Google Inc.  All rights reserved.
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

#ifndef VTTParser_h
#define VTTParser_h

#include "core/HTMLNames.h"
#include "core/dom/DocumentFragment.h"
#include "core/html/parser/TextResourceDecoder.h"
#include "core/html/track/vtt/BufferedLineReader.h"
#include "core/html/track/vtt/VTTCue.h"
#include "core/html/track/vtt/VTTRegion.h"
#include "core/html/track/vtt/VTTTokenizer.h"
#include "platform/heap/Handle.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/text/StringBuilder.h"

namespace blink {

class Document;
class VTTScanner;

class VTTParserClient {
public:
    virtual ~VTTParserClient() { }

    virtual void newCuesParsed() = 0;
    virtual void newRegionsParsed() = 0;
    virtual void fileFailedToParse() = 0;
};

class VTTParser final : public NoBaseWillBeGarbageCollectedFinalized<VTTParser> {
public:
    enum ParseState {
        Initial,
        Header,
        Id,
        TimingsAndSettings,
        CueText,
        BadCue
    };

    static PassOwnPtrWillBeRawPtr<VTTParser> create(VTTParserClient* client, Document& document)
    {
        return adoptPtrWillBeNoop(new VTTParser(client, document));
    }

    static inline bool isRecognizedTag(const AtomicString& tagName)
    {
        return tagName == HTMLNames::iTag
            || tagName == HTMLNames::bTag
            || tagName == HTMLNames::uTag
            || tagName == HTMLNames::rubyTag
            || tagName == HTMLNames::rtTag;
    }
    static inline bool isASpace(UChar c)
    {
        // WebVTT space characters are U+0020 SPACE, U+0009 CHARACTER TABULATION (tab), U+000A LINE FEED (LF), U+000C FORM FEED (FF), and U+000D CARRIAGE RETURN    (CR).
        return c == ' ' || c == '\t' || c == '\n' || c == '\f' || c == '\r';
    }
    static inline bool isValidSettingDelimiter(UChar c)
    {
        // ... a WebVTT cue consists of zero or more of the following components, in any order, separated from each other by one or more
        // U+0020 SPACE characters or U+0009 CHARACTER TABULATION (tab) characters.
        return c == ' ' || c == '\t';
    }
    static bool collectTimeStamp(const String&, double& timeStamp);

    // Useful functions for parsing percentage settings.
    static bool parseFloatPercentageValue(VTTScanner& valueScanner, float& percentage);
    static bool parseFloatPercentageValuePair(VTTScanner&, char, FloatPoint&);

    // Create the DocumentFragment representation of the WebVTT cue text.
    static PassRefPtrWillBeRawPtr<DocumentFragment> createDocumentFragmentFromCueText(Document&, const String&);

    // Input data to the parser to parse.
    void parseBytes(const char* data, unsigned length);
    void flush();

    // Transfers ownership of last parsed cues to caller.
    void getNewCues(WillBeHeapVector<RefPtrWillBeMember<TextTrackCue>>&);
    void getNewRegions(WillBeHeapVector<RefPtrWillBeMember<VTTRegion>>&);

    DECLARE_TRACE();

private:
    VTTParser(VTTParserClient*, Document&);

    RawPtrWillBeMember<Document> m_document;
    ParseState m_state;

    void parse();
    void flushPendingCue();
    bool hasRequiredFileIdentifier(const String& line);
    ParseState collectCueId(const String&);
    ParseState collectTimingsAndSettings(const String&);
    ParseState collectCueText(const String&);
    ParseState recoverCue(const String&);
    ParseState ignoreBadCue(const String&);

    void createNewCue();
    void resetCueValues();

    void collectMetadataHeader(const String&);
    void createNewRegion(const String& headerValue);

    static bool collectTimeStamp(VTTScanner& input, double& timeStamp);

    BufferedLineReader m_lineReader;
    OwnPtr<TextResourceDecoder> m_decoder;
    AtomicString m_currentId;
    double m_currentStartTime;
    double m_currentEndTime;
    StringBuilder m_currentContent;
    String m_currentSettings;

    VTTParserClient* m_client;

    WillBeHeapVector<RefPtrWillBeMember<TextTrackCue>> m_cueList;

    WillBeHeapVector<RefPtrWillBeMember<VTTRegion>> m_regionList;
};

} // namespace blink

#endif
