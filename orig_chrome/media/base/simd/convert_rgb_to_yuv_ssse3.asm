; Copyright (c) 2011 The Chromium Authors. All rights reserved.
; Use of this source code is governed by a BSD-style license that can be
; found in the LICENSE file.

%include "media/base/simd/media_export.asm"
%include "third_party/x86inc/x86inc.asm"

;
; This file uses SSE, SSE2, SSE3, and SSSE3, which are supported by all ATOM
; processors.
;
  SECTION_TEXT
  CPU       SSE, SSE3, SSE3, SSSE3

;
; XMM registers representing constants. We must not use these registers as
; destination operands.
; for (int i = 0; i < 16; i += 4) {
;   xmm7.b[i] = 25;  xmm7.b[i+1] = 2;   xmm7.b[i+2] = 66;  xmm7.b[i+3] = 0;
;   xmm6.b[i] = 0;   xmm6.b[i+1] = 127; xmm6.b[i+2] = 0;   xmm6.b[i+3] = 0;
;   xmm5.b[i] = 112; xmm5.b[i+1] = -74; xmm5.b[i+2] = -38; xmm5.b[i+3] = 0;
;   xmm4.b[i] = -18; xmm4.b[i+1] = -94; xmm4.b[i+2] = 112; xmm4.b[i+3] = 0;
; }
;
%define XMM_CONST_Y0    xmm7
%define XMM_CONST_Y1    xmm6
%define XMM_CONST_U     xmm5
%define XMM_CONST_V     xmm4
%define XMM_CONST_128   xmm3

;
; LOAD_XMM %1 (xmm), %2 (imm32)
; Loads an immediate value to an XMM register.
;   %1.d[0] = %1.d[1] =  %1.d[2] =  %1.d[3] = %2;
;
%macro LOAD_XMM 2
  mov       TEMPd, %2
  movd      %1, TEMPd
  pshufd    %1, %1, 00000000B
%endmacro

;
; UNPACKRGB %1 (xmm), %2 (imm8)
; Unpacks one RGB pixel in the specified XMM register.
;   for (int i = 15; i > %2; --i) %1.b[i] = %1.b[i - 1];
;   %1.b[%2] = 0;
;   for (int i = %2 - 1; i >= 0; --i) %1.b[i] = %1.b[i];
;
%macro UNPACKRGB 2
  movdqa    xmm1, %1
  psrldq    xmm1, %2
  pslldq    xmm1, %2
  pxor      %1, xmm1
  pslldq    xmm1, 1
  por       %1, xmm1
%endmacro

;
; READ_ARGB %1 (xmm), %2 (imm)
; Read the specified number of ARGB (or RGB) pixels from the source and store
; them to the destination xmm register. If the input format is RGB, we read RGB
; pixels and convert them to ARGB pixels. (For this case, the alpha values of
; the output pixels become 0.)
;
%macro READ_ARGB 2

%if PIXELSIZE == 4

  ; Read ARGB pixels from the source. (This macro assumes the input buffer may
  ; not be aligned to a 16-byte boundary.)
%if %2 == 1
  movd      %1, DWORD [ARGBq + WIDTHq * 4 * 2]
%elif %2 == 2
  movq      %1, QWORD [ARGBq + WIDTHq * 4 * 2]
%elif %2 == 4
  movdqu    %1, DQWORD [ARGBq + WIDTHq * 4 * 2]
%else
%error unsupported number of pixels.
%endif

%elif PIXELSIZE == 3

  ; Read RGB pixels from the source and convert them to ARGB pixels.
%if %2 == 1
  ; Read one RGB pixel and convert it to one ARGB pixel.
  ; Save the WIDTH register to xmm1. (This macro needs to break it.)
  MOVq      xmm1, WIDTHq

  ; Once read three bytes from the source to TEMPd, and copy it to the
  ; destination xmm register.
  lea       WIDTHq, [WIDTHq + WIDTHq * 2]
  movzx     TEMPd, BYTE [ARGBq + WIDTHq * 2 + 2]
  shl       TEMPd, 16
  mov       TEMPw, WORD [ARGBq + WIDTHq * 2]
  movd      %1, TEMPd

  ; Restore the WIDTH register.
  MOVq      WIDTHq, xmm1
%elif %2 == 2
  ; Read two RGB pixels and convert them to two ARGB pixels.
  ; Read six bytes from the source to the destination xmm register.
  mov       TEMPq, WIDTHq
  lea       TEMPq, [TEMPq + TEMPq * 2]
  movd      %1, DWORD [ARGBq + TEMPq * 2]
  pinsrw    %1, WORD [ARGBq + TEMPq * 2 + 4], 3

  ; Fill the alpha values of these RGB pixels with 0 and convert them to two
  ; ARGB pixels.
  UNPACKRGB %1, 3
