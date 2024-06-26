// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFDOC_CPDF_APSETTINGS_H_
#define CORE_FPDFDOC_CPDF_APSETTINGS_H_

#include "core/fpdfdoc/cpdf_iconfit.h"
#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/fx_system.h"
#include "core/fxcrt/unowned_ptr.h"
#include "core/fxge/fx_dib.h"

class CPDF_Dictionary;
class CPDF_FormControl;
class CPDF_Stream;

class CPDF_ApSettings {
 public:
  explicit CPDF_ApSettings(CPDF_Dictionary* pDict);
  CPDF_ApSettings(const CPDF_ApSettings& that);
  ~CPDF_ApSettings();

  bool HasMKEntry(const ByteString& csEntry) const;
  int GetRotation() const;

  FX_ARGB GetBorderColor(int& iColorType) const {
    return GetColor(iColorType, "BC");
  }

  float GetOriginalBorderColor(int index) const {
    return GetOriginalColor(index, "BC");
  }

  void GetOriginalBorderColor(int& iColorType, float fc[4]) const {
    GetOriginalColor(iColorType, fc, "BC");
  }

  FX_ARGB GetBackgroundColor(int& iColorType) const {
    return GetColor(iColorType, "BG");
  }

  float GetOriginalBackgroundColor(int index) const {
    return GetOriginalColor(index, "BG");
  }

  void GetOriginalBackgroundColor(int& iColorType, float fc[4]) const {
    GetOriginalColor(iColorType, fc, "BG");
  }

  WideString GetNormalCaption() const { return GetCaption("CA"); }
  WideString GetRolloverCaption() const { return GetCaption("RC"); }
  WideString GetDownCaption() const { return GetCaption("AC"); }
  CPDF_Stream* GetNormalIcon() const { return GetIcon("I"); }
  CPDF_Stream* GetRolloverIcon() const { return GetIcon("RI"); }
  CPDF_Stream* GetDownIcon() const { return GetIcon("IX"); }
  CPDF_IconFit GetIconFit() const;
  int GetTextPosition() const;

  FX_ARGB GetColor(int& iColorType, const ByteString& csEntry) const;
  float GetOriginalColor(int index, const ByteString& csEntry) const;
  void GetOriginalColor(int& iColorType,
                        float fc[4],
                        const ByteString& csEntry) const;

  WideString GetCaption(const ByteString& csEntry) const;
  CPDF_Stream* GetIcon(const ByteString& csEntry) const;

 private:
  UnownedPtr<CPDF_Dictionary> const m_pDict;
};

#endif  // CORE_FPDFDOC_CPDF_APSETTINGS_H_
