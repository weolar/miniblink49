// This file is generated

// Copyright (c) 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef protocol_Debugger_h
#define protocol_Debugger_h

#include "platform/inspector_protocol/Platform.h"
#include "platform/inspector_protocol/Array.h"
#include "platform/inspector_protocol/BackendCallback.h"
#include "platform/inspector_protocol/DispatcherBase.h"
#include "platform/inspector_protocol/ErrorSupport.h"
#include "platform/inspector_protocol/FrontendChannel.h"
#include "platform/inspector_protocol/Maybe.h"
#include "platform/inspector_protocol/Object.h"
#include "platform/inspector_protocol/Platform.h"
#include "platform/inspector_protocol/String16.h"
#include "platform/inspector_protocol/Values.h"
#include "platform/inspector_protocol/ValueConversions.h"
// For each imported domain we generate a ValueConversions struct instead of a full domain definition
// and include Domain::API version from there.
#include "platform\v8_inspector\protocol/Runtime.h"
#include "platform\v8_inspector\public\protocol/Debugger.h"

namespace blink {
namespace protocol {
namespace Debugger {

// ------------- Forward and enum declarations.
// Breakpoint identifier.
using BreakpointId = String16;
// Call frame identifier.
using CallFrameId = String16;
// Location in the source code.
class Location;
// Location in the source code.
class ScriptPosition;
// JavaScript call frame. Array of call frames form the call stack.
class CallFrame;
// Scope description.
class Scope;
// Search match for resource.
class SearchMatch;

namespace SetPauseOnExceptions {
namespace StateEnum {
PLATFORM_EXPORT extern const char* None;
PLATFORM_EXPORT extern const char* Uncaught;
PLATFORM_EXPORT extern const char* All;
} // StateEnum
} // SetPauseOnExceptions

namespace Paused {
namespace ReasonEnum {
PLATFORM_EXPORT extern const char* XHR;
PLATFORM_EXPORT extern const char* DOM;
PLATFORM_EXPORT extern const char* EventListener;
PLATFORM_EXPORT extern const char* Exception;
PLATFORM_EXPORT extern const char* Assert;
PLATFORM_EXPORT extern const char* DebugCommand;
PLATFORM_EXPORT extern const char* PromiseRejection;
PLATFORM_EXPORT extern const char* Other;
} // ReasonEnum
} // Paused

// ------------- Type and builder declarations.

// Location in the source code.
class PLATFORM_EXPORT Location {
    PROTOCOL_DISALLOW_COPY(Location);
public:
    static std::unique_ptr<Location> parse(protocol::Value* value, ErrorSupport* errors);

    ~Location() { }

    String16 getScriptId() { return m_scriptId; }
    void setScriptId(const String16& value) { m_scriptId = value; }

    int getLineNumber() { return m_lineNumber; }
    void setLineNumber(int value) { m_lineNumber = value; }

    bool hasColumnNumber() { return m_columnNumber.isJust(); }
    int getColumnNumber(int defaultValue) { return m_columnNumber.isJust() ? m_columnNumber.fromJust() : defaultValue; }
    void setColumnNumber(int value) { m_columnNumber = value; }

    std::unique_ptr<protocol::DictionaryValue> serialize() const;
    std::unique_ptr<Location> clone() const;

    template<int STATE>
    class LocationBuilder {
    public:
        enum {
            NoFieldsSet = 0,
          ScriptIdSet = 1 << 1,
          LineNumberSet = 1 << 2,
            AllFieldsSet = (ScriptIdSet | LineNumberSet | 0)};


        LocationBuilder<STATE | ScriptIdSet>& setScriptId(const String16& value)
        {
            static_assert(!(STATE & ScriptIdSet), "property scriptId should not be set yet");
            m_result->setScriptId(value);
            return castState<ScriptIdSet>();
        }

        LocationBuilder<STATE | LineNumberSet>& setLineNumber(int value)
        {
            static_assert(!(STATE & LineNumberSet), "property lineNumber should not be set yet");
            m_result->setLineNumber(value);
            return castState<LineNumberSet>();
        }

        LocationBuilder<STATE>& setColumnNumber(int value)
        {
            m_result->setColumnNumber(value);
            return *this;
        }

        std::unique_ptr<Location> build()
        {
            static_assert(STATE == AllFieldsSet, "state should be AllFieldsSet");
            return std::move(m_result);
        }

    private:
        friend class Location;
        LocationBuilder() : m_result(new Location()) { }

