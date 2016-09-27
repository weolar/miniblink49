/*
 * Copyright (C) 2008 Apple Inc. All rights reserved.
 * Copyright (C) 2010 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef ScriptDebugListener_h
#define ScriptDebugListener_h

#include "bindings/core/v8/ScriptState.h"
#include "core/CoreExport.h"
#include "wtf/Forward.h"
#include "wtf/Vector.h"
#include "wtf/text/WTFString.h"

namespace blink {

class ExecutionContext;
class ScriptValue;

enum CompileResult { CompileSuccess, CompileError };

class CORE_EXPORT ScriptDebugListener {
public:
    class Script {
    public:
        Script();

        String url() const { return m_url; }
        bool hasSourceURL() const { return !m_sourceURL.isEmpty(); }
        String sourceURL() const;
        String sourceMappingURL() const { return m_sourceMappingURL; }
        String source() const { return m_source; }
        int startLine() const { return m_startLine; }
        int startColumn() const { return m_startColumn; }
        int endLine() const { return m_endLine; }
        int endColumn() const { return m_endColumn; }
        bool isContentScript() const { return m_isContentScript; }
        bool isInternalScript() const { return m_isInternalScript; }

        bool getBlackboxedState(unsigned blackboxGeneration, bool* isBlackboxed) const;
        void setBlackboxedState(unsigned blackboxGeneration, bool isBlackboxed);

        Script& setURL(const String&);
        Script& setSourceURL(const String&);
        Script& setSourceMappingURL(const String&);
        Script& setSource(const String&);
        Script& setStartLine(int);
        Script& setStartColumn(int);
        Script& setEndLine(int);
        Script& setEndColumn(int);
        Script& setIsContentScript(bool);
        Script& setIsInternalScript(bool);

    private:
        String m_url;
        String m_sourceURL;
        String m_sourceMappingURL;
        String m_source;
        int m_startLine;
        int m_startColumn;
        int m_endLine;
        int m_endColumn;
        bool m_isContentScript;
        bool m_isInternalScript;
        // Used from outside for caching.
        bool m_isBlackboxedURL;
        unsigned m_blackboxGeneration;
    };

    enum SkipPauseRequest {
        NoSkip,
        Continue,
        StepInto,
        StepOut,
        StepFrame
    };

    struct ParsedScript {
        String scriptId;
        Script script;
        CompileResult compileResult;
    };

    virtual ~ScriptDebugListener() { }

    virtual void didParseSource(const ParsedScript&) = 0;
    virtual SkipPauseRequest didPause(ScriptState*, const ScriptValue& callFrames, const ScriptValue& exception, const Vector<String>& hitBreakpoints, bool isPromiseRejection) = 0;
    virtual void didContinue() = 0;
    virtual bool v8AsyncTaskEventsEnabled() const = 0;
    virtual void didReceiveV8AsyncTaskEvent(ScriptState*, const String& eventType, const String& eventName, int id) = 0;
    virtual bool v8PromiseEventsEnabled() const = 0;
    virtual void didReceiveV8PromiseEvent(ScriptState*, v8::Local<v8::Object> promise, v8::Local<v8::Value> parentPromise, int status) = 0;
};

} // namespace blink


#endif // ScriptDebugListener_h
