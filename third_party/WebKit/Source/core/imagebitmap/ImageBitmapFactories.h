/*
 * Copyright (c) 2013, Google Inc. All rights reserved.
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

#ifndef ImageBitmapFactories_h
#define ImageBitmapFactories_h

#include "bindings/core/v8/ScriptPromise.h"
#include "bindings/core/v8/ScriptPromiseResolver.h"
#include "bindings/core/v8/ScriptState.h"
#include "core/fileapi/FileReaderLoader.h"
#include "core/fileapi/FileReaderLoaderClient.h"
#include "platform/Supplementable.h"
#include "platform/geometry/IntRect.h"
#include "wtf/Forward.h"
#include "wtf/HashSet.h"

namespace blink {

class Blob;
class EventTarget;
class ExceptionState;
class ImageBitmap;
class ImageData;
class ExecutionContext;

class ImageBitmapFactories final : public NoBaseWillBeGarbageCollectedFinalized<ImageBitmapFactories>, public WillBeHeapSupplement<LocalDOMWindow>, public WillBeHeapSupplement<WorkerGlobalScope> {
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(ImageBitmapFactories);

public:
    static ScriptPromise createImageBitmap(ScriptState*, EventTarget&, Blob*, ExceptionState&);
    static ScriptPromise createImageBitmap(ScriptState*, EventTarget&, Blob*, int sx, int sy, int sw, int sh, ExceptionState&);
    static ScriptPromise createImageBitmap(ScriptState*, EventTarget&, ImageData*, ExceptionState&);
    static ScriptPromise createImageBitmap(ScriptState*, EventTarget&, ImageData*, int sx, int sy, int sw, int sh, ExceptionState&);
    static ScriptPromise createImageBitmap(ScriptState*, EventTarget&, ImageBitmap*, ExceptionState&);
    static ScriptPromise createImageBitmap(ScriptState*, EventTarget&, ImageBitmap*, int sx, int sy, int sw, int sh, ExceptionState&);

    virtual ~ImageBitmapFactories() { }

    DECLARE_TRACE();

protected:
    static const char* supplementName();

private:
    class ImageBitmapLoader final : public GarbageCollectedFinalized<ImageBitmapLoader>, public FileReaderLoaderClient {
    public:
        static ImageBitmapLoader* create(ImageBitmapFactories& factory, const IntRect& cropRect, ScriptState* scriptState)
        {
            return new ImageBitmapLoader(factory, cropRect, scriptState);
        }

        void loadBlobAsync(ExecutionContext*, Blob*);
        ScriptPromise promise() { return m_resolver->promise(); }

        DECLARE_TRACE();

        virtual ~ImageBitmapLoader() { }

    private:
        ImageBitmapLoader(ImageBitmapFactories&, const IntRect&, ScriptState*);

        void rejectPromise();

        // FileReaderLoaderClient
        virtual void didStartLoading() override { }
        virtual void didReceiveData() override { }
        virtual void didFinishLoading() override;
        virtual void didFail(FileError::ErrorCode) override;

        FileReaderLoader m_loader;
        RawPtrWillBeMember<ImageBitmapFactories> m_factory;
        RefPtrWillBeMember<ScriptPromiseResolver> m_resolver;
        IntRect m_cropRect;
    };

    static ImageBitmapFactories& from(EventTarget&);

    template<class GlobalObject>
    static ImageBitmapFactories& fromInternal(GlobalObject&);

    void addLoader(ImageBitmapLoader*);
    void didFinishLoading(ImageBitmapLoader*);

    PersistentHeapHashSetWillBeHeapHashSet<Member<ImageBitmapLoader>> m_pendingLoaders;
};

} // namespace blink

#endif // ImageBitmapFactories_h
