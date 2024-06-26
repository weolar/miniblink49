; Copyright (c) 2011 The Chromium Authors. All rights reserved.
; Use of this source code is governed by a BSD-style license that can be
; found in the LICENSE file.

%include "media/base/simd/media_export.asm"

  EXPORT    SYMBOL
  align     function_align

mangle(SYMBOL):
  %assign   stack_offset 0
  PROLOGUE  6, 7, 3, Y, U, V, ARGB, WIDTH, TABLE, TEMP

  jmp       .convertend

.convertloop:
  movzx     TEMPd, BYTE [Uq]
  movq      mm0, [TABLEq + 2048 + 8 * TEMPq]
  add       Uq, 1

  movzx     TEMPd, BYTE [Vq]
  paddsw    mm0, [TABLEq + 4096 + 8 * TEMPq]
  add       Vq, 1

  movzx     TEMPd, BYTE [Yq]
  movq      mm1, [TABLEq + 8 * TEMPq]

  movzx     TEMPd, BYTE [Yq + 1]
  movq      mm2, [TABLEq + 8 * TEMPq]
  add       Yq, 2

  ; Add UV components to Y component.
  paddsw    mm1, mm0
  paddsw    mm2, mm0

  ; Down shift and then pack.
  psraw     mm1, 6
  psraw     mm2, 6
  packuswb  mm1, mm2
  MOVQ      [ARGBq], mm1
  add       ARGBq, 8

.convertend:
  sub       WIDTHq, 2
  jns       .convertloop

  ; If number of pixels is odd then compute it.
  and       WIDTHq, 1
  jz        .convertdone

  movzx     TEMPd, BYTE [Uq]
  movq      mm0, [TABLEq + 2048 + 8 * TEMPq]
  movzx     TEMPd, BYTE [Vq]
  paddsw    mm0, [TABLEq + 4096 + 8 * TEMPq]
  movzx     TEMPd, BYTE [Yq]
  movq      mm1, [TABLEq + 8 * TEMPq]
  paddsw    mm1, mm0
  psraw     mm1, 6
  packuswb  mm1, mm1
  movd      [ARGBq], mm1

.convertdone:
  RET
