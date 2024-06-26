; Copyright (c) 2011 The Chromium Authors. All rights reserved.
; Use of this source code is governed by a BSD-style license that can be
; found in the LICENSE file.

;
; void SYMBOL(const uint8* argb, uint8* y, uint8* u, uint8* v, int width);
;
; The main code that converts RGB pixels to YUV pixels. This function roughly
; consists of three parts: converting one ARGB pixel to YUV pixels, converting
; two ARGB pixels to YUV pixels, and converting four ARGB pixels to YUV pixels.
; To write the structure of this function in C, it becomes the snippet listed
; below.
;
;   if (width & 1) {
;     --width;
;     // Convert one ARGB pixel to one Y pixel, one U pixel, and one V pixel.
;   }
;
;   if (width & 2) {
;     width -= 2;
;     // Convert two ARGB pixels to two Y pixels, one U pixel, and one V pixel.
;   }
;
;   while (width) {
;     width -= 4;
;     // Convert four ARGB pixels to four Y pixels, two U pixels, and two V
;     // pixels.
;   }
;
  EXPORT    SYMBOL
  align     function_align

mangle(SYMBOL):
  %assign stack_offset 0
  PROLOGUE 5, 6, 8, ARGB, Y, U, V, WIDTH, TEMP

  ; Initialize constants used in this function. (We use immediates to avoid
  ; dependency onto GOT.)
  LOAD_XMM  XMM_CONST_Y0, 0x00420219
  LOAD_XMM  XMM_CONST_Y1, 0x00007F00
  LOAD_XMM  XMM_CONST_U, 0x00DAB670
  LOAD_XMM  XMM_CONST_V, 0x0070A2EE
  LOAD_XMM  XMM_CONST_128, 0x00800080

.convert_one_pixel:
  ; Divide the input width by two so it represents the offsets for u[] and v[].
  ; When the width is odd, We read the rightmost ARGB pixel and convert its
  ; colorspace to YUV. This code stores one Y pixel, one U pixel, and one V
  ; pixel.
  sar       WIDTHq, 1
  jnc       .convert_two_pixels

  ; Read one ARGB (or RGB) pixel.
  READ_ARGB xmm0, 1

  ; Calculate y[0] from one RGB pixel read above.
  CALC_Y    xmm1, xmm0
  movd      TEMPd, xmm1
  mov       BYTE [Yq + WIDTHq * 2], TEMPb

  ; Calculate u[0] from one RGB pixel read above. If this is an odd line, the
  ; output pixel contains the U value calculated in the previous call. We also
  ; read this pixel and calculate their average.
  INIT_UV   TEMPd, Uq, 4
  CALC_UV   xmm1, xmm0, XMM_CONST_U, TEMPd
  movd      TEMPd, xmm1
  mov       BYTE [Uq + WIDTHq], TEMPb

  ; Calculate v[0] from one RGB pixel. Same as u[0], we read the result of the
  ; previous call and get their average.
  INIT_UV   TEMPd, Uq, 4
  CALC_UV   xmm1, xmm0, XMM_CONST_V, TEMPd
  movd      TEMPd, xmm1
  mov       BYTE [Vq + WIDTHq], TEMPb

.convert_two_pixels:
  ; If the input width is not a multiple of four, read the rightmost two ARGB
  ; pixels and convert their colorspace to YUV. This code stores two Y pixels,
  ; one U pixel, and one V pixel.
  test      WIDTHb, 2 / 2
  jz        .convert_four_pixels
  sub       WIDTHb, 2 / 2

  ; Read two ARGB (or RGB) pixels.
  READ_ARGB xmm0, 2

  ; Calculate r[0] and r[1] from two RGB pixels read above.
  CALC_Y    xmm1, xmm0
  movd      TEMPd, xmm1
  mov       WORD [Yq + WIDTHq * 2], TEMPw

  ; Skip calculating u and v if the output buffer is NULL.
  test      Uq, Uq
  jz        .convert_four_pixels

  ; Calculate u[0] from two RGB pixels read above. (For details, read the above
  ; comment in .convert_one_pixel).
  INIT_UV   TEMPd, Uq, 2
  CALC_UV   xmm1, xmm0, XMM_CONST_U, TEMPd
  movd      TEMPd, xmm1
  mov       BYTE [Uq + WIDTHq], TEMPb

  ; Calculate v[0] from two RGB pixels read above.
  INIT_UV   TEMPd, Vq, 2
  CALC_UV   xmm1, xmm0, XMM_CONST_V, TEMPd
  movd      TEMPd, xmm1
  mov       BYTE [Vq + WIDTHq], TEMPb

