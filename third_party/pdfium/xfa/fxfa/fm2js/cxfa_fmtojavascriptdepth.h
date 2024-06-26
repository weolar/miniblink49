// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef XFA_FXFA_FM2JS_CXFA_FMTOJAVASCRIPTDEPTH_H_
#define XFA_FXFA_FM2JS_CXFA_FMTOJAVASCRIPTDEPTH_H_

class CXFA_FMToJavaScriptDepth {
 public:
  CXFA_FMToJavaScriptDepth() { depth_++; }
  ~CXFA_FMToJavaScriptDepth() { depth_--; }

  bool IsWithinMaxDepth() const { return depth_ <= kMaxDepth; }

  static void Reset();

 private:
  // Arbitarily picked by looking at how deep a translation got before hitting
  // the getting fuzzer memory limits. Should be larger then |kMaxParseDepth| in
  // cxfa_fmparser.cpp.
  const unsigned long kMaxDepth = 5000;

  static unsigned long depth_;
};

#endif  // XFA_FXFA_FM2JS_CXFA_FMTOJAVASCRIPTDEPTH_H_
