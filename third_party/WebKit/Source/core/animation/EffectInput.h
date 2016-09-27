// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef EffectInput_h
#define EffectInput_h

#include "core/CoreExport.h"
#include "core/animation/EffectModel.h"
#include "wtf/Vector.h"

namespace blink {

class EffectModel;
class EffectModelOrDictionarySequence;
class Dictionary;
class Element;
class ExceptionState;

class CORE_EXPORT EffectInput {
public:
    static PassRefPtrWillBeRawPtr<EffectModel> convert(Element*, const Vector<Dictionary>& keyframeDictionaryVector, ExceptionState&);
    static PassRefPtrWillBeRawPtr<EffectModel> convert(Element*, const EffectModelOrDictionarySequence&, ExceptionState&);
};

} // namespace blink

#endif
