// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/cmaps/Korea1/cmaps_korea1.h"

const FXCMAP_CMap g_FXCMAP_Korea1_cmaps[] = {
    {"KSC-EUC-H", g_FXCMAP_KSC_EUC_H_0, nullptr, 467, 0, FXCMAP_CMap::Range, 0},
    {"KSC-EUC-V", g_FXCMAP_KSC_EUC_V_0, nullptr, 16, 0, FXCMAP_CMap::Range, -1},
    {"KSCms-UHC-H", g_FXCMAP_KSCms_UHC_H_1, nullptr, 675, 0, FXCMAP_CMap::Range,
     -2},
    {"KSCms-UHC-V", g_FXCMAP_KSCms_UHC_V_1, nullptr, 16, 0, FXCMAP_CMap::Range,
     -1},
    {"KSCms-UHC-HW-H", g_FXCMAP_KSCms_UHC_HW_H_1, nullptr, 675, 0,
     FXCMAP_CMap::Range, 0},
    {"KSCms-UHC-HW-V", g_FXCMAP_KSCms_UHC_HW_V_1, nullptr, 16, 0,
     FXCMAP_CMap::Range, -1},
    {"KSCpc-EUC-H", g_FXCMAP_KSCpc_EUC_H_0, nullptr, 509, 0, FXCMAP_CMap::Range,
     -6},
    {"UniKS-UCS2-H", g_FXCMAP_UniKS_UCS2_H_1, nullptr, 8394, 0,
     FXCMAP_CMap::Range, 0},
    {"UniKS-UCS2-V", g_FXCMAP_UniKS_UCS2_V_1, nullptr, 18, 0,
     FXCMAP_CMap::Range, -1},
    {"UniKS-UTF16-H", g_FXCMAP_UniKS_UTF16_H_0, nullptr, 158, 0,
     FXCMAP_CMap::Single, -2},
    {"UniKS-UTF16-V", g_FXCMAP_UniKS_UCS2_V_1, nullptr, 18, 0,
     FXCMAP_CMap::Range, -1},
};

const size_t g_FXCMAP_Korea1_cmaps_size = FX_ArraySize(g_FXCMAP_Korea1_cmaps);