        template<int STEP> LocationBuilder<STATE | STEP>& castState()
        {
            return *reinterpret_cast<LocationBuilder<STATE | STEP>*>(this);
        }

        std::unique_ptr<protocol::Debugger::Location> m_result;
    };

    static LocationBuilder<0> create()
    {
        return LocationBuilder<0>();
    }

private:
    Location()
    {
          m_lineNumber = 0;
    }

    String16 m_scriptId;
    int m_lineNumber;
    Maybe<int> m_columnNumber;
};


// Location in the source code.
class PLATFORM_EXPORT ScriptPosition {
    PROTOCOL_DISALLOW_COPY(ScriptPosition);
public:
    static std::unique_ptr<ScriptPosition> parse(protocol::Value* value, ErrorSupport* errors);

    ~ScriptPosition() { }

    int getLineNumber() { return m_lineNumber; }
    void setLineNumber(int value) { m_lineNumber = value; }

    int getColumnNumber() { return m_columnNumber; }
    void setColumnNumber(int value) { m_columnNumber = value; }

    std::unique_ptr<protocol::DictionaryValue> serialize() const;
    std::unique_ptr<ScriptPosition> clone() const;

    template<int STATE>
    class ScriptPositionBuilder {
    public:
        enum {
            NoFieldsSet = 0,
          LineNumberSet = 1 << 1,
          ColumnNumberSet = 1 << 2,
            AllFieldsSet = (LineNumberSet | ColumnNumberSet | 0)};


        ScriptPositionBuilder<STATE | LineNumberSet>& setLineNumber(int value)
        {
            static_assert(!(STATE & LineNumberSet), "property lineNumber should not be set yet");
            m_result->setLineNumber(value);
            return castState<LineNumberSet>();
        }

        ScriptPositionBuilder<STATE | ColumnNumberSet>& setColumnNumber(int value)
        {
            static_assert(!(STATE & ColumnNumberSet), "property columnNumber should not be set yet");
            m_result->setColumnNumber(value);
            return castState<ColumnNumberSet>();
        }

        std::unique_ptr<ScriptPosition> build()
        {
            static_assert(STATE == AllFieldsSet, "state should be AllFieldsSet");
            return std::move(m_result);
        }

    private:
        friend class ScriptPosition;
        ScriptPositionBuilder() : m_result(new ScriptPosition()) { }

        template<int STEP> ScriptPositionBuilder<STATE | STEP>& castState()
        {
            return *reinterpret_cast<ScriptPositionBuilder<STATE | STEP>*>(this);
        }

        std::unique_ptr<protocol::Debugger::ScriptPosition> m_result;
    };

    static ScriptPositionBuilder<0> create()
    {
        return ScriptPositionBuilder<0>();
    }

private:
    ScriptPosition()
    {
          m_lineNumber = 0;
          m_columnNumber = 0;
    }

    int m_lineNumber;
    int m_columnNumber;
};


// JavaScript call frame. Array of call frames form the call stack.
class PLATFORM_EXPORT CallFrame {
    PROTOCOL_DISALLOW_COPY(CallFrame);
public:
    static std::unique_ptr<CallFrame> parse(protocol::Value* value, ErrorSupport* errors);

    ~CallFrame() { }

    String16 getCallFrameId() { return m_callFrameId; }
    void setCallFrameId(const String16& value) { m_callFrameId = value; }

    String16 getFunctionName() { return m_functionName; }
    void setFunctionName(const String16& value) { m_functionName = value; }

    bool hasFunctionLocation() { return m_functionLocation.isJust(); }
    protocol::Debugger::Location* getFunctionLocation(protocol::Debugger::Location* defaultValue) { return m_functionLocation.isJust() ? m_functionLocation.fromJust() : defaultValue; }
    void setFunctionLocation(std::unique_ptr<protocol::Debugger::Location> value) { m_functionLocation = std::move(value); }

    protocol::Debugger::Location* getLocation() { return m_location.get(); }
    void setLocation(std::unique_ptr<protocol::Debugger::Location> value) { m_location = std::move(value); }

    protocol::Array<protocol::Debugger::Scope>* getScopeChain() { return m_scopeChain.get(); }
    void setScopeChain(std::unique_ptr<protocol::Array<protocol::Debugger::Scope>> value) { m_scopeChain = std::move(value); }