%elif %2 == 4
  ; Read four RGB pixels and convert them to four ARGB pixels.
  ; Read twelve bytes from the source to the destination xmm register.
  mov       TEMPq, WIDTHq
  lea       TEMPq, [TEMPq + TEMPq * 2]
  movq      %1, QWORD [ARGBq + TEMPq * 2]
  movd      xmm1, DWORD [ARGBq + TEMPq * 2 + 8]
  shufps    %1, xmm1, 01000100B

  ; Fill the alpha values of these RGB pixels with 0 and convert them to four
  ; ARGB pixels.
  UNPACKRGB %1, 3
  UNPACKRGB %1, 4 + 3
  UNPACKRGB %1, 4 + 4 + 3
%else
%error unsupported number of pixels.
%endif

%else
%error unsupported PIXELSIZE value.
%endif

%endmacro

;
; CALC_Y %1 (xmm), %2 (xmm)
; Calculates four Y values from four ARGB pixels stored in %2.
;   %1.b[0] = ToByte((25 * B(0) + 129 * G(0) + 66 * R(0) + 128) / 256 + 16);
;   %1.b[1] = ToByte((25 * B(1) + 129 * G(1) + 66 * R(1) + 128) / 256 + 16);
;   %1.b[2] = ToByte((25 * B(2) + 129 * G(2) + 66 * R(2) + 128) / 256 + 16);
;   %1.b[3] = ToByte((25 * B(3) + 129 * G(3) + 66 * R(3) + 128) / 256 + 16);
;
%macro CALC_Y 2
  ; To avoid signed saturation, we divide this conversion formula into two
  ; formulae and store their results into two XMM registers %1 and xmm2.
  ; %1.w[0]   = 25  * %2.b[0]  + 2   * %2.b[1]  + 66  * %2.b[2]  + 0 * %2.b[3];
  ; %1.w[1]   = 25  * %2.b[4]  + 2   * %2.b[5]  + 66  * %2.b[6]  + 0 * %2.b[7];
  ; %1.w[2]   = 25  * %2.b[8]  + 2   * %2.b[9]  + 66  * %2.b[10] + 0 * %2.b[11];
  ; %1.w[3]   = 25  * %2.b[12] + 2   * %2.b[13] + 66  * %2.b[14] + 0 * %2.b[15];
  ; xmm2.w[0] = 0   * %2.b[0]  + 127 * %2.b[1]  + 0   * %2.b[2]  + 0 * %2.b[3];
  ; xmm2.w[1] = 0   * %2.b[4]  + 127 * %2.b[5]  + 0   * %2.b[6]  + 0 * %2.b[7];
  ; xmm2.w[2] = 0   * %2.b[8]  + 127 * %2.b[9]  + 0   * %2.b[10] + 0 * %2.b[11];
  ; xmm2.w[3] = 0   * %2.b[12] + 127 * %2.b[13] + 0   * %2.b[14] + 0 * %2.b[15];
  movdqa    %1, %2
  pmaddubsw %1, XMM_CONST_Y0
  phaddsw   %1, %1
  movdqa    xmm2, %2
  pmaddubsw xmm2, XMM_CONST_Y1
  phaddsw   xmm2, xmm2

  ; %1.b[0] = ToByte((%1.w[0] + xmm2.w[0] + 128) / 256 + 16);
  ; %1.b[1] = ToByte((%1.w[1] + xmm2.w[1] + 128) / 256 + 16);
  ; %1.b[2] = ToByte((%1.w[2] + xmm2.w[2] + 128) / 256 + 16);
  ; %1.b[3] = ToByte((%1.w[3] + xmm2.w[3] + 128) / 256 + 16);
  paddw     %1, xmm2
  movdqa    xmm2, XMM_CONST_128
  paddw     %1, xmm2
  psrlw     %1, 8
  psrlw     xmm2, 3
  paddw     %1, xmm2
  packuswb  %1, %1
%endmacro

;
; INIT_UV %1 (r32), %2 (reg) %3 (imm)
;
%macro INIT_UV 3

%if SUBSAMPLING == 1 && LINE == 1
%if %3 == 1 || %3 == 2
  movzx     %1, BYTE [%2 + WIDTHq]
%elif %3 == 4
  movzx     %1, WORD [%2 + WIDTHq]
%else
%error unsupported number of pixels.
%endif
%endif

%endmacro

