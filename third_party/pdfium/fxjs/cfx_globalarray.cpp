// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/cfx_globalarray.h"

#include <utility>

#include "fxjs/cfx_keyvalue.h"

CFX_GlobalArray::CFX_GlobalArray() = default;

CFX_GlobalArray::CFX_GlobalArray(CFX_GlobalArray&& that) = default;

CFX_GlobalArray::~CFX_GlobalArray() = default;

CFX_GlobalArray& CFX_GlobalArray::operator=(CFX_GlobalArray&& array) = default;

void CFX_GlobalArray::Add(std::unique_ptr<CFX_KeyValue> pKeyValue) {
  m_Array.push_back(std::move(pKeyValue));
}

size_t CFX_GlobalArray::Count() const {
  return m_Array.size();
}

CFX_KeyValue* CFX_GlobalArray::GetAt(int index) const {
  return m_Array.at(index).get();
}
