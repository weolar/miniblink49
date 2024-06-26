; Copyright (c) 2011 The Chromium Authors. All rights reserved.
; Use of this source code is governed by a BSD-style license that can be
; found in the LICENSE file.

%include "media/base/simd/media_export.asm"

  EXPORT    SYMBOL
  align     function_align

mangle(SYMBOL):
  %assign   stack_offset 0
  PROLOGUE  7, 7, 3, Y, U, V, A, ARGB, WIDTH, TABLE
  PUSH      WIDTHq
  DEFINE_ARGS Y, U, V, A, ARGB, TABLE, TEMP
  mov       TABLEq, TEMPq
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

  ; Unpack
  movq      mm0, mm1
  pxor      mm2, mm2
  punpcklbw mm0, mm2
  punpckhbw mm1, mm2

  ; Add one to our alpha values, this is a somewhat unfortunate hack; while
  ; the pack/unpack above handle saturating any negative numbers to 0, they also
  ; truncate the alpha value to 255. The math ahead wants to produce the same
  ; ARGB alpha value as the source pixel in YUVA, but this depends on the alpha
  ; value in |mm0| and |mm1| being 256, (let A be the source image alpha,
  ; 256 * A >> 8 == A, whereas 255 * A >> 8 is off by one except at 0).
  mov       TEMPq, 0x00010000
  movd      mm2, TEMPd
  psllq     mm2, 32
  paddsw    mm0, mm2
  paddsw    mm1, mm2

  ; Multiply by alpha value, then repack high bytes of words.
  movzx     TEMPd, BYTE [Aq]
  movq      mm2, [TABLEq + 6144 + 8 * TEMPq]
  pmullw    mm0, mm2
  psrlw     mm0, 8
  movzx     TEMPd, BYTE [Aq + 1]
  movq      mm2, [TABLEq + 6144 + 8 * TEMPq]
  add       Aq, 2
  pmullw    mm1, mm2
  psrlw     mm1, 8
  packuswb  mm0, mm1

  MOVQ      [ARGBq], mm0
  add       ARGBq, 8

.convertend:
  sub       dword [rsp], 2
  jns       .convertloop

  ; If number of pixels is odd then compute it.
  and       dword [rsp], 1
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

  ; Multiply ARGB by alpha value.
  pxor      mm0, mm0
  punpcklbw mm1, mm0

  ; See above note about this hack.
  mov       TEMPq, 0x00010000
  movd      mm0, TEMPd
  psllq     mm0, 32
  paddsw    mm1, mm0

  movzx     TEMPd, BYTE [Aq]
  movq      mm0, [TABLEq + 6144 + 8 * TEMPq]
  pmullw    mm1, mm0
  psrlw     mm1, 8
  packuswb  mm1, mm1

  movd      [ARGBq], mm1

.convertdone:
  POP       TABLEq
  RET
