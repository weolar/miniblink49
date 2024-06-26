// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXJS_CFX_GLOBALARRAY_H_
#define FXJS_CFX_GLOBALARRAY_H_

#include <memory>
#include <vector>

class CFX_KeyValue;

class CFX_GlobalArray {
 public:
  CFX_GlobalArray();
  CFX_GlobalArray(const CFX_GlobalArray& that) = delete;
  CFX_GlobalArray(CFX_GlobalArray&& that);
  ~CFX_GlobalArray();

  CFX_GlobalArray& operator=(const CFX_GlobalArray& array) = delete;
  CFX_GlobalArray& operator=(CFX_GlobalArray&& array);

  void Add(std::unique_ptr<CFX_KeyValue> pKeyValue);
  size_t Count() const;
  CFX_KeyValue* GetAt(int index) const;

 private:
  std::vector<std::unique_ptr<CFX_KeyValue>> m_Array;
};

#endif  // FXJS_CFX_GLOBALARRAY_H_