;
; CALC_UV %1 (xmm), %2 (xmm), %3 (xmm), %4 (r32)
; Calculates two U (or V) values from four ARGB pixels stored in %2.
; if %3 == XMM_CONST_U
; if (SUBSAMPLING) {
;   %1.b[0] = ToByte((112 * B(0) - 74 * G(0) - 38 * R(0) + 128) / 256 + 128);
;   %1.b[0] = ToByte((112 * B(0) - 74 * G(0) - 38 * R(0) + 128) / 256 + 128);
;   %1.b[1] = ToByte((112 * B(2) - 74 * G(2) - 38 * R(2) + 128) / 256 + 128);
;   %1.b[1] = ToByte((112 * B(2) - 74 * G(2) - 38 * R(2) + 128) / 256 + 128);
; } else {
;   %1.b[0] = ToByte((112 * B(0) - 74 * G(0) - 38 * R(0) + 128) / 256 + 128);
;   %1.b[1] = ToByte((112 * B(2) - 74 * G(2) - 38 * R(2) + 128) / 256 + 128);
; }
; if %3 == XMM_CONST_V
;   %1.b[0] = ToByte((-18 * B(0) - 94 * G(0) + 112 * R(0) + 128) / 256 + 128);
;   %1.b[1] = ToByte((-18 * B(2) - 94 * G(2) + 112 * R(2) + 128) / 256 + 128);
;
%macro CALC_UV 4
  ; for (int i = 0; i < 4; ++i) {
  ;   %1.w[i] = 0;
  ;   for (int j = 0; j < 4; ++j)
  ;     %1.w[i] += %3.b[i * 4 + j] + %2.b[i * 4 + j];
  ; }
  movdqa    %1, %2
  pmaddubsw %1, %3
  phaddsw   %1, %1

%if SUBSAMPLING == 1
  ; %1.w[0] = (%1.w[0] + %1.w[1] + 1) / 2;
  ; %1.w[1] = (%1.w[1] + %1.w[0] + 1) / 2;
  ; %1.w[2] = (%1.w[2] + %1.w[3] + 1) / 2;
  ; %1.w[3] = (%1.w[3] + %1.w[2] + 1) / 2;
  pshuflw   xmm2, %1, 10110001B
  pavgw     %1, xmm2
%endif

  ; %1.b[0] = ToByte((%1.w[0] + 128) / 256 + 128);
  ; %1.b[1] = ToByte((%1.w[2] + 128) / 256 + 128);
  pshuflw   %1, %1, 10001000B
  paddw     %1, XMM_CONST_128
  psraw     %1, 8
  paddw     %1, XMM_CONST_128
  packuswb  %1, %1

%if SUBSAMPLING == 1 && LINE == 1
  ; %1.b[0] = (%1.b[0] + %3.b[0] + 1) / 2;
  ; %1.b[1] = (%1.b[1] + %3.b[1] + 1) / 2;
  movd      xmm2, %4
  pavgb     %1, xmm2
%endif
%endmacro

;
; extern "C" void ConvertARGBToYUVRow_SSSE3(const uint8* argb,
;                                           uint8* y,
;                                           uint8* u,
;                                           uint8* v,
;                                           ptrdiff_t width);
;
%define SYMBOL          ConvertARGBToYUVRow_SSSE3
%define PIXELSIZE       4
%define SUBSAMPLING     0
%define LINE            0
%include "convert_rgb_to_yuv_ssse3.inc"

;
; extern "C" void ConvertRGBToYUVRow_SSSE3(const uint8* rgb,
;                                          uint8* y,
;                                          uint8* u,
;                                          uint8* v,
;                                          ptrdiff_t width);
;
%define SYMBOL          ConvertRGBToYUVRow_SSSE3
%define PIXELSIZE       3
%define SUBSAMPLING     0
%define LINE            0
%include "convert_rgb_to_yuv_ssse3.inc"

;
; extern "C" void ConvertARGBToYUVEven_SSSE3(const uint8* argb,
;                                            uint8* y,
;                                            uint8* u,
;                                            uint8* v,
;                                            ptrdiff_t width);
;
%define SYMBOL          ConvertARGBToYUVEven_SSSE3
%define PIXELSIZE       4
%define SUBSAMPLING     1
%define LINE            0
%include "convert_rgb_to_yuv_ssse3.inc"

;
; extern "C" void ConvertARGBToYUVOdd_SSSE3(const uint8* argb,
;                                           uint8* y,
;                                           uint8* u,
;                                           uint8* v,
;                                           ptrdiff_t width);
;
%define SYMBOL          ConvertARGBToYUVOdd_SSSE3
%define PIXELSIZE       4
%define SUBSAMPLING     1
%define LINE            1
%include "convert_rgb_to_yuv_ssse3.inc"

;
; extern "C" void ConvertRGBToYUVEven_SSSE3(const uint8* rgb,
;                                           uint8* y,
;                                           uint8* u,
;                                           uint8* v,
;                                           ptrdiff_t width);
;
%define SYMBOL          ConvertRGBToYUVEven_SSSE3
%define PIXELSIZE       3
%define SUBSAMPLING     1
%define LINE            0
%include "convert_rgb_to_yuv_ssse3.inc"

;
; extern "C" void ConvertRGBToYUVOdd_SSSE3(const uint8* rgb,
;                                          uint8* y,
;                                          uint8* u,
;                                          uint8* v,
;                                          ptrdiff_t width);
;
%define SYMBOL          ConvertRGBToYUVOdd_SSSE3
%define PIXELSIZE       3
%define SUBSAMPLING     1
%define LINE            1
%include "convert_rgb_to_yuv_ssse3.inc"