    protocol::Runtime::RemoteObject* getThis() { return m_this.get(); }
    void setThis(std::unique_ptr<protocol::Runtime::RemoteObject> value) { m_this = std::move(value); }

    bool hasReturnValue() { return m_returnValue.isJust(); }
    protocol::Runtime::RemoteObject* getReturnValue(protocol::Runtime::RemoteObject* defaultValue) { return m_returnValue.isJust() ? m_returnValue.fromJust() : defaultValue; }
    void setReturnValue(std::unique_ptr<protocol::Runtime::RemoteObject> value) { m_returnValue = std::move(value); }

    std::unique_ptr<protocol::DictionaryValue> serialize() const;
    std::unique_ptr<CallFrame> clone() const;

    template<int STATE>
    class CallFrameBuilder {
    public:
        enum {
            NoFieldsSet = 0,
          CallFrameIdSet = 1 << 1,
          FunctionNameSet = 1 << 2,
          LocationSet = 1 << 3,
          ScopeChainSet = 1 << 4,
          ThisSet = 1 << 5,
            AllFieldsSet = (CallFrameIdSet | FunctionNameSet | LocationSet | ScopeChainSet | ThisSet | 0)};


        CallFrameBuilder<STATE | CallFrameIdSet>& setCallFrameId(const String16& value)
        {
            static_assert(!(STATE & CallFrameIdSet), "property callFrameId should not be set yet");
            m_result->setCallFrameId(value);
            return castState<CallFrameIdSet>();
        }

        CallFrameBuilder<STATE | FunctionNameSet>& setFunctionName(const String16& value)
        {
            static_assert(!(STATE & FunctionNameSet), "property functionName should not be set yet");
            m_result->setFunctionName(value);
            return castState<FunctionNameSet>();
        }

        CallFrameBuilder<STATE>& setFunctionLocation(std::unique_ptr<protocol::Debugger::Location> value)
        {
            m_result->setFunctionLocation(std::move(value));
            return *this;
        }

        CallFrameBuilder<STATE | LocationSet>& setLocation(std::unique_ptr<protocol::Debugger::Location> value)
        {
            static_assert(!(STATE & LocationSet), "property location should not be set yet");
            m_result->setLocation(std::move(value));
            return castState<LocationSet>();
        }

        CallFrameBuilder<STATE | ScopeChainSet>& setScopeChain(std::unique_ptr<protocol::Array<protocol::Debugger::Scope>> value)
        {
            static_assert(!(STATE & ScopeChainSet), "property scopeChain should not be set yet");
            m_result->setScopeChain(std::move(value));
            return castState<ScopeChainSet>();
        }

        CallFrameBuilder<STATE | ThisSet>& setThis(std::unique_ptr<protocol::Runtime::RemoteObject> value)
        {
            static_assert(!(STATE & ThisSet), "property this should not be set yet");
            m_result->setThis(std::move(value));
            return castState<ThisSet>();
        }

        CallFrameBuilder<STATE>& setReturnValue(std::unique_ptr<protocol::Runtime::RemoteObject> value)
        {
            m_result->setReturnValue(std::move(value));
            return *this;
        }

        std::unique_ptr<CallFrame> build()
        {
            static_assert(STATE == AllFieldsSet, "state should be AllFieldsSet");
            return std::move(m_result);
        }

    private:
        friend class CallFrame;
        CallFrameBuilder() : m_result(new CallFrame()) { }

        template<int STEP> CallFrameBuilder<STATE | STEP>& castState()
        {
            return *reinterpret_cast<CallFrameBuilder<STATE | STEP>*>(this);
        }

        std::unique_ptr<protocol::Debugger::CallFrame> m_result;
    };

    static CallFrameBuilder<0> create()
    {
        return CallFrameBuilder<0>();
    }

private:
    CallFrame()
    {
    }

    String16 m_callFrameId;
    String16 m_functionName;
    Maybe<protocol::Debugger::Location> m_functionLocation;
    std::unique_ptr<protocol::Debugger::Location> m_location;
    std::unique_ptr<protocol::Array<protocol::Debugger::Scope>> m_scopeChain;
    std::unique_ptr<protocol::Runtime::RemoteObject> m_this;
    Maybe<protocol::Runtime::RemoteObject> m_returnValue;
};


// Scope description.
class PLATFORM_EXPORT Scope {
    PROTOCOL_DISALLOW_COPY(Scope);
public:
    static std::unique_ptr<Scope> parse(protocol::Value* value, ErrorSupport* errors);

    ~Scope() { }

