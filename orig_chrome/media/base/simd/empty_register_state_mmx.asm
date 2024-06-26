; Copyright (c) 2013 The Chromium Authors. All rights reserved.
; Use of this source code is governed by a BSD-style license that can be
; found in the LICENSE file.

%include "media/base/simd/media_export.asm"
%include "third_party/x86inc/x86inc.asm"

;
; This file uses MMX instructions as an alternative to _mm_empty() which
; is not supported in Visual Studio 2010 on x64.
; TODO(wolenetz): Use MMX intrinsics when compiling win64 with Visual
; Studio 2012? http://crbug.com/173450
;
  SECTION_TEXT
  CPU       MMX

%define SYMBOL EmptyRegisterState_MMX
  EXPORT    SYMBOL
  align     function_align

mangle(SYMBOL):
  emms
  ret

