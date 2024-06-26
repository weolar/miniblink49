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
; 7. Conversion lookup table

PROLOGUE  7, 7, 3, Y, R0, R1, ARGB, R2, TEMP, R3

%if gprsize == 8
%define     WORD_SIZE   QWORD
%else
%define     WORD_SIZE   DWORD
%endif

; Define register aliases.
%define     Xq                  R1q     ; Current X position
%define     COMPLq              R2q     ; Component A value
%define     COMPLd              R2d     ; Component A value
%define     U_ARG_REGq          R0q     ; U plane address argument
%define     V_ARG_REGq          R1q     ; V plane address argument
%define     SOURCE_DX_ARG_REGq  TEMPq   ; Source dx argument
%define     WIDTH_ARG_REGq      R2q     ; Width argument

%define     COMPRq              R0q     ; Component B value
%define     COMPRd              R0d     ; Component B value
%define     Uq                  R0q     ; U plane address
%define     Vq                  R0q     ; V plane address
%define     U_PLANE             WORD_SIZE [rsp + 3 * gprsize]
%define     TABLE               R3q     ; Address of the table

; Defines for stack variables.
%define     V_PLANE             WORD_SIZE [rsp + 2 * gprsize]
%define     SOURCE_DX           WORD_SIZE [rsp + gprsize]
%define     SOURCE_WIDTH        WORD_SIZE [rsp]

; Define stack usage.
  PUSH      U_ARG_REGq
  PUSH      V_ARG_REGq
  PUSH      SOURCE_DX_ARG_REGq
  imul      WIDTH_ARG_REGq, SOURCE_DX_ARG_REGq  ; source_width = width * source_dx
  PUSH      WIDTH_ARG_REGq

%macro EPILOGUE 0
  ADD       rsp, 4 * gprsize
%endmacro

  xor       Xq, Xq                       ; x = 0
  cmp       SOURCE_DX_ARG_REGq, 0x20000
  jl        .lscaleend
  mov       Xq, 0x8000                   ; x = 0.5 for 1/2 or less
  jmp       .lscaleend

.lscaleloop:
  mov       Uq, U_PLANE

; Define macros for scaling YUV components since they are reused.
%macro SCALEUV 1
  mov       TEMPq, Xq
  sar       TEMPq, 0x11
  movzx     COMPLd, BYTE [%1 + TEMPq]
  movzx     COMPRd, BYTE [%1 + TEMPq + 1]
  mov       TEMPq, Xq
  and       TEMPq, 0x1fffe
  imul      COMPRq, TEMPq
  xor       TEMPq, 0x1fffe
  imul      COMPLq, TEMPq
  add       COMPLq, COMPRq
  shr       COMPLq, 17
%endmacro
  SCALEUV   Uq                           ; Use the above macro to scale U
  movq      mm0, [TABLE + 2048 + 8 * COMPLq]

  mov       Vq, V_PLANE                  ; Read V address from stack
  SCALEUV   Vq                           ; Use the above macro to scale V
  paddsw    mm0, [TABLE + 4096 + 8 * COMPLq]

%macro SCALEY 0
  mov       TEMPq, Xq
  sar       TEMPq, 0x10
  movzx     COMPLd, BYTE [Yq + TEMPq]
  movzx     COMPRd, BYTE [Yq + TEMPq + 1]
  mov       TEMPq, Xq
  add       Xq, SOURCE_DX                 ; Add source_dx from stack
  and       TEMPq, 0xffff
  imul      COMPRq, TEMPq
  xor       TEMPq, 0xffff
  imul      COMPLq, TEMPq
  add       COMPLq, COMPRq
  shr       COMPLq, 16
%endmacro
  SCALEY                                  ; Use the above macro to scale Y1
  movq      mm1, [TABLE + 8 * COMPLq]

  cmp       Xq, SOURCE_WIDTH              ; Compare source_width from stack
  jge       .lscalelastpixel

  SCALEY                                  ; Use the above macro to sacle Y2
  movq      mm2, [TABLE + 8 * COMPLq]

  paddsw    mm1, mm0
  paddsw    mm2, mm0
  psraw     mm1, 0x6
  psraw     mm2, 0x6
  packuswb  mm1, mm2
  MOVQ      [ARGBq], mm1
  add       ARGBq, 0x8

.lscaleend:
  cmp       Xq, SOURCE_WIDTH     ; Compare source_width from stack
  jl        .lscaleloop
  EPILOGUE
  RET

.lscalelastpixel:
  paddsw    mm1, mm0
  psraw     mm1, 6
  packuswb  mm1, mm1
  movd      [ARGBq], mm1
  EPILOGUE
  RET
