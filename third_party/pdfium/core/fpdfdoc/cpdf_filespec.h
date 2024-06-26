// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFDOC_CPDF_FILESPEC_H_
#define CORE_FPDFDOC_CPDF_FILESPEC_H_

#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/string_pool_template.h"
#include "core/fxcrt/unowned_ptr.h"
#include "core/fxcrt/weak_ptr.h"

class CPDF_Dictionary;
class CPDF_Object;
class CPDF_Stream;

class CPDF_FileSpec {
 public:
  explicit CPDF_FileSpec(const CPDF_Object* pObj);
  explicit CPDF_FileSpec(CPDF_Object* pObj);
  ~CPDF_FileSpec();

  // Convert a platform dependent file name into pdf format.
  static WideString EncodeFileName(const WideString& filepath);

  // Convert a pdf file name into platform dependent format.
  static WideString DecodeFileName(const WideString& filepath);

  const CPDF_Object* GetObj() const { return m_pObj.Get(); }
  CPDF_Object* GetObj() { return m_pWritableObj.Get(); }
  WideString GetFileName() const;
  const CPDF_Stream* GetFileStream() const;
  CPDF_Stream* GetFileStream();
  const CPDF_Dictionary* GetParamsDict() const;
  CPDF_Dictionary* GetParamsDict();

  // Set this file spec to refer to a file name (not a url).
  void SetFileName(const WideString& wsFileName);

 private:
  UnownedPtr<const CPDF_Object> const m_pObj;
  UnownedPtr<CPDF_Object> const m_pWritableObj;
};

#endif  // CORE_FPDFDOC_CPDF_FILESPEC_H_
