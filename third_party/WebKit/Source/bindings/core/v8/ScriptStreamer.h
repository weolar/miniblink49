// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ScriptStreamer_h
#define ScriptStreamer_h

#include "core/CoreExport.h"
#include "core/dom/PendingScript.h"
#include "platform/heap/Handle.h"
#include "wtf/RefCounted.h"

#include <v8.h>

namespace blink {

class PendingScript;
class Resource;
class ScriptResource;
class ScriptResourceClient;
class ScriptState;
class Settings;
class SourceStream;

// ScriptStreamer streams incomplete script data to V8 so that it can be parsed
// while it's loaded. PendingScript holds a reference to ScriptStreamer. At the
// moment, ScriptStreamer is only used for parser blocking scripts; this means
// that the Document stays stable and no other scripts are executing while we're
// streaming. It is possible, though, that Document and the PendingScript are
// destroyed while the streaming is in progress, and ScriptStreamer handles it
// gracefully.
class CORE_EXPORT ScriptStreamer final : public RefCountedWillBeRefCountedGarbageCollected<ScriptStreamer> {
    WTF_MAKE_NONCOPYABLE(ScriptStreamer);
public:
    static PassRefPtrWillBeRawPtr<ScriptStreamer> create(ScriptResource* resource, PendingScript::Type scriptType, ScriptState* scriptState, v8::ScriptCompiler::CompileOptions compileOptions)
    {
        return adoptRefWillBeNoop(new ScriptStreamer(resource, scriptType, scriptState, compileOptions));
    }

    ~ScriptStreamer();
    DECLARE_TRACE();

    // Launches a task (on a background thread) which will stream the given
    // PendingScript into V8 as it loads.
    static void startStreaming(PendingScript&, PendingScript::Type, Settings*, ScriptState*);

    // Returns false if we cannot stream the given encoding.
    static bool convertEncoding(const char* encodingName, v8::ScriptCompiler::StreamedSource::Encoding*);

    bool isFinished() const;

    v8::ScriptCompiler::StreamedSource* source() { return m_source.get(); }
    ScriptResource* resource() const { return m_resource; }

    // Called when the script is not needed any more (e.g., loading was
    // cancelled). After calling cancel, PendingScript can drop its reference to
    // ScriptStreamer, and ScriptStreamer takes care of eventually deleting
    // itself (after the V8 side has finished too).
    void cancel();

    // When the streaming is suppressed, the data is not given to V8, but
    // ScriptStreamer still watches the resource load and notifies the upper
    // layers when loading is finished. It is used in situations when we have
    // started streaming but then we detect we don't want to stream (e.g., when
    // we have the code cache for the script) and we still want to parse and
    // execute it when it has finished loading.
    void suppressStreaming();
    bool streamingSuppressed() const { return m_streamingSuppressed; }

    v8::ScriptCompiler::CompileOptions compileOptions() const
    {
        return m_compileOptions;
    }

    void addClient(ScriptResourceClient* client)
    {
        ASSERT(!m_client);
        m_client = client;
        notifyFinishedToClient();
    }

    void removeClient(ScriptResourceClient* client)
    {
        ASSERT(m_client == client);
        m_client = 0;
    }

    // Called by PendingScript when data arrives from the network.
    void notifyAppendData(ScriptResource*);
    void notifyFinished(Resource*);

    // Called by ScriptStreamingTask when it has streamed all data to V8 and V8
    // has processed it.
    void streamingCompleteOnBackgroundThread();

    v8::ScriptCompiler::StreamedSource::Encoding encoding() const { return m_encoding; }

    static void setSmallScriptThresholdForTesting(size_t threshold)
    {
        kSmallScriptThreshold = threshold;
    }

    static size_t smallScriptThreshold() { return kSmallScriptThreshold; }

private:
    // Scripts whose first data chunk is smaller than this constant won't be
    // streamed. Non-const for testing.
    static size_t kSmallScriptThreshold;

    ScriptStreamer(ScriptResource*, PendingScript::Type, ScriptState*, v8::ScriptCompiler::CompileOptions);

    void streamingComplete();
    void notifyFinishedToClient();

    static bool startStreamingInternal(PendingScript&, PendingScript::Type, Settings*, ScriptState*);

    // This pointer is weak. If PendingScript and its Resource are deleted
    // before ScriptStreamer, PendingScript will notify ScriptStreamer of its
    // deletion by calling cancel().
    RawPtrWillBeMember<ScriptResource> m_resource;
    // Whether ScriptStreamer is detached from the Resource. In those cases, the
    // script data is not needed any more, and the client won't get notified
    // when the loading and streaming are done.
    bool m_detached;

    SourceStream* m_stream;
    OwnPtr<v8::ScriptCompiler::StreamedSource> m_source;
    ScriptResourceClient* m_client;
    bool m_loadingFinished; // Whether loading from the network is done.
    // Whether the V8 side processing is done. Will be used by the main thread
    // and the streamer thread; guarded by m_mutex.
    bool m_parsingFinished;
    // Whether we have received enough data to start the streaming.
    bool m_haveEnoughDataForStreaming;

    // Whether the script source code should be retrieved from the Resource
    // instead of the ScriptStreamer; guarded by m_mutex.
    bool m_streamingSuppressed;

    // What kind of cached data V8 produces during streaming.
    v8::ScriptCompiler::CompileOptions m_compileOptions;

    RefPtr<ScriptState> m_scriptState;

    // For recording metrics for different types of scripts separately.
    PendingScript::Type m_scriptType;

    mutable Mutex m_mutex;

    // Encoding of the streamed script. Saved for sanity checking purposes.
    v8::ScriptCompiler::StreamedSource::Encoding m_encoding;
};

} // namespace blink

#endif // ScriptStreamer_h
