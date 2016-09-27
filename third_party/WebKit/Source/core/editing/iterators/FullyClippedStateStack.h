// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FullyClippedStateStack_h
#define FullyClippedStateStack_h

#include "core/editing/EditingStrategy.h"
#include "core/editing/iterators/BitStack.h"

namespace blink {

template<typename Strategy>
class FullyClippedStateStackAlgorithm : public BitStack {
public:
    FullyClippedStateStackAlgorithm();
    ~FullyClippedStateStackAlgorithm();

    void pushFullyClippedState(Node*);
    void setUpFullyClippedStack(Node*);
};

extern template class FullyClippedStateStackAlgorithm<EditingStrategy>;
extern template class FullyClippedStateStackAlgorithm<EditingInComposedTreeStrategy>;

using FullyClippedStateStack = FullyClippedStateStackAlgorithm<EditingStrategy>;

} // namespace blink

#endif // FullyClippedStateStack_h
