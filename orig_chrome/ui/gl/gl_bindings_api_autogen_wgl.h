// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// This file is auto-generated from
// ui/gl/generate_bindings.py
// It's formatted by clang-format using chromium coding style:
//    clang-format -i -style=chromium filename
// DO NOT EDIT!

BOOL wglChoosePixelFormatARBFn(HDC dc,
    const int* int_attrib_list,
    const float* float_attrib_list,
    UINT max_formats,
    int* formats,
    UINT* num_formats) override;
BOOL wglCopyContextFn(HGLRC hglrcSrc, HGLRC hglrcDst, UINT mask) override;
HGLRC wglCreateContextFn(HDC hdc) override;
HGLRC wglCreateLayerContextFn(HDC hdc, int iLayerPlane) override;
HPBUFFERARB wglCreatePbufferARBFn(HDC hDC,
    int iPixelFormat,
    int iWidth,
    int iHeight,
    const int* piAttribList) override;
BOOL wglDeleteContextFn(HGLRC hglrc) override;
BOOL wglDestroyPbufferARBFn(HPBUFFERARB hPbuffer) override;
HGLRC wglGetCurrentContextFn() override;
HDC wglGetCurrentDCFn() override;
const char* wglGetExtensionsStringARBFn(HDC hDC) override;
const char* wglGetExtensionsStringEXTFn() override;
HDC wglGetPbufferDCARBFn(HPBUFFERARB hPbuffer) override;
BOOL wglMakeCurrentFn(HDC hdc, HGLRC hglrc) override;
BOOL wglQueryPbufferARBFn(HPBUFFERARB hPbuffer,
    int iAttribute,
    int* piValue) override;
int wglReleasePbufferDCARBFn(HPBUFFERARB hPbuffer, HDC hDC) override;
BOOL wglShareListsFn(HGLRC hglrc1, HGLRC hglrc2) override;
BOOL wglSwapIntervalEXTFn(int interval) override;
BOOL wglSwapLayerBuffersFn(HDC hdc, UINT fuPlanes) override;
