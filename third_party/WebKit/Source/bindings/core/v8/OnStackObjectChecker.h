// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OnStackObjectChecker_h
#define OnStackObjectChecker_h

#include "core/CoreExport.h"
#include "wtf/HashSet.h"
#include "wtf/Noncopyable.h"

namespace blink {

#if ENABLE(ASSERT)
class Dictionary;

// A lifetime checker for C++ objects which must not outlive the owner of an
// instance of this class. In general, we should rely on Oilpan or RefCounted
// to manage object lifetime. However, we don't use them for some objects
// (e.g. Dictionary) for performance reason. This checker is for such objects.
// The C++ objects to be checked must call add() on creation, and remove() on
// destruction, respectively.
class CORE_EXPORT OnStackObjectChecker final {
    WTF_MAKE_NONCOPYABLE(OnStackObjectChecker);
public:
    OnStackObjectChecker() { }
    ~OnStackObjectChecker();

    void add(Dictionary*);
    void remove(Dictionary*);

private:
    HashSet<Dictionary*> m_dictionaries;
};
#endif

} // namespace blink

#endif // OnStackObjectChecker_h
