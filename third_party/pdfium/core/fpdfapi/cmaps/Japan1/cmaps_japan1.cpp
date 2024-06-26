// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/cmaps/Japan1/cmaps_japan1.h"

const FXCMAP_CMap g_FXCMAP_Japan1_cmaps[] = {
    {"83pv-RKSJ-H", g_FXCMAP_83pv_RKSJ_H_1, nullptr, 222, 0, FXCMAP_CMap::Range,
     0},
    {"90ms-RKSJ-H", g_FXCMAP_90ms_RKSJ_H_2, nullptr, 171, 0, FXCMAP_CMap::Range,
     0},
    {"90ms-RKSJ-V", g_FXCMAP_90ms_RKSJ_V_2, nullptr, 78, 0, FXCMAP_CMap::Range,
     -1},
    {"90msp-RKSJ-H", g_FXCMAP_90msp_RKSJ_H_2, nullptr, 170, 0,
     FXCMAP_CMap::Range, -2},
    {"90msp-RKSJ-V", g_FXCMAP_90msp_RKSJ_V_2, nullptr, 78, 0,
     FXCMAP_CMap::Range, -1},
    {"90pv-RKSJ-H", g_FXCMAP_90pv_RKSJ_H_1, nullptr, 263, 0, FXCMAP_CMap::Range,
     0},
    {"Add-RKSJ-H", g_FXCMAP_Add_RKSJ_H_1, nullptr, 635, 0, FXCMAP_CMap::Range,
     0},
    {"Add-RKSJ-V", g_FXCMAP_Add_RKSJ_V_1, nullptr, 57, 0, FXCMAP_CMap::Range,
     -1},
    {"EUC-H", g_FXCMAP_EUC_H_1, nullptr, 120, 0, FXCMAP_CMap::Range, 0},
    {"EUC-V", g_FXCMAP_EUC_V_1, nullptr, 27, 0, FXCMAP_CMap::Range, -1},
    {"Ext-RKSJ-H", g_FXCMAP_Ext_RKSJ_H_2, nullptr, 665, 0, FXCMAP_CMap::Range,
     -4},
    {"Ext-RKSJ-V", g_FXCMAP_Ext_RKSJ_V_2, nullptr, 39, 0, FXCMAP_CMap::Range,
     -1},
    {"H", g_FXCMAP_H_1, nullptr, 118, 0, FXCMAP_CMap::Range, 0},
    {"V", g_FXCMAP_V_1, nullptr, 27, 0, FXCMAP_CMap::Range, -1},
    {"UniJIS-UCS2-H", g_FXCMAP_UniJIS_UCS2_H_4, nullptr, 9772, 0,
     FXCMAP_CMap::Single, 0},
    {"UniJIS-UCS2-V", g_FXCMAP_UniJIS_UCS2_V_4, nullptr, 251, 0,
     FXCMAP_CMap::Single, -1},
    {"UniJIS-UCS2-HW-H", g_FXCMAP_UniJIS_UCS2_HW_H_4, nullptr, 4, 0,
     FXCMAP_CMap::Range, -2},
    {"UniJIS-UCS2-HW-V", g_FXCMAP_UniJIS_UCS2_HW_V_4, nullptr, 199, 0,
     FXCMAP_CMap::Range, -1},
    {"UniJIS-UTF16-H", g_FXCMAP_UniJIS_UCS2_H_4, nullptr, 9772, 0,
     FXCMAP_CMap::Single, 0},
    {"UniJIS-UTF16-V", g_FXCMAP_UniJIS_UCS2_V_4, nullptr, 251, 0,
     FXCMAP_CMap::Single, -1},
};

const size_t g_FXCMAP_Japan1_cmaps_size = FX_ArraySize(g_FXCMAP_Japan1_cmaps);