    struct PLATFORM_EXPORT TypeEnum {
        static const char* Global;
        static const char* Local;
        static const char* With;
        static const char* Closure;
        static const char* Catch;
        static const char* Block;
        static const char* Script;
    }; // TypeEnum

    String16 getType() { return m_type; }
    void setType(const String16& value) { m_type = value; }

    protocol::Runtime::RemoteObject* getObject() { return m_object.get(); }
    void setObject(std::unique_ptr<protocol::Runtime::RemoteObject> value) { m_object = std::move(value); }

    bool hasName() { return m_name.isJust(); }
    String16 getName(const String16& defaultValue) { return m_name.isJust() ? m_name.fromJust() : defaultValue; }
    void setName(const String16& value) { m_name = value; }

    bool hasStartLocation() { return m_startLocation.isJust(); }
    protocol::Debugger::Location* getStartLocation(protocol::Debugger::Location* defaultValue) { return m_startLocation.isJust() ? m_startLocation.fromJust() : defaultValue; }
    void setStartLocation(std::unique_ptr<protocol::Debugger::Location> value) { m_startLocation = std::move(value); }

    bool hasEndLocation() { return m_endLocation.isJust(); }
    protocol::Debugger::Location* getEndLocation(protocol::Debugger::Location* defaultValue) { return m_endLocation.isJust() ? m_endLocation.fromJust() : defaultValue; }
    void setEndLocation(std::unique_ptr<protocol::Debugger::Location> value) { m_endLocation = std::move(value); }

    std::unique_ptr<protocol::DictionaryValue> serialize() const;
    std::unique_ptr<Scope> clone() const;

    template<int STATE>
    class ScopeBuilder {
    public:
        enum {
            NoFieldsSet = 0,
          TypeSet = 1 << 1,
          ObjectSet = 1 << 2,
            AllFieldsSet = (TypeSet | ObjectSet | 0)};


        ScopeBuilder<STATE | TypeSet>& setType(const String16& value)
        {
            static_assert(!(STATE & TypeSet), "property type should not be set yet");
            m_result->setType(value);
            return castState<TypeSet>();
        }

        ScopeBuilder<STATE | ObjectSet>& setObject(std::unique_ptr<protocol::Runtime::RemoteObject> value)
        {
            static_assert(!(STATE & ObjectSet), "property object should not be set yet");
            m_result->setObject(std::move(value));
            return castState<ObjectSet>();
        }

        ScopeBuilder<STATE>& setName(const String16& value)
        {
            m_result->setName(value);
            return *this;
        }

        ScopeBuilder<STATE>& setStartLocation(std::unique_ptr<protocol::Debugger::Location> value)
        {
            m_result->setStartLocation(std::move(value));
            return *this;
        }

        ScopeBuilder<STATE>& setEndLocation(std::unique_ptr<protocol::Debugger::Location> value)
        {
            m_result->setEndLocation(std::move(value));
            return *this;
        }

        std::unique_ptr<Scope> build()
        {
            static_assert(STATE == AllFieldsSet, "state should be AllFieldsSet");
            return std::move(m_result);
        }

    private:
        friend class Scope;
        ScopeBuilder() : m_result(new Scope()) { }

        template<int STEP> ScopeBuilder<STATE | STEP>& castState()
        {
            return *reinterpret_cast<ScopeBuilder<STATE | STEP>*>(this);
        }

        std::unique_ptr<protocol::Debugger::Scope> m_result;
    };

    static ScopeBuilder<0> create()
    {
        return ScopeBuilder<0>();
    }

private:
    Scope()
    {
    }

    String16 m_type;
    std::unique_ptr<protocol::Runtime::RemoteObject> m_object;
    Maybe<String16> m_name;
    Maybe<protocol::Debugger::Location> m_startLocation;
    Maybe<protocol::Debugger::Location> m_endLocation;
};


// Search match for resource.
class PLATFORM_EXPORT SearchMatch : public API::SearchMatch {
    PROTOCOL_DISALLOW_COPY(SearchMatch);
public:
    static std::unique_ptr<SearchMatch> parse(protocol::Value* value, ErrorSupport* errors);

    ~SearchMatch() { }

    double getLineNumber() { return m_lineNumber; }
    void setLineNumber(double value) { m_lineNumber = value; }

    String16 getLineContent() { return m_lineContent; }
    void setLineContent(const String16& value) { m_lineContent = value; }