.convert_four_pixels:
  ; Read four ARGB pixels and convert their colorspace to YUV. This code stores
  ; four Y pixels, two U pixels, and two V pixels.
  test      WIDTHq, WIDTHq
  jz        .convert_finish

%if PIXELSIZE == 4
  ; Check if the input buffer is aligned to a 16-byte boundary and use movdqa
  ; for reading the ARGB pixels.
  test      ARGBw, 15
  jnz       .convert_four_pixels_unaligned

.convert_four_pixels_aligned:
  sub       WIDTHq, 4 / 2

  ; Read four ARGB pixels. (We can use movdqa here since we have checked if the
  ; source address is aligned.)
  movdqa    xmm0, DQWORD [ARGBq + WIDTHq * 4 * 2]

  ; Calculate y[0], y[1], y[2],and, y[3] from the input ARGB pixels.
  CALC_Y    xmm1, xmm0
  movd      DWORD [Yq + WIDTHq * 2], xmm1

%if SUBSAMPLING == 0
  ; Skip calculating u and v if the output buffer is NULL, which means we are
  ; converting an odd line. (When we enable subsampling, these buffers must
  ; contain the u and v values for the previous call, i.e. these variables must
  ; not be NULL.)
  test      Uq, Uq
  jz        .convert_four_pixels_aligned_next
%endif

  ; Calculate u[0] and u[1] from four ARGB pixels read above.
  INIT_UV   TEMPd, Uq, 4
  CALC_UV   xmm1, xmm0, XMM_CONST_U, TEMPd
  movd      TEMPd, xmm1
  mov       WORD [Uq + WIDTHq], TEMPw

  ; Calculate v[0] and v[1] from four ARGB pixels read above.
  INIT_UV   TEMPd, Vq, 4
  CALC_UV   xmm1, xmm0, XMM_CONST_V, TEMPd
  movd      TEMPd, xmm1
  mov       WORD [Vq + WIDTHq], TEMPw

%if SUBSAMPLING == 0
.convert_four_pixels_aligned_next:
%endif

  test      WIDTHq, WIDTHq
  jnz       .convert_four_pixels_aligned

  jmp       .convert_finish
%endif

.convert_four_pixels_unaligned:
  sub       WIDTHq, 4 / 2

  ; Read four ARGB (or RGB) pixels.
  READ_ARGB xmm0, 4

  ; Calculate y[0], y[1], y[2],and, y[3] from the input ARGB pixels.
  CALC_Y    xmm1, xmm0
  movd      DWORD [Yq + WIDTHq * 2], xmm1

%if SUBSAMPLING == 0
  ; Skip calculating u and v if the output buffer is NULL.
  test      Uq, Uq
  jz        .convert_four_pixels_unaligned_next
%endif

  ; Calculate u[0] and u[1] from the input ARGB pixels.
  INIT_UV   TEMPd, Uq, 4
  CALC_UV   xmm1, xmm0, XMM_CONST_U, TEMPd
  movd      TEMPd, xmm1
  mov       WORD [Uq + WIDTHq], TEMPw

  ; Calculate v[0] and v[1] from the input ARGB pixels.
  INIT_UV   TEMPd, Vq, 4
  CALC_UV   xmm1, xmm0, XMM_CONST_V, TEMPd
  movd      TEMPd, xmm1
  mov       WORD [Vq + WIDTHq], TEMPw

%if SUBSAMPLING == 0
.convert_four_pixels_unaligned_next:
%endif

  test      WIDTHq, WIDTHq
  jnz       .convert_four_pixels_unaligned

.convert_finish:
  ; Just exit this function since this is a void function.
  RET
