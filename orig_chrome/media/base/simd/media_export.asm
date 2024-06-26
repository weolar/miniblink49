; Copyright 2013 The Chromium Authors. All rights reserved.
; Use of this source code is governed by a BSD-style license that can be
; found in the LICENSE file.
;
; A set of helper macros for controlling symbol visibility.
;

%ifndef MEDIA_BASE_SIMD_MEDIA_EXPORT_ASM_
%define MEDIA_BASE_SIMD_MEDIA_EXPORT_ASM_

; Necessary for the mangle() macro.
%include "third_party/x86inc/x86inc.asm"

;
; PRIVATE
; A flag representing the specified symbol is a private symbol. This define adds
; a hidden flag on Linux and a private_extern flag on Mac. (We can use this
; private_extern flag only on the latest yasm.)
;
%ifdef MACHO
%define PRIVATE :private_extern
%elifdef ELF
%define PRIVATE :hidden
%else
%define PRIVATE
%endif

;
; EXPORT %1
; Designates a symbol as PRIVATE if EXPORT_SYMBOLS is not set.
;
%macro EXPORT 1
%ifdef EXPORT_SYMBOLS
global mangle(%1)

; Windows needs an additional export declaration.
%ifidn __OUTPUT_FORMAT__,win32
export mangle(%1)
%elifidn __OUTPUT_FORMAT__,win64
export mangle(%1)
%endif

%else
global mangle(%1) PRIVATE
%endif
%endmacro

%endif  ; MEDIA_BASE_SIMD_MEDIA_EXPORT_ASM_
