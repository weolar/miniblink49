// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFDOC_CPDF_METADATA_H_
#define CORE_FPDFDOC_CPDF_METADATA_H_

#include <vector>

#include "core/fxcrt/unowned_ptr.h"

class CPDF_Stream;

enum class UnsupportedFeature : uint8_t {
  kDocumentXFAForm = 1,
  kDocumentPortableCollection = 2,
  kDocumentAttachment = 3,
  kDocumentSecurity = 4,
  kDocumentSharedReview = 5,
  kDocumentSharedFormAcrobat = 6,
  kDocumentSharedFormFilesystem = 7,
  kDocumentSharedFormEmail = 8,

  kAnnotation3d = 11,
  kAnnotationMovie = 12,
  kAnnotationSound = 13,
  kAnnotationScreenMedia = 14,
  kAnnotationScreenRichMedia = 15,
  kAnnotationAttachment = 16,
  kAnnotationSignature = 17
};

class CPDF_Metadata {
 public:
  explicit CPDF_Metadata(const CPDF_Stream* pStream);
  ~CPDF_Metadata();

  std::vector<UnsupportedFeature> CheckForSharedForm() const;

 private:
  UnownedPtr<const CPDF_Stream> stream_;
};

#endif  // CORE_FPDFDOC_CPDF_METADATA_H_
