// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFDOC_CPDF_VIEWERPREFERENCES_H_
#define CORE_FPDFDOC_CPDF_VIEWERPREFERENCES_H_

#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/fx_system.h"
#include "core/fxcrt/unowned_ptr.h"
#include "third_party/base/optional.h"

class CPDF_Array;
class CPDF_Dictionary;
class CPDF_Document;

class CPDF_ViewerPreferences {
 public:
  explicit CPDF_ViewerPreferences(const CPDF_Document* pDoc);
  ~CPDF_ViewerPreferences();

  bool IsDirectionR2L() const;
  bool PrintScaling() const;
  int32_t NumCopies() const;
  const CPDF_Array* PrintPageRange() const;
  ByteString Duplex() const;

  // Gets the entry for |bsKey|.
  Optional<ByteString> GenericName(const ByteString& bsKey) const;

 private:
  const CPDF_Dictionary* GetViewerPreferences() const;

  UnownedPtr<const CPDF_Document> const m_pDoc;
};

#endif  // CORE_FPDFDOC_CPDF_VIEWERPREFERENCES_H_
