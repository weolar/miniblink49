// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_PARSER_CPDF_STRING_H_
#define CORE_FPDFAPI_PARSER_CPDF_STRING_H_

#include <memory>

#include "core/fpdfapi/parser/cpdf_object.h"
#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/fx_system.h"
#include "core/fxcrt/string_pool_template.h"
#include "core/fxcrt/weak_ptr.h"

class CPDF_String final : public CPDF_Object {
 public:
  CPDF_String();
  CPDF_String(WeakPtr<ByteStringPool> pPool, const ByteString& str, bool bHex);
  CPDF_String(WeakPtr<ByteStringPool> pPool, const WideString& str);
  ~CPDF_String() override;

  // CPDF_Object:
  Type GetType() const override;
  std::unique_ptr<CPDF_Object> Clone() const override;
  ByteString GetString() const override;
  WideString GetUnicodeText() const override;
  void SetString(const ByteString& str) override;
  bool IsString() const override;
  CPDF_String* AsString() override;
  const CPDF_String* AsString() const override;
  bool WriteTo(IFX_ArchiveStream* archive,
               const CPDF_Encryptor* encryptor) const override;

  bool IsHex() const { return m_bHex; }

 private:
  ByteString m_String;
  bool m_bHex;
};

inline CPDF_String* ToString(CPDF_Object* obj) {
  return obj ? obj->AsString() : nullptr;
}

inline const CPDF_String* ToString(const CPDF_Object* obj) {
  return obj ? obj->AsString() : nullptr;
}

#endif  // CORE_FPDFAPI_PARSER_CPDF_STRING_H_
