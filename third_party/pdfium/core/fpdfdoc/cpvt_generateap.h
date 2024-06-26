// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFDOC_CPVT_GENERATEAP_H_
#define CORE_FPDFDOC_CPVT_GENERATEAP_H_

#include "core/fpdfdoc/cpdf_annot.h"
#include "core/fpdfdoc/cpdf_defaultappearance.h"
#include "core/fpdfdoc/cpdf_variabletext.h"
#include "core/fxcrt/fx_coordinates.h"
#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/fx_system.h"

class CPDF_Dictionary;
class CPDF_Document;
struct CPVT_Dash;

class CPVT_GenerateAP {
 public:
  enum Type { kTextField, kComboBox, kListBox };

  static void GenerateFormAP(Type type,
                             CPDF_Document* pDoc,
                             CPDF_Dictionary* pAnnotDict);
  static void GenerateEmptyAP(CPDF_Document* pDoc, CPDF_Dictionary* pAnnotDict);

  static bool GenerateAnnotAP(CPDF_Annot::Subtype subtype,
                              CPDF_Document* pDoc,
                              CPDF_Dictionary* pAnnotDict);
};

#endif  // CORE_FPDFDOC_CPVT_GENERATEAP_H_
