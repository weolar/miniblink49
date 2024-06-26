// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/xfa/cjx_command.h"

#include "xfa/fxfa/parser/cxfa_command.h"

CJX_Command::CJX_Command(CXFA_Command* node) : CJX_Node(node) {}

CJX_Command::~CJX_Command() = default;
