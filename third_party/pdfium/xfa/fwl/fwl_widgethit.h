// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_FWL_WIDGETHIT_H_
#define XFA_FWL_FWL_WIDGETHIT_H_

enum class FWL_WidgetHit {
  Unknown = 0,
  Client,
  Left,
  Top,
  Right,
  Bottom,
  LeftTop,
  RightTop,
  LeftBottom,
  RightBottom,
  Titlebar,
  MinBox,
  MaxBox,
  CloseBox,
  HScrollBar,
  VScrollBar,
  Border,
  Edge,
  Edit,
  HyperLink,
  UpButton,
  DownButton
};

#endif  // XFA_FWL_FWL_WIDGETHIT_H_
