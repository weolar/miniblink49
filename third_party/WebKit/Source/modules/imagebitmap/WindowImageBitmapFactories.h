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

#ifndef WindowImageBitmapFactories_h
#define WindowImageBitmapFactories_h

#include "bindings/core/v8/ScriptPromise.h"
#include "bindings/core/v8/ScriptPromiseResolver.h"
#include "bindings/core/v8/ScriptState.h"
#include "platform/geometry/IntRect.h"
#include "wtf/Forward.h"
#include "wtf/HashSet.h"

namespace blink {

class CanvasRenderingContext2D;
class EventTarget;
class ExceptionState;
class HTMLCanvasElement;
class HTMLImageElement;
class HTMLVideoElement;

class WindowImageBitmapFactories {
public:
    static ScriptPromise createImageBitmap(ScriptState*, EventTarget&, HTMLImageElement*, ExceptionState&);
    static ScriptPromise createImageBitmap(ScriptState*, EventTarget&, HTMLImageElement*, int sx, int sy, int sw, int sh, ExceptionState&);
    static ScriptPromise createImageBitmap(ScriptState*, EventTarget&, HTMLVideoElement*, ExceptionState&);
    static ScriptPromise createImageBitmap(ScriptState*, EventTarget&, HTMLVideoElement*, int sx, int sy, int sw, int sh, ExceptionState&);
    static ScriptPromise createImageBitmap(ScriptState*, EventTarget&, CanvasRenderingContext2D*, ExceptionState&);
    static ScriptPromise createImageBitmap(ScriptState*, EventTarget&, CanvasRenderingContext2D*, int sx, int sy, int sw, int sh, ExceptionState&);
    static ScriptPromise createImageBitmap(ScriptState*, EventTarget&, HTMLCanvasElement*, ExceptionState&);
    static ScriptPromise createImageBitmap(ScriptState*, EventTarget&, HTMLCanvasElement*, int sx, int sy, int sw, int sh, ExceptionState&);
};

} // namespace blink

#endif // WindowImageBitmapFactories_h
