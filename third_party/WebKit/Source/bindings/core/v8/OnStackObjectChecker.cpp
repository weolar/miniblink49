// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "bindings/core/v8/OnStackObjectChecker.h"

#include "bindings/core/v8/Dictionary.h"
#include "wtf/HashSet.h"

namespace blink {

#if ENABLE(ASSERT)
OnStackObjectChecker::~OnStackObjectChecker()
{
    // All dictionaries must be removed before the checker is destructed.
    ASSERT(m_dictionaries.isEmpty());
}

void OnStackObjectChecker::add(Dictionary* dictionary)
{
    m_dictionaries.add(dictionary);
}

void OnStackObjectChecker::remove(Dictionary* dictionary)
{
    m_dictionaries.remove(dictionary);
}
#endif // ENABLE(ASSERT)

} // namespace blink
