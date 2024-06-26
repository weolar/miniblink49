// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFDOC_CPDF_NUMBERTREE_H_
#define CORE_FPDFDOC_CPDF_NUMBERTREE_H_

#include "core/fxcrt/unowned_ptr.h"

class CPDF_Dictionary;
class CPDF_Object;

class CPDF_NumberTree {
 public:
  explicit CPDF_NumberTree(const CPDF_Dictionary* pRoot);
  ~CPDF_NumberTree();

  const CPDF_Object* LookupValue(int num) const;

 protected:
  UnownedPtr<const CPDF_Dictionary> const m_pRoot;
};

#endif  // CORE_FPDFDOC_CPDF_NUMBERTREE_H_
