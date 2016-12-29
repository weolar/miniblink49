// Copyright 2014 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/snapshot/natives.h"

#include "src/base/logging.h"
#include "src/list.h"
#include "src/list-inl.h"
#include "src/snapshot/snapshot-source-sink.h"
#include "src/vector.h"

namespace v8 {
    namespace internal {
        // Implement NativesCollection<T> bsaed on NativesHolder + NativesStore.
        //
        // (The callers expect a purely static interface, since this is how the
        //  natives are usually compiled in. Since we implement them based on
        //  runtime content, we have to implement this indirection to offer
        //  a static interface.)
        template<NativeType type>
        int NativesCollection<type>::GetBuiltinsCount() {
            return 0;
        }

        template<NativeType type>
        int NativesCollection<type>::GetDebuggerCount() {
            return 0;
        }

        template<NativeType type>
        int NativesCollection<type>::GetIndex(const char* name) {
            return 0;
        }

        template <NativeType type>
        Vector<const char> NativesCollection<type>::GetScriptSource(int index) {
            return Vector<const char>();
        }

        template<NativeType type>
        Vector<const char> NativesCollection<type>::GetScriptName(int index) {
            return Vector<const char>();
        }

        template <NativeType type>
        Vector<const char> NativesCollection<type>::GetScriptsSource() {
            return Vector<const char>();
        }


        // The compiler can't 'see' all uses of the static methods and hence
        // my choice to elide them. This we'll explicitly instantiate these.
        template class NativesCollection < CORE > ;
        template class NativesCollection < EXPERIMENTAL > ;
        template class NativesCollection < EXTRAS > ;
    }  // namespace internal
}  // namespace v8