    std::unique_ptr<protocol::DictionaryValue> serialize() const;
    std::unique_ptr<SearchMatch> clone() const;
    String16 toJSONString() const override;

    template<int STATE>
    class SearchMatchBuilder {
    public:
        enum {
            NoFieldsSet = 0,
          LineNumberSet = 1 << 1,
          LineContentSet = 1 << 2,
            AllFieldsSet = (LineNumberSet | LineContentSet | 0)};


        SearchMatchBuilder<STATE | LineNumberSet>& setLineNumber(double value)
        {
            static_assert(!(STATE & LineNumberSet), "property lineNumber should not be set yet");
            m_result->setLineNumber(value);
            return castState<LineNumberSet>();
        }

        SearchMatchBuilder<STATE | LineContentSet>& setLineContent(const String16& value)
        {
            static_assert(!(STATE & LineContentSet), "property lineContent should not be set yet");
            m_result->setLineContent(value);
            return castState<LineContentSet>();
        }

        std::unique_ptr<SearchMatch> build()
        {
            static_assert(STATE == AllFieldsSet, "state should be AllFieldsSet");
            return std::move(m_result);
        }

    private:
        friend class SearchMatch;
        SearchMatchBuilder() : m_result(new SearchMatch()) { }

        template<int STEP> SearchMatchBuilder<STATE | STEP>& castState()
        {
            return *reinterpret_cast<SearchMatchBuilder<STATE | STEP>*>(this);
        }

        std::unique_ptr<protocol::Debugger::SearchMatch> m_result;
    };

    static SearchMatchBuilder<0> create()
    {
        return SearchMatchBuilder<0>();
    }

private:
    SearchMatch()
    {
          m_lineNumber = 0;
    }

