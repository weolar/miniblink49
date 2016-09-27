// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ScopeRecorder_h
#define ScopeRecorder_h

#include "core/CoreExport.h"
#include "platform/graphics/paint/DisplayItem.h"

namespace blink {

class DisplayItemList;
class GraphicsContext;
class LayoutObject;

class CORE_EXPORT ScopeRecorder {
public:
    ScopeRecorder(GraphicsContext&, const DisplayItemClientWrapper&);

    ~ScopeRecorder();

private:
    DisplayItemList* m_displayItemList;
    DisplayItemClientWrapper m_object;
};

} // namespace blink

#endif // ScopeRecorder_h
