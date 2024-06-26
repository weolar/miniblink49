// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FPDFSDK_CPDFSDK_COMMON_H_
#define FPDFSDK_CPDFSDK_COMMON_H_

// for all fields
#define FIELDFLAG_READONLY 1
#define FIELDFLAG_REQUIRED 2
// for text fields
#define FIELDFLAG_MULTILINE (1 << 12)
#define FIELDFLAG_PASSWORD (1 << 13)
#define FIELDFLAG_FILESELECT (1 << 20)
#define FIELDFLAG_DONOTSPELLCHECK (1 << 22)
#define FIELDFLAG_DONOTSCROLL (1 << 23)
#define FIELDFLAG_COMB (1 << 24)
#define FIELDFLAG_RICHTEXT (1 << 25)
// for button fileds
#define FIELDFLAG_RADIOSINUNISON (1 << 27)
// for choice fields
#define FIELDFLAG_EDIT (1 << 18)
#define FIELDFLAG_MULTISELECT (1 << 21)
#define FIELDFLAG_COMMITONSELCHANGE (1 << 26)

#endif  // FPDFSDK_CPDFSDK_COMMON_H_
