; Copyright (c) 2011 The Chromium Authors. All rights reserved.
; Use of this source code is governed by a BSD-style license that can be
; found in the LICENSE file.

%include "media/base/simd/media_export.asm"

  EXPORT    SYMBOL
  align     function_align

mangle(SYMBOL):
  %assign   stack_offset 0

; Parameters are in the following order:
; 1. Y plane
; 2. U plane
; 3. V plane
; 4. ARGB frame
; 5. Width
; 6. Source dx
; 7. Lookup table address

PROLOGUE  7, 7, 3, Y, U, V, ARGB, R1, R2, TEMP

%ifdef ARCH_X86_64
%define     WORD_SIZE   QWORD
%else
%define     WORD_SIZE   DWORD
%endif

  PUSH      R1q  ; Width
  PUSH      R2q  ; Source dx

%define     SOURCE_DX   WORD_SIZE [rsp]

  mov       R1q, TEMPq

%define     WIDTH       WORD_SIZE [rsp + gprsize]
%define     TABLE       R1q
%define     Xq          R2q

  ; Set Xq index to 0.
  xor       Xq, Xq
  jmp       .scaleend

.scaleloop:
  mov       TEMPq, Xq
  sar       TEMPq, 17
  movzx     TEMPd, BYTE [Uq + TEMPq]
  movq      mm0, [TABLE + 2048 + 8 * TEMPq]
  mov       TEMPq, Xq
  sar       TEMPq, 17
  movzx     TEMPd, BYTE [Vq + TEMPq]
  paddsw    mm0, [TABLE + 4096 + 8 * TEMPq]
  mov       TEMPq, Xq
  add       Xq, SOURCE_DX
  sar       TEMPq, 16
  movzx     TEMPd, BYTE [Yq + TEMPq]
  movq      mm1, [TABLE + 8 * TEMPq]
  mov       TEMPq, Xq
  add       Xq, SOURCE_DX
  sar       TEMPq, 16
  movzx     TEMPd, BYTE [Yq + TEMPq]
  movq      mm2, [TABLE + 8 * TEMPq]
  paddsw    mm1, mm0
  paddsw    mm2, mm0
  psraw     mm1, 6
  psraw     mm2, 6
  packuswb  mm1, mm2
  MOVQ      QWORD [ARGBq], mm1
  add       ARGBq, 8

.scaleend:
  sub       WIDTH, 2
  jns       .scaleloop

  and       WIDTH, 1             ; odd number of pixels?
  jz        .scaledone

  mov       TEMPq, Xq
  sar       TEMPq, 17
  movzx     TEMPd, BYTE [Uq + TEMPq]
  movq      mm0, [TABLE + 2048 + 8 * TEMPq]
  mov       TEMPq, Xq
  sar       TEMPq, 17
  movzx     TEMPd, BYTE [Vq + TEMPq]
  paddsw    mm0, [TABLE + 4096 + 8 * TEMPq]
  mov       TEMPq, Xq
  sar       TEMPq, 16
  movzx     TEMPd, BYTE [Yq + TEMPq]
  movq      mm1, [TABLE + 8 * TEMPq]
  paddsw    mm1, mm0
  psraw     mm1, 6
  packuswb  mm1, mm1
  movd      DWORD [ARGBq], mm1

.scaledone:
  ADD       rsp, 2 * gprsize
  RET
