// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FPDFSDK_PWL_CPWL_APPSTREAM_H_
#define FPDFSDK_PWL_CPWL_APPSTREAM_H_

#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/unowned_ptr.h"
#include "third_party/base/optional.h"

class CPDFSDK_Widget;
class CPDF_Dictionary;
class CPDF_Stream;

class CPWL_AppStream {
 public:
  CPWL_AppStream(CPDFSDK_Widget* widget, CPDF_Dictionary* dict);
  ~CPWL_AppStream();

  void SetAsPushButton();
  void SetAsCheckBox();
  void SetAsRadioButton();
  void SetAsComboBox(Optional<WideString> sValue);
  void SetAsListBox();
  void SetAsTextField(Optional<WideString> sValue);

 private:
  void AddImage(const ByteString& sAPType, CPDF_Stream* pImage);
  void Write(const ByteString& sAPType,
             const ByteString& sContents,
             const ByteString& sAPState);
  void Remove(const ByteString& sAPType);

  ByteString GetBackgroundAppStream() const;
  ByteString GetBorderAppStream() const;

  UnownedPtr<CPDFSDK_Widget> const widget_;
  UnownedPtr<CPDF_Dictionary> const dict_;
};

#endif  // FPDFSDK_PWL_CPWL_APPSTREAM_H_