    double m_lineNumber;
    String16 m_lineContent;
};


// ------------- Backend interface.

class PLATFORM_EXPORT Backend {
public:
    virtual void enable(ErrorString*) = 0;
    virtual void disable(ErrorString*) = 0;
    virtual void setBreakpointsActive(ErrorString*, bool in_active) = 0;
    virtual void setSkipAllPauses(ErrorString*, bool in_skipped) = 0;
    virtual void setBreakpointByUrl(ErrorString*, int in_lineNumber, const Maybe<String16>& in_url, const Maybe<String16>& in_urlRegex, const Maybe<int>& in_columnNumber, const Maybe<String16>& in_condition, String16* out_breakpointId, std::unique_ptr<protocol::Array<protocol::Debugger::Location>>* out_locations) = 0;
    virtual void setBreakpoint(ErrorString*, std::unique_ptr<protocol::Debugger::Location> in_location, const Maybe<String16>& in_condition, String16* out_breakpointId, std::unique_ptr<protocol::Debugger::Location>* out_actualLocation) = 0;
    virtual void removeBreakpoint(ErrorString*, const String16& in_breakpointId) = 0;
    virtual void continueToLocation(ErrorString*, std::unique_ptr<protocol::Debugger::Location> in_location, const Maybe<bool>& in_interstatementLocation) = 0;
    virtual void stepOver(ErrorString*) = 0;
    virtual void stepInto(ErrorString*) = 0;
    virtual void stepOut(ErrorString*) = 0;
    virtual void pause(ErrorString*) = 0;
    virtual void resume(ErrorString*) = 0;
    virtual void searchInContent(ErrorString*, const String16& in_scriptId, const String16& in_query, const Maybe<bool>& in_caseSensitive, const Maybe<bool>& in_isRegex, std::unique_ptr<protocol::Array<protocol::Debugger::SearchMatch>>* out_result) = 0;
    virtual void canSetScriptSource(ErrorString*, bool* out_result) = 0;
    virtual void setScriptSource(ErrorString*, const String16& in_scriptId, const String16& in_scriptSource, const Maybe<bool>& in_preview, Maybe<protocol::Array<protocol::Debugger::CallFrame>>* out_callFrames, Maybe<bool>* out_stackChanged, Maybe<protocol::Runtime::StackTrace>* out_asyncStackTrace, Maybe<protocol::Runtime::ExceptionDetails>* out_compileError) = 0;
    virtual void restartFrame(ErrorString*, const String16& in_callFrameId, std::unique_ptr<protocol::Array<protocol::Debugger::CallFrame>>* out_callFrames, Maybe<protocol::Runtime::StackTrace>* out_asyncStackTrace) = 0;
    virtual void getScriptSource(ErrorString*, const String16& in_scriptId, String16* out_scriptSource) = 0;
    virtual void setPauseOnExceptions(ErrorString*, const String16& in_state) = 0;
    virtual void evaluateOnCallFrame(ErrorString*, const String16& in_callFrameId, const String16& in_expression, const Maybe<String16>& in_objectGroup, const Maybe<bool>& in_includeCommandLineAPI, const Maybe<bool>& in_doNotPauseOnExceptionsAndMuteConsole, const Maybe<bool>& in_returnByValue, const Maybe<bool>& in_generatePreview, std::unique_ptr<protocol::Runtime::RemoteObject>* out_result, Maybe<bool>* out_wasThrown, Maybe<protocol::Runtime::ExceptionDetails>* out_exceptionDetails) = 0;
    virtual void setVariableValue(ErrorString*, int in_scopeNumber, const String16& in_variableName, std::unique_ptr<protocol::Runtime::CallArgument> in_newValue, const String16& in_callFrameId) = 0;
    virtual void getBacktrace(ErrorString*, std::unique_ptr<protocol::Array<protocol::Debugger::CallFrame>>* out_callFrames, Maybe<protocol::Runtime::StackTrace>* out_asyncStackTrace) = 0;
    virtual void setAsyncCallStackDepth(ErrorString*, int in_maxDepth) = 0;
    virtual void setBlackboxPatterns(ErrorString*, std::unique_ptr<protocol::Array<String16>> in_patterns) = 0;
    virtual void setBlackboxedRanges(ErrorString*, const String16& in_scriptId, std::unique_ptr<protocol::Array<protocol::Debugger::ScriptPosition>> in_positions) = 0;


protected:
    virtual ~Backend() { }
};

// ------------- Frontend interface.

class PLATFORM_EXPORT Frontend {
public:
    Frontend(FrontendChannel* frontendChannel) : m_frontendChannel(frontendChannel) { }
    void scriptParsed(const String16& scriptId, const String16& url, int startLine, int startColumn, int endLine, int endColumn, int executionContextId, const String16& hash, const Maybe<protocol::DictionaryValue>& executionContextAuxData = Maybe<protocol::DictionaryValue>(), const Maybe<bool>& isInternalScript = Maybe<bool>(), const Maybe<bool>& isLiveEdit = Maybe<bool>(), const Maybe<String16>& sourceMapURL = Maybe<String16>(), const Maybe<bool>& hasSourceURL = Maybe<bool>(), const Maybe<bool>& deprecatedCommentWasUsed = Maybe<bool>());
    void scriptFailedToParse(const String16& scriptId, const String16& url, int startLine, int startColumn, int endLine, int endColumn, int executionContextId, const String16& hash, const Maybe<protocol::DictionaryValue>& executionContextAuxData = Maybe<protocol::DictionaryValue>(), const Maybe<bool>& isInternalScript = Maybe<bool>(), const Maybe<String16>& sourceMapURL = Maybe<String16>(), const Maybe<bool>& hasSourceURL = Maybe<bool>(), const Maybe<bool>& deprecatedCommentWasUsed = Maybe<bool>());
    void breakpointResolved(const String16& breakpointId, std::unique_ptr<protocol::Debugger::Location> location);
    void paused(std::unique_ptr<protocol::Array<protocol::Debugger::CallFrame>> callFrames, const String16& reason, const Maybe<protocol::DictionaryValue>& data = Maybe<protocol::DictionaryValue>(), const Maybe<protocol::Array<String16>>& hitBreakpoints = Maybe<protocol::Array<String16>>(), const Maybe<protocol::Runtime::StackTrace>& asyncStackTrace = Maybe<protocol::Runtime::StackTrace>());
    void resumed();

    void flush() { m_frontendChannel->flushProtocolNotifications(); }
private:
    FrontendChannel* m_frontendChannel;
};

// ------------- Dispatcher.

class PLATFORM_EXPORT Dispatcher {
public:
    static void wire(UberDispatcher*, blink::protocol::Debugger::Backend*);

private:
    Dispatcher() { }
};

// ------------- Metainfo.

class PLATFORM_EXPORT Metainfo {
public:
    using BackendClass = Backend;
    using FrontendClass = Frontend;
    using DispatcherClass = Dispatcher;
    static const char domainName[];
    static const char commandPrefix[];
};

} // namespace Debugger
} // namespace protocol
} // namespace blink

#endif // !defined(protocol_Debugger_h)
