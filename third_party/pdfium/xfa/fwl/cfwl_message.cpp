// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fwl/cfwl_message.h"

CFWL_Message::CFWL_Message(Type type,
                           CFWL_Widget* pSrcTarget,
                           CFWL_Widget* pDstTarget)
    : m_type(type), m_pSrcTarget(pSrcTarget), m_pDstTarget(pDstTarget) {}

CFWL_Message::CFWL_Message(const CFWL_Message& that) = default;

CFWL_Message::~CFWL_Message() = default;
