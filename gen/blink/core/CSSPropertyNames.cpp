/* C++ code produced by gperf version 3.0.3 */
/* Command-line: /usr/bin/gperf --key-positions='*' -P -n -m 50 -D  */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gnu-gperf@gnu.org>."
#endif


// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "CSSPropertyNames.h"
#include "core/css/HashTools.h"
#include <string.h>

#include "wtf/ASCIICType.h"
#include "wtf/text/AtomicString.h"
#include "wtf/text/WTFString.h"

namespace blink {
static const char propertyNameStringsPool[] = {
    "color\0"
    "direction\0"
    "font-family\0"
    "font-kerning\0"
    "font-size\0"
    "font-size-adjust\0"
    "font-stretch\0"
    "font-style\0"
    "font-variant\0"
    "font-variant-ligatures\0"
    "font-weight\0"
    "font-feature-settings\0"
    "-webkit-font-feature-settings\0"
    "-webkit-font-smoothing\0"
    "-webkit-locale\0"
    "-webkit-text-orientation\0"
    "-webkit-writing-mode\0"
    "text-rendering\0"
    "zoom\0"
    "align-content\0"
    "align-items\0"
    "alignment-baseline\0"
    "align-self\0"
    "animation-delay\0"
    "animation-direction\0"
    "animation-duration\0"
    "animation-fill-mode\0"
    "animation-iteration-count\0"
    "animation-name\0"
    "animation-play-state\0"
    "animation-timing-function\0"
    "backdrop-filter\0"
    "backface-visibility\0"
    "background-attachment\0"
    "background-blend-mode\0"
    "background-clip\0"
    "background-color\0"
    "background-image\0"
    "background-origin\0"
    "background-position-x\0"
    "background-position-y\0"
    "background-repeat-x\0"
    "background-repeat-y\0"
    "background-size\0"
    "baseline-shift\0"
    "border-bottom-color\0"
    "border-bottom-left-radius\0"
    "border-bottom-right-radius\0"
    "border-bottom-style\0"
    "border-bottom-width\0"
    "border-collapse\0"
    "border-image-outset\0"
    "border-image-repeat\0"
    "border-image-slice\0"
    "border-image-source\0"
    "border-image-width\0"
    "border-left-color\0"
    "border-left-style\0"
    "border-left-width\0"
    "border-right-color\0"
    "border-right-style\0"
    "border-right-width\0"
    "border-top-color\0"
    "border-top-left-radius\0"
    "border-top-right-radius\0"
    "border-top-style\0"
    "border-top-width\0"
    "bottom\0"
    "box-shadow\0"
    "box-sizing\0"
    "buffered-rendering\0"
    "caption-side\0"
    "clear\0"
    "clip\0"
    "clip-path\0"
    "clip-rule\0"
    "color-interpolation\0"
    "color-interpolation-filters\0"
    "color-rendering\0"
    "column-fill\0"
    "content\0"
    "counter-increment\0"
    "counter-reset\0"
    "cursor\0"
    "cx\0"
    "cy\0"
    "display\0"
    "dominant-baseline\0"
    "empty-cells\0"
    "fill\0"
    "fill-opacity\0"
    "fill-rule\0"
    "filter\0"
    "flex-basis\0"
    "flex-direction\0"
    "flex-grow\0"
    "flex-shrink\0"
    "flex-wrap\0"
    "float\0"
    "flood-color\0"
    "flood-opacity\0"
    "ime-mode\0"
    "glyph-orientation-horizontal\0"
    "glyph-orientation-vertical\0"
    "grid-auto-columns\0"
    "grid-auto-flow\0"
    "grid-auto-rows\0"
    "grid-column-end\0"
    "grid-column-start\0"
    "grid-column-gap\0"
    "grid-row-end\0"
    "grid-row-start\0"
    "grid-row-gap\0"
    "grid-template-areas\0"
    "grid-template-columns\0"
    "grid-template-rows\0"
    "height\0"
    "image-rendering\0"
    "image-orientation\0"
    "isolation\0"
    "justify-content\0"
    "justify-items\0"
    "justify-self\0"
    "left\0"
    "letter-spacing\0"
    "lighting-color\0"
    "line-height\0"
    "list-style-image\0"
    "list-style-position\0"
    "list-style-type\0"
    "margin-bottom\0"
    "margin-left\0"
    "margin-right\0"
    "margin-top\0"
    "marker-end\0"
    "marker-mid\0"
    "marker-start\0"
    "mask\0"
    "mask-source-type\0"
    "mask-type\0"
    "max-height\0"
    "max-width\0"
    "min-height\0"
    "min-width\0"
    "mix-blend-mode\0"
    "motion-offset\0"
    "motion-path\0"
    "motion-rotation\0"
    "object-fit\0"
    "object-position\0"
    "opacity\0"
    "order\0"
    "orphans\0"
    "outline-color\0"
    "outline-offset\0"
    "outline-style\0"
    "outline-width\0"
    "overflow-wrap\0"
    "overflow-x\0"
    "overflow-y\0"
    "padding-bottom\0"
    "padding-left\0"
    "padding-right\0"
    "padding-top\0"
    "page-break-after\0"
    "page-break-before\0"
    "page-break-inside\0"
    "paint-order\0"
    "perspective\0"
    "perspective-origin\0"
    "pointer-events\0"
    "position\0"
    "quotes\0"
    "resize\0"
    "right\0"
    "r\0"
    "rx\0"
    "ry\0"
    "scroll-behavior\0"
    "scroll-blocks-on\0"
    "scroll-snap-type\0"
    "scroll-snap-points-x\0"
    "scroll-snap-points-y\0"
    "scroll-snap-destination\0"
    "scroll-snap-coordinate\0"
    "shape-image-threshold\0"
    "shape-margin\0"
    "shape-outside\0"
    "shape-rendering\0"
    "size\0"
    "speak\0"
    "stop-color\0"
    "stop-opacity\0"
    "stroke\0"
    "stroke-dasharray\0"
    "stroke-dashoffset\0"
    "stroke-linecap\0"
    "stroke-linejoin\0"
    "stroke-miterlimit\0"
    "stroke-opacity\0"
    "stroke-width\0"
    "table-layout\0"
    "tab-size\0"
    "text-align\0"
    "text-align-last\0"
    "text-anchor\0"
    "text-decoration\0"
    "text-decoration-color\0"
    "text-decoration-line\0"
    "text-decoration-style\0"
    "text-indent\0"
    "text-justify\0"
    "text-overflow\0"
    "text-shadow\0"
    "text-transform\0"
    "text-underline-position\0"
    "top\0"
    "touch-action\0"
    "transform\0"
    "transform-origin\0"
    "transform-style\0"
    "translate\0"
    "rotate\0"
    "scale\0"
    "transition-delay\0"
    "transition-duration\0"
    "transition-property\0"
    "transition-timing-function\0"
    "unicode-bidi\0"
    "vector-effect\0"
    "vertical-align\0"
    "visibility\0"
    "x\0"
    "y\0"
    "-webkit-appearance\0"
    "-webkit-app-region\0"
    "-webkit-background-clip\0"
    "-webkit-background-composite\0"
    "-webkit-background-origin\0"
    "-webkit-border-horizontal-spacing\0"
    "-webkit-border-image\0"
    "-webkit-border-vertical-spacing\0"
    "-webkit-box-align\0"
    "-webkit-box-decoration-break\0"
    "-webkit-box-direction\0"
    "-webkit-box-flex\0"
    "-webkit-box-flex-group\0"
    "-webkit-box-lines\0"
    "-webkit-box-ordinal-group\0"
    "-webkit-box-orient\0"
    "-webkit-box-pack\0"
    "-webkit-box-reflect\0"
    "-webkit-clip-path\0"
    "-webkit-column-break-after\0"
    "-webkit-column-break-before\0"
    "-webkit-column-break-inside\0"
    "-webkit-column-count\0"
    "-webkit-column-gap\0"
    "-webkit-column-rule-color\0"
    "-webkit-column-rule-style\0"
    "-webkit-column-rule-width\0"
    "-webkit-column-span\0"
    "-webkit-column-width\0"
    "-webkit-filter\0"
    "-webkit-highlight\0"
    "-webkit-hyphenate-character\0"
    "-webkit-line-box-contain\0"
    "-webkit-line-break\0"
    "-webkit-line-clamp\0"
    "-webkit-margin-after-collapse\0"
    "-webkit-margin-before-collapse\0"
    "-webkit-margin-bottom-collapse\0"
    "-webkit-margin-top-collapse\0"
    "-webkit-mask-box-image-outset\0"
    "-webkit-mask-box-image-repeat\0"
    "-webkit-mask-box-image-slice\0"
    "-webkit-mask-box-image-source\0"
    "-webkit-mask-box-image-width\0"
    "-webkit-mask-clip\0"
    "-webkit-mask-composite\0"
    "-webkit-mask-image\0"
    "-webkit-mask-origin\0"
    "-webkit-mask-position-x\0"
    "-webkit-mask-position-y\0"
    "-webkit-mask-repeat-x\0"
    "-webkit-mask-repeat-y\0"
    "-webkit-mask-size\0"
    "-webkit-perspective-origin-x\0"
    "-webkit-perspective-origin-y\0"
    "-webkit-print-color-adjust\0"
    "-webkit-rtl-ordering\0"
    "-webkit-ruby-position\0"
    "-webkit-tap-highlight-color\0"
    "-webkit-text-combine\0"
    "-webkit-text-emphasis-color\0"
    "-webkit-text-emphasis-position\0"
    "-webkit-text-emphasis-style\0"
    "-webkit-text-fill-color\0"
    "-webkit-text-security\0"
    "-webkit-text-stroke-color\0"
    "-webkit-text-stroke-width\0"
    "-webkit-transform-origin-x\0"
    "-webkit-transform-origin-y\0"
    "-webkit-transform-origin-z\0"
    "-webkit-user-drag\0"
    "-webkit-user-modify\0"
    "-webkit-user-select\0"
    "white-space\0"
    "widows\0"
    "width\0"
    "will-change\0"
    "word-break\0"
    "word-spacing\0"
    "word-wrap\0"
    "writing-mode\0"
    "z-index\0"
    "-webkit-border-end-color\0"
    "-webkit-border-end-style\0"
    "-webkit-border-end-width\0"
    "-webkit-border-start-color\0"
    "-webkit-border-start-style\0"
    "-webkit-border-start-width\0"
    "-webkit-border-before-color\0"
    "-webkit-border-before-style\0"
    "-webkit-border-before-width\0"
    "-webkit-border-after-color\0"
    "-webkit-border-after-style\0"
    "-webkit-border-after-width\0"
    "-webkit-margin-end\0"
    "-webkit-margin-start\0"
    "-webkit-margin-before\0"
    "-webkit-margin-after\0"
    "-webkit-padding-end\0"
    "-webkit-padding-start\0"
    "-webkit-padding-before\0"
    "-webkit-padding-after\0"
    "-webkit-logical-width\0"
    "-webkit-logical-height\0"
    "-webkit-min-logical-width\0"
    "-webkit-min-logical-height\0"
    "-webkit-max-logical-width\0"
    "-webkit-max-logical-height\0"
    "all\0"
    "enable-background\0"
    "max-zoom\0"
    "min-zoom\0"
    "orientation\0"
    "page\0"
    "src\0"
    "unicode-range\0"
    "user-zoom\0"
    "-webkit-font-size-delta\0"
    "-webkit-text-decorations-in-effect\0"
    "animation\0"
    "background\0"
    "background-position\0"
    "background-repeat\0"
    "border\0"
    "border-bottom\0"
    "border-color\0"
    "border-image\0"
    "border-left\0"
    "border-radius\0"
    "border-right\0"
    "border-spacing\0"
    "border-style\0"
    "border-top\0"
    "border-width\0"
    "flex\0"
    "flex-flow\0"
    "font\0"
    "grid\0"
    "grid-area\0"
    "grid-column\0"
    "grid-row\0"
    "grid-template\0"
    "list-style\0"
    "margin\0"
    "marker\0"
    "motion\0"
    "outline\0"
    "overflow\0"
    "padding\0"
    "transition\0"
    "-webkit-border-after\0"
    "-webkit-border-before\0"
    "-webkit-border-end\0"
    "-webkit-border-start\0"
    "-webkit-column-rule\0"
    "-webkit-columns\0"
    "-webkit-margin-collapse\0"
    "-webkit-mask\0"
    "-webkit-mask-box-image\0"
    "-webkit-mask-position\0"
    "-webkit-mask-repeat\0"
    "-webkit-text-emphasis\0"
    "-webkit-text-stroke\0"
    "-epub-text-orientation\0"
    "-epub-writing-mode\0"
    "-webkit-align-content\0"
    "-webkit-align-items\0"
    "-webkit-align-self\0"
    "-webkit-animation-delay\0"
    "-webkit-animation-direction\0"
    "-webkit-animation-duration\0"
    "-webkit-animation-fill-mode\0"
    "-webkit-animation-iteration-count\0"
    "-webkit-animation-name\0"
    "-webkit-animation-play-state\0"
    "-webkit-animation-timing-function\0"
    "-webkit-backface-visibility\0"
    "-webkit-background-size\0"
    "-webkit-border-bottom-left-radius\0"
    "-webkit-border-bottom-right-radius\0"
    "-webkit-border-top-left-radius\0"
    "-webkit-border-top-right-radius\0"
    "-webkit-box-shadow\0"
    "-webkit-box-sizing\0"
    "-epub-caption-side\0"
    "-webkit-flex-basis\0"
    "-webkit-flex-direction\0"
    "-webkit-flex-grow\0"
    "-webkit-flex-shrink\0"
    "-webkit-flex-wrap\0"
    "-webkit-justify-content\0"
    "-webkit-opacity\0"
    "-webkit-order\0"
    "-webkit-perspective\0"
    "-webkit-perspective-origin\0"
    "-webkit-shape-image-threshold\0"
    "-webkit-shape-margin\0"
    "-webkit-shape-outside\0"
    "-epub-text-transform\0"
    "-webkit-transform\0"
    "-webkit-transform-origin\0"
    "-webkit-transform-style\0"
    "-webkit-transition-delay\0"
    "-webkit-transition-duration\0"
    "-webkit-transition-property\0"
    "-webkit-transition-timing-function\0"
    "-epub-text-combine\0"
    "-epub-text-emphasis-color\0"
    "-epub-text-emphasis-style\0"
    "-epub-word-break\0"
    "-webkit-animation\0"
    "-webkit-border-radius\0"
    "-webkit-flex\0"
    "-webkit-flex-flow\0"
    "-webkit-transition\0"
    "-epub-text-emphasis\0"
};

static const unsigned short propertyNameStringsOffsets[] = {
    0,
    6,
    16,
    28,
    41,
    51,
    68,
    81,
    92,
    105,
    128,
    140,
    162,
    192,
    215,
    230,
    255,
    276,
    291,
    296,
    310,
    322,
    341,
    352,
    368,
    388,
    407,
    427,
    453,
    468,
    489,
    515,
    531,
    551,
    573,
    595,
    611,
    628,
    645,
    663,
    685,
    707,
    727,
    747,
    763,
    778,
    798,
    824,
    851,
    871,
    891,
    907,
    927,
    947,
    966,
    986,
    1005,
    1023,
    1041,
    1059,
    1078,
    1097,
    1116,
    1133,
    1156,
    1180,
    1197,
    1214,
    1221,
    1232,
    1243,
    1262,
    1275,
    1281,
    1286,
    1296,
    1306,
    1326,
    1354,
    1370,
    1382,
    1390,
    1408,
    1422,
    1429,
    1432,
    1435,
    1443,
    1461,
    1473,
    1478,
    1491,
    1501,
    1508,
    1519,
    1534,
    1544,
    1556,
    1566,
    1572,
    1584,
    1598,
    1607,
    1636,
    1663,
    1681,
    1696,
    1711,
    1727,
    1745,
    1761,
    1774,
    1789,
    1802,
    1822,
    1844,
    1863,
    1870,
    1886,
    1904,
    1914,
    1930,
    1944,
    1957,
    1962,
    1977,
    1992,
    2004,
    2021,
    2041,
    2057,
    2071,
    2083,
    2096,
    2107,
    2118,
    2129,
    2142,
    2147,
    2164,
    2174,
    2185,
    2195,
    2206,
    2216,
    2231,
    2245,
    2257,
    2273,
    2284,
    2300,
    2308,
    2314,
    2322,
    2336,
    2351,
    2365,
    2379,
    2393,
    2404,
    2415,
    2430,
    2443,
    2457,
    2469,
    2486,
    2504,
    2522,
    2534,
    2546,
    2565,
    2580,
    2589,
    2596,
    2603,
    2609,
    2611,
    2614,
    2617,
    2633,
    2650,
    2667,
    2688,
    2709,
    2733,
    2756,
    2778,
    2791,
    2805,
    2821,
    2826,
    2832,
    2843,
    2856,
    2863,
    2880,
    2898,
    2913,
    2929,
    2947,
    2962,
    2975,
    2988,
    2997,
    3008,
    3024,
    3036,
    3052,
    3074,
    3095,
    3117,
    3129,
    3142,
    3156,
    3168,
    3183,
    3207,
    3211,
    3224,
    3234,
    3251,
    3267,
    3277,
    3284,
    3290,
    3307,
    3327,
    3347,
    3374,
    3387,
    3401,
    3416,
    3427,
    3429,
    3431,
    3450,
    3469,
    3493,
    3522,
    3548,
    3582,
    3603,
    3635,
    3653,
    3682,
    3704,
    3721,
    3744,
    3762,
    3788,
    3807,
    3824,
    3844,
    3862,
    3889,
    3917,
    3945,
    3966,
    3985,
    4011,
    4037,
    4063,
    4083,
    4104,
    4119,
    4137,
    4165,
    4190,
    4209,
    4228,
    4258,
    4289,
    4320,
    4348,
    4378,
    4408,
    4437,
    4467,
    4496,
    4514,
    4537,
    4556,
    4576,
    4600,
    4624,
    4646,
    4668,
    4686,
    4715,
    4744,
    4771,
    4792,
    4814,
    4842,
    4863,
    4891,
    4922,
    4950,
    4974,
    4996,
    5022,
    5048,
    5075,
    5102,
    5129,
    5147,
    5167,
    5187,
    5199,
    5206,
    5212,
    5224,
    5235,
    5248,
    5258,
    5271,
    5279,
    5304,
    5329,
    5354,
    5381,
    5408,
    5435,
    5463,
    5491,
    5519,
    5546,
    5573,
    5600,
    5619,
    5640,
    5662,
    5683,
    5703,
    5725,
    5748,
    5770,
    5792,
    5815,
    5841,
    5868,
    5894,
    5921,
    5925,
    5943,
    5952,
    5961,
    5973,
    5978,
    5982,
    5996,
    6006,
    6030,
    6065,
    6075,
    6086,
    6106,
    6124,
    6131,
    6145,
    6158,
    6171,
    6183,
    6197,
    6210,
    6225,
    6238,
    6249,
    6262,
    6267,
    6277,
    6282,
    6287,
    6297,
    6309,
    6318,
    6332,
    6343,
    6350,
    6357,
    6364,
    6372,
    6381,
    6389,
    6400,
    6421,
    6443,
    6462,
    6483,
    6503,
    6519,
    6543,
    6556,
    6579,
    6601,
    6621,
    6643,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6663,
    6686,
    6705,
    6705,
    6705,
    6727,
    6747,
    6747,
    6766,
    6790,
    6818,
    6845,
    6873,
    6907,
    6930,
    6959,
    6993,
    6993,
    7021,
    7021,
    7021,
    7021,
    7021,
    7021,
    7021,
    7021,
    7021,
    7021,
    7021,
    7045,
    7045,
    7045,
    7079,
    7114,
    7114,
    7114,
    7114,
    7114,
    7114,
    7114,
    7114,
    7114,
    7114,
    7114,
    7114,
    7114,
    7114,
    7114,
    7114,
    7145,
    7177,
    7177,
    7177,
    7177,
    7196,
    7215,
    7215,
    7234,
    7234,
    7234,
    7234,
    7234,
    7234,
    7234,
    7234,
    7234,
    7234,
    7234,
    7234,
    7234,
    7234,
    7234,
    7234,
    7234,
    7234,
    7234,
    7234,
    7234,
    7234,
    7253,
    7276,
    7294,
    7314,
    7332,
    7332,
    7332,
    7332,
    7332,
    7332,
    7332,
    7332,
    7332,
    7332,
    7332,
    7332,
    7332,
    7332,
    7332,
    7332,
    7332,
    7332,
    7332,
    7332,
    7332,
    7332,
    7332,
    7356,
    7356,
    7356,
    7356,
    7356,
    7356,
    7356,
    7356,
    7356,
    7356,
    7356,
    7356,
    7356,
    7356,
    7356,
    7356,
    7356,
    7356,
    7356,
    7356,
    7356,
    7356,
    7356,
    7356,
    7356,
    7356,
    7356,
    7356,
    7356,
    7356,
    7372,
    7386,
    7386,
    7386,
    7386,
    7386,
    7386,
    7386,
    7386,
    7386,
    7386,
    7386,
    7386,
    7386,
    7386,
    7386,
    7386,
    7386,
    7406,
    7433,
    7433,
    7433,
    7433,
    7433,
    7433,
    7433,
    7433,
    7433,
    7433,
    7433,
    7433,
    7433,
    7433,
    7433,
    7433,
    7463,
    7484,
    7506,
    7506,
    7506,
    7506,
    7506,
    7506,
    7506,
    7506,
    7506,
    7506,
    7506,
    7506,
    7506,
    7506,
    7506,
    7506,
    7506,
    7506,
    7506,
    7506,
    7506,
    7506,
    7506,
    7506,
    7506,
    7506,
    7506,
    7527,
    7527,
    7527,
    7527,
    7545,
    7570,
    7594,
    7594,
    7594,
    7594,
    7619,
    7647,
    7675,
    7710,
    7710,
    7710,
    7710,
    7710,
    7710,
    7710,
    7710,
    7710,
    7710,
    7710,
    7710,
    7710,
    7710,
    7710,
    7710,
    7710,
    7710,
    7710,
    7710,
    7710,
    7710,
    7710,
    7710,
    7710,
    7710,
    7710,
    7710,
    7710,
    7710,
    7710,
    7710,
    7710,
    7710,
    7710,
    7710,
    7710,
    7710,
    7710,
    7710,
    7710,
    7710,
    7710,
    7710,
    7710,
    7710,
    7710,
    7710,
    7710,
    7710,
    7710,
    7710,
    7710,
    7710,
    7710,
    7710,
    7710,
    7710,
    7710,
    7710,
    7710,
    7710,
    7710,
    7710,
    7710,
    7710,
    7729,
    7755,
    7755,
    7781,
    7781,
    7781,
    7781,
    7781,
    7781,
    7781,
    7781,
    7781,
    7781,
    7781,
    7781,
    7781,
    7781,
    7781,
    7798,
    7798,
    7798,
    7798,
    7798,
    7798,
    7798,
    7798,
    7798,
    7798,
    7798,
    7798,
    7798,
    7798,
    7798,
    7798,
    7798,
    7798,
    7798,
    7798,
    7798,
    7798,
    7798,
    7798,
    7798,
    7798,
    7798,
    7798,
    7798,
    7798,
    7798,
    7798,
    7798,
    7798,
    7798,
    7798,
    7798,
    7798,
    7798,
    7798,
    7798,
    7798,
    7816,
    7816,
    7816,
    7816,
    7816,
    7816,
    7816,
    7816,
    7816,
    7838,
    7838,
    7838,
    7838,
    7838,
    7838,
    7851,
    7869,
    7869,
    7869,
    7869,
    7869,
    7869,
    7869,
    7869,
    7869,
    7869,
    7869,
    7869,
    7869,
    7869,
    7888,
    7888,
    7888,
    7888,
    7888,
    7888,
    7888,
    7888,
    7888,
    7888,
    7888,
    7888,
};

enum
  {
    TOTAL_KEYWORDS = 450,
    MIN_WORD_LENGTH = 1,
    MAX_WORD_LENGTH = 34,
    MIN_HASH_VALUE = 5,
    MAX_HASH_VALUE = 2964
  };

/* maximum key range = 2960, duplicates = 0 */

class CSSPropertyNamesHash
{
private:
  static inline unsigned int property_hash_function (const char *str, unsigned int len);
public:
  static const struct Property *findPropertyImpl (const char *str, unsigned int len);
};

inline unsigned int
CSSPropertyNamesHash::property_hash_function (register const char *str, register unsigned int len)
{
  static const unsigned short asso_values[] =
    {
      2965, 2965, 2965, 2965, 2965, 2965, 2965, 2965, 2965, 2965,
      2965, 2965, 2965, 2965, 2965, 2965, 2965, 2965, 2965, 2965,
      2965, 2965, 2965, 2965, 2965, 2965, 2965, 2965, 2965, 2965,
      2965, 2965, 2965, 2965, 2965, 2965, 2965, 2965, 2965, 2965,
      2965, 2965, 2965, 2965, 2965,    5, 2965, 2965, 2965, 2965,
      2965, 2965, 2965, 2965, 2965, 2965, 2965, 2965, 2965, 2965,
      2965, 2965, 2965, 2965, 2965, 2965, 2965, 2965, 2965, 2965,
      2965, 2965, 2965, 2965, 2965, 2965, 2965, 2965, 2965, 2965,
      2965, 2965, 2965, 2965, 2965, 2965, 2965, 2965, 2965, 2965,
      2965, 2965, 2965, 2965, 2965, 2965, 2965,    5,    7,  138,
         7,    6,  452,    5,  504,    5,    7,   27,  122,    5,
         5,    5,  102,    5,    5,   34,    6,  254,  100,    5,
       831,  863,  150, 2965, 2965, 2965, 2965, 2965, 2965, 2965,
      2965, 2965, 2965, 2965, 2965, 2965, 2965, 2965, 2965, 2965,
      2965, 2965, 2965, 2965, 2965, 2965, 2965, 2965, 2965, 2965,
      2965, 2965, 2965, 2965, 2965, 2965, 2965, 2965, 2965, 2965,
      2965, 2965, 2965, 2965, 2965, 2965, 2965, 2965, 2965, 2965,
      2965, 2965, 2965, 2965, 2965, 2965, 2965, 2965, 2965, 2965,
      2965, 2965, 2965, 2965, 2965, 2965, 2965, 2965, 2965, 2965,
      2965, 2965, 2965, 2965, 2965, 2965, 2965, 2965, 2965, 2965,
      2965, 2965, 2965, 2965, 2965, 2965, 2965, 2965, 2965, 2965,
      2965, 2965, 2965, 2965, 2965, 2965, 2965, 2965, 2965, 2965,
      2965, 2965, 2965, 2965, 2965, 2965, 2965, 2965, 2965, 2965,
      2965, 2965, 2965, 2965, 2965, 2965, 2965, 2965, 2965, 2965,
      2965, 2965, 2965, 2965, 2965, 2965, 2965, 2965, 2965, 2965,
      2965, 2965, 2965, 2965, 2965, 2965
    };
  register int hval = 0;

  switch (len)
    {
      default:
        hval += asso_values[(unsigned char)str[33]];
      /*FALLTHROUGH*/
      case 33:
        hval += asso_values[(unsigned char)str[32]];
      /*FALLTHROUGH*/
      case 32:
        hval += asso_values[(unsigned char)str[31]];
      /*FALLTHROUGH*/
      case 31:
        hval += asso_values[(unsigned char)str[30]];
      /*FALLTHROUGH*/
      case 30:
        hval += asso_values[(unsigned char)str[29]];
      /*FALLTHROUGH*/
      case 29:
        hval += asso_values[(unsigned char)str[28]];
      /*FALLTHROUGH*/
      case 28:
        hval += asso_values[(unsigned char)str[27]];
      /*FALLTHROUGH*/
      case 27:
        hval += asso_values[(unsigned char)str[26]];
      /*FALLTHROUGH*/
      case 26:
        hval += asso_values[(unsigned char)str[25]];
      /*FALLTHROUGH*/
      case 25:
        hval += asso_values[(unsigned char)str[24]];
      /*FALLTHROUGH*/
      case 24:
        hval += asso_values[(unsigned char)str[23]];
      /*FALLTHROUGH*/
      case 23:
        hval += asso_values[(unsigned char)str[22]];
      /*FALLTHROUGH*/
      case 22:
        hval += asso_values[(unsigned char)str[21]];
      /*FALLTHROUGH*/
      case 21:
        hval += asso_values[(unsigned char)str[20]];
      /*FALLTHROUGH*/
      case 20:
        hval += asso_values[(unsigned char)str[19]];
      /*FALLTHROUGH*/
      case 19:
        hval += asso_values[(unsigned char)str[18]];
      /*FALLTHROUGH*/
      case 18:
        hval += asso_values[(unsigned char)str[17]];
      /*FALLTHROUGH*/
      case 17:
        hval += asso_values[(unsigned char)str[16]];
      /*FALLTHROUGH*/
      case 16:
        hval += asso_values[(unsigned char)str[15]];
      /*FALLTHROUGH*/
      case 15:
        hval += asso_values[(unsigned char)str[14]];
      /*FALLTHROUGH*/
      case 14:
        hval += asso_values[(unsigned char)str[13]];
      /*FALLTHROUGH*/
      case 13:
        hval += asso_values[(unsigned char)str[12]];
      /*FALLTHROUGH*/
      case 12:
        hval += asso_values[(unsigned char)str[11]];
      /*FALLTHROUGH*/
      case 11:
        hval += asso_values[(unsigned char)str[10]];
      /*FALLTHROUGH*/
      case 10:
        hval += asso_values[(unsigned char)str[9]];
      /*FALLTHROUGH*/
      case 9:
        hval += asso_values[(unsigned char)str[8]];
      /*FALLTHROUGH*/
      case 8:
        hval += asso_values[(unsigned char)str[7]];
      /*FALLTHROUGH*/
      case 7:
        hval += asso_values[(unsigned char)str[6]];
      /*FALLTHROUGH*/
      case 6:
        hval += asso_values[(unsigned char)str[5]];
      /*FALLTHROUGH*/
      case 5:
        hval += asso_values[(unsigned char)str[4]];
      /*FALLTHROUGH*/
      case 4:
        hval += asso_values[(unsigned char)str[3]];
      /*FALLTHROUGH*/
      case 3:
        hval += asso_values[(unsigned char)str[2]];
      /*FALLTHROUGH*/
      case 2:
        hval += asso_values[(unsigned char)str[1]];
      /*FALLTHROUGH*/
      case 1:
        hval += asso_values[(unsigned char)str[0]];
        break;
    }
  return hval;
}

struct stringpool_t
  {
    char stringpool_str0[sizeof("r")];
    char stringpool_str1[sizeof("grid")];
    char stringpool_str2[sizeof("order")];
    char stringpool_str3[sizeof("margin")];
    char stringpool_str4[sizeof("motion")];
    char stringpool_str5[sizeof("rotate")];
    char stringpool_str6[sizeof("bottom")];
    char stringpool_str7[sizeof("border")];
    char stringpool_str8[sizeof("grid-row")];
    char stringpool_str9[sizeof("ime-mode")];
    char stringpool_str10[sizeof("animation")];
    char stringpool_str11[sizeof("grid-area")];
    char stringpool_str12[sizeof("marker")];
    char stringpool_str13[sizeof("orientation")];
    char stringpool_str14[sizeof("widows")];
    char stringpool_str15[sizeof("writing-mode")];
    char stringpool_str16[sizeof("grid-row-end")];
    char stringpool_str17[sizeof("border-image")];
    char stringpool_str18[sizeof("margin-bottom")];
    char stringpool_str19[sizeof("mask")];
    char stringpool_str20[sizeof("animation-name")];
    char stringpool_str21[sizeof("border-bottom")];
    char stringpool_str22[sizeof("marker-mid")];
    char stringpool_str23[sizeof("marker-end")];
    char stringpool_str24[sizeof("word-break")];
    char stringpool_str25[sizeof("motion-rotation")];
    char stringpool_str26[sizeof("image-rendering")];
    char stringpool_str27[sizeof("transition")];
    char stringpool_str28[sizeof("stroke")];
    char stringpool_str29[sizeof("image-orientation")];
    char stringpool_str30[sizeof("-webkit-order")];
    char stringpool_str31[sizeof("grid-row-start")];
    char stringpool_str32[sizeof("-webkit-animation")];
    char stringpool_str33[sizeof("top")];
    char stringpool_str34[sizeof("marker-start")];
    char stringpool_str35[sizeof("page")];
    char stringpool_str36[sizeof("-webkit-margin-end")];
    char stringpool_str37[sizeof("-webkit-border-end")];
    char stringpool_str38[sizeof("-webkit-writing-mode")];
    char stringpool_str39[sizeof("-webkit-border-image")];
    char stringpool_str40[sizeof("padding")];
    char stringpool_str41[sizeof("-webkit-mask")];
    char stringpool_str42[sizeof("-webkit-animation-name")];
    char stringpool_str43[sizeof("word-wrap")];
    char stringpool_str44[sizeof("-webkit-transition")];
    char stringpool_str45[sizeof("margin-top")];
    char stringpool_str46[sizeof("border-top")];
    char stringpool_str47[sizeof("paint-order")];
    char stringpool_str48[sizeof("-webkit-margin-start")];
    char stringpool_str49[sizeof("grid-row-gap")];
    char stringpool_str50[sizeof("-webkit-border-start")];
    char stringpool_str51[sizeof("zoom")];
    char stringpool_str52[sizeof("position")];
    char stringpool_str53[sizeof("-webkit-mask-image")];
    char stringpool_str54[sizeof("content")];
    char stringpool_str55[sizeof("-webkit-mask-origin")];
    char stringpool_str56[sizeof("speak")];
    char stringpool_str57[sizeof("padding-bottom")];
    char stringpool_str58[sizeof("src")];
    char stringpool_str59[sizeof("direction")];
    char stringpool_str60[sizeof("min-zoom")];
    char stringpool_str61[sizeof("isolation")];
    char stringpool_str62[sizeof("translate")];
    char stringpool_str63[sizeof("size")];
    char stringpool_str64[sizeof("border-image-repeat")];
    char stringpool_str65[sizeof("align-items")];
    char stringpool_str66[sizeof("resize")];
    char stringpool_str67[sizeof("tab-size")];
    char stringpool_str68[sizeof("-webkit-padding-end")];
    char stringpool_str69[sizeof("animation-direction")];
    char stringpool_str70[sizeof("dominant-baseline")];
    char stringpool_str71[sizeof("page-break-inside")];
    char stringpool_str72[sizeof("-webkit-rtl-ordering")];
    char stringpool_str73[sizeof("stroke-linejoin")];
    char stringpool_str74[sizeof("all")];
    char stringpool_str75[sizeof("padding-top")];
    char stringpool_str76[sizeof("stroke-miterlimit")];
    char stringpool_str77[sizeof("-webkit-line-break")];
    char stringpool_str78[sizeof("-webkit-padding-start")];
    char stringpool_str79[sizeof("-webkit-align-items")];
    char stringpool_str80[sizeof("-webkit-mask-repeat")];
    char stringpool_str81[sizeof("color")];
    char stringpool_str82[sizeof("clear")];
    char stringpool_str83[sizeof("grid-template")];
    char stringpool_str84[sizeof("pointer-events")];
    char stringpool_str85[sizeof("-webkit-animation-direction")];
    char stringpool_str86[sizeof("scale")];
    char stringpool_str87[sizeof("-webkit-mask-position")];
    char stringpool_str88[sizeof("quotes")];
    char stringpool_str89[sizeof("-webkit-app-region")];
    char stringpool_str90[sizeof("border-color")];
    char stringpool_str91[sizeof("align-content")];
    char stringpool_str92[sizeof("word-spacing")];
    char stringpool_str93[sizeof("caption-side")];
    char stringpool_str94[sizeof("color-rendering")];
    char stringpool_str95[sizeof("border-spacing")];
    char stringpool_str96[sizeof("-webkit-mask-size")];
    char stringpool_str97[sizeof("grid-template-rows")];
    char stringpool_str98[sizeof("object-position")];
    char stringpool_str99[sizeof("animation-duration")];
    char stringpool_str100[sizeof("grid-template-areas")];
    char stringpool_str101[sizeof("border-radius")];
    char stringpool_str102[sizeof("grid-auto-rows")];
    char stringpool_str103[sizeof("border-bottom-color")];
    char stringpool_str104[sizeof("alignment-baseline")];
    char stringpool_str105[sizeof("clip")];
    char stringpool_str106[sizeof("border-image-slice")];
    char stringpool_str107[sizeof("transition-duration")];
    char stringpool_str108[sizeof("border-image-outset")];
    char stringpool_str109[sizeof("-webkit-align-content")];
    char stringpool_str110[sizeof("-webkit-user-drag")];
    char stringpool_str111[sizeof("outline")];
    char stringpool_str112[sizeof("-webkit-border-end-color")];
    char stringpool_str113[sizeof("-webkit-animation-duration")];
    char stringpool_str114[sizeof("-webkit-border-radius")];
    char stringpool_str115[sizeof("stop-color")];
    char stringpool_str116[sizeof("border-top-color")];
    char stringpool_str117[sizeof("cursor")];
    char stringpool_str118[sizeof("-webkit-border-start-color")];
    char stringpool_str119[sizeof("-epub-writing-mode")];
    char stringpool_str120[sizeof("-webkit-transition-duration")];
    char stringpool_str121[sizeof("-webkit-appearance")];
    char stringpool_str122[sizeof("-webkit-mask-composite")];
    char stringpool_str123[sizeof("unicode-bidi")];
    char stringpool_str124[sizeof("letter-spacing")];
    char stringpool_str125[sizeof("unicode-range")];
    char stringpool_str126[sizeof("-epub-word-break")];
    char stringpool_str127[sizeof("background")];
    char stringpool_str128[sizeof("-webkit-locale")];
    char stringpool_str129[sizeof("font")];
    char stringpool_str130[sizeof("user-zoom")];
    char stringpool_str131[sizeof("stroke-linecap")];
    char stringpool_str132[sizeof("counter-reset")];
    char stringpool_str133[sizeof("background-image")];
    char stringpool_str134[sizeof("background-origin")];
    char stringpool_str135[sizeof("-webkit-mask-clip")];
    char stringpool_str136[sizeof("perspective")];
    char stringpool_str137[sizeof("animation-iteration-count")];
    char stringpool_str138[sizeof("border-image-source")];
    char stringpool_str139[sizeof("transform")];
    char stringpool_str140[sizeof("right")];
    char stringpool_str141[sizeof("width")];
    char stringpool_str142[sizeof("font-kerning")];
    char stringpool_str143[sizeof("vertical-align")];
    char stringpool_str144[sizeof("perspective-origin")];
    char stringpool_str145[sizeof("min-width")];
    char stringpool_str146[sizeof("grid-column")];
    char stringpool_str147[sizeof("transform-origin")];
    char stringpool_str148[sizeof("-webkit-background-origin")];
    char stringpool_str149[sizeof("margin-right")];
    char stringpool_str150[sizeof("color-interpolation")];
    char stringpool_str151[sizeof("border-right")];
    char stringpool_str152[sizeof("border-width")];
    char stringpool_str153[sizeof("border-collapse")];
    char stringpool_str154[sizeof("-webkit-margin-after")];
    char stringpool_str155[sizeof("-webkit-perspective")];
    char stringpool_str156[sizeof("-webkit-animation-iteration-count")];
    char stringpool_str157[sizeof("grid-column-end")];
    char stringpool_str158[sizeof("-webkit-border-after")];
    char stringpool_str159[sizeof("-webkit-line-clamp")];
    char stringpool_str160[sizeof("-webkit-margin-before")];
    char stringpool_str161[sizeof("left")];
    char stringpool_str162[sizeof("-webkit-border-before")];
    char stringpool_str163[sizeof("-webkit-transform")];
    char stringpool_str164[sizeof("float")];
    char stringpool_str165[sizeof("background-repeat")];
    char stringpool_str166[sizeof("filter")];
    char stringpool_str167[sizeof("border-image-width")];
    char stringpool_str168[sizeof("font-variant")];
    char stringpool_str169[sizeof("counter-increment")];
    char stringpool_str170[sizeof("border-bottom-width")];
    char stringpool_str171[sizeof("-webkit-perspective-origin")];
    char stringpool_str172[sizeof("enable-background")];
    char stringpool_str173[sizeof("stroke-width")];
    char stringpool_str174[sizeof("grid-column-start")];
    char stringpool_str175[sizeof("margin-left")];
    char stringpool_str176[sizeof("-webkit-transform-origin")];
    char stringpool_str177[sizeof("border-left")];
    char stringpool_str178[sizeof("-webkit-columns")];
    char stringpool_str179[sizeof("background-position")];
    char stringpool_str180[sizeof("-webkit-margin-collapse")];
    char stringpool_str181[sizeof("object-fit")];
    char stringpool_str182[sizeof("background-blend-mode")];
    char stringpool_str183[sizeof("page-break-after")];
    char stringpool_str184[sizeof("motion-path")];
    char stringpool_str185[sizeof("-webkit-border-end-width")];
    char stringpool_str186[sizeof("background-size")];
    char stringpool_str187[sizeof("page-break-before")];
    char stringpool_str188[sizeof("orphans")];
    char stringpool_str189[sizeof("-webkit-filter")];
    char stringpool_str190[sizeof("padding-right")];
    char stringpool_str191[sizeof("font-size")];
    char stringpool_str192[sizeof("scroll-snap-destination")];
    char stringpool_str193[sizeof("grid-column-gap")];
    char stringpool_str194[sizeof("-webkit-margin-bottom-collapse")];
    char stringpool_str195[sizeof("-webkit-padding-after")];
    char stringpool_str196[sizeof("-webkit-user-select")];
    char stringpool_str197[sizeof("outline-color")];
    char stringpool_str198[sizeof("border-top-width")];
    char stringpool_str199[sizeof("shape-margin")];
    char stringpool_str200[sizeof("-webkit-padding-before")];
    char stringpool_str201[sizeof("-webkit-border-start-width")];
    char stringpool_str202[sizeof("overflow")];
    char stringpool_str203[sizeof("fill")];
    char stringpool_str204[sizeof("-epub-caption-side")];
    char stringpool_str205[sizeof("shape-rendering")];
    char stringpool_str206[sizeof("-webkit-column-gap")];
    char stringpool_str207[sizeof("-webkit-column-break-inside")];
    char stringpool_str208[sizeof("-webkit-background-size")];
    char stringpool_str209[sizeof("padding-left")];
    char stringpool_str210[sizeof("background-color")];
    char stringpool_str211[sizeof("-webkit-column-span")];
    char stringpool_str212[sizeof("-webkit-shape-margin")];
    char stringpool_str213[sizeof("-webkit-margin-top-collapse")];
    char stringpool_str214[sizeof("clip-rule")];
    char stringpool_str215[sizeof("align-self")];
    char stringpool_str216[sizeof("scroll-snap-coordinate")];
    char stringpool_str217[sizeof("-webkit-transform-origin-z")];
    char stringpool_str218[sizeof("scroll-blocks-on")];
    char stringpool_str219[sizeof("animation-fill-mode")];
    char stringpool_str220[sizeof("-webkit-print-color-adjust")];
    char stringpool_str221[sizeof("-webkit-border-vertical-spacing")];
    char stringpool_str222[sizeof("white-space")];
    char stringpool_str223[sizeof("overflow-wrap")];
    char stringpool_str224[sizeof("-webkit-align-self")];
    char stringpool_str225[sizeof("background-clip")];
    char stringpool_str226[sizeof("x")];
    char stringpool_str227[sizeof("-webkit-background-composite")];
    char stringpool_str228[sizeof("rx")];
    char stringpool_str229[sizeof("border-right-color")];
    char stringpool_str230[sizeof("-webkit-animation-fill-mode")];
    char stringpool_str231[sizeof("grid-template-columns")];
    char stringpool_str232[sizeof("-webkit-border-after-color")];
    char stringpool_str233[sizeof("y")];
    char stringpool_str234[sizeof("grid-auto-columns")];
    char stringpool_str235[sizeof("-webkit-border-before-color")];
    char stringpool_str236[sizeof("ry")];
    char stringpool_str237[sizeof("flood-color")];
    char stringpool_str238[sizeof("-webkit-font-size-delta")];
    char stringpool_str239[sizeof("grid-auto-flow")];
    char stringpool_str240[sizeof("text-indent")];
    char stringpool_str241[sizeof("-webkit-background-clip")];
    char stringpool_str242[sizeof("backdrop-filter")];
    char stringpool_str243[sizeof("text-rendering")];
    char stringpool_str244[sizeof("border-left-color")];
    char stringpool_str245[sizeof("border-bottom-right-radius")];
    char stringpool_str246[sizeof("will-change")];
    char stringpool_str247[sizeof("outline-width")];
    char stringpool_str248[sizeof("lighting-color")];
    char stringpool_str249[sizeof("-webkit-box-orient")];
    char stringpool_str250[sizeof("animation-timing-function")];
    char stringpool_str251[sizeof("cx")];
    char stringpool_str252[sizeof("shape-outside")];
    char stringpool_str253[sizeof("-webkit-text-orientation")];
    char stringpool_str254[sizeof("border-bottom-left-radius")];
    char stringpool_str255[sizeof("-webkit-border-bottom-right-radius")];
    char stringpool_str256[sizeof("font-size-adjust")];
    char stringpool_str257[sizeof("-webkit-column-rule")];
    char stringpool_str258[sizeof("clip-path")];
    char stringpool_str259[sizeof("motion-offset")];
    char stringpool_str260[sizeof("transition-timing-function")];
    char stringpool_str261[sizeof("text-align")];
    char stringpool_str262[sizeof("border-top-right-radius")];
    char stringpool_str263[sizeof("-webkit-logical-width")];
    char stringpool_str264[sizeof("cy")];
    char stringpool_str265[sizeof("-webkit-text-stroke")];
    char stringpool_str266[sizeof("font-weight")];
    char stringpool_str267[sizeof("-webkit-column-count")];
    char stringpool_str268[sizeof("z-index")];
    char stringpool_str269[sizeof("max-zoom")];
    char stringpool_str270[sizeof("-webkit-mask-box-image")];
    char stringpool_str271[sizeof("-webkit-min-logical-width")];
    char stringpool_str272[sizeof("mix-blend-mode")];
    char stringpool_str273[sizeof("-webkit-animation-timing-function")];
    char stringpool_str274[sizeof("height")];
    char stringpool_str275[sizeof("-webkit-shape-outside")];
    char stringpool_str276[sizeof("text-decoration")];
    char stringpool_str277[sizeof("-webkit-border-bottom-left-radius")];
    char stringpool_str278[sizeof("min-height")];
    char stringpool_str279[sizeof("font-variant-ligatures")];
    char stringpool_str280[sizeof("box-sizing")];
    char stringpool_str281[sizeof("mask-type")];
    char stringpool_str282[sizeof("animation-delay")];
    char stringpool_str283[sizeof("-webkit-clip-path")];
    char stringpool_str284[sizeof("-webkit-box-align")];
    char stringpool_str285[sizeof("-webkit-transition-timing-function")];
    char stringpool_str286[sizeof("border-top-left-radius")];
    char stringpool_str287[sizeof("-webkit-border-top-right-radius")];
    char stringpool_str288[sizeof("scroll-behavior")];
    char stringpool_str289[sizeof("border-style")];
    char stringpool_str290[sizeof("touch-action")];
    char stringpool_str291[sizeof("-webkit-box-lines")];
    char stringpool_str292[sizeof("transition-delay")];
    char stringpool_str293[sizeof("-webkit-text-combine")];
    char stringpool_str294[sizeof("fill-rule")];
    char stringpool_str295[sizeof("-webkit-box-direction")];
    char stringpool_str296[sizeof("border-right-width")];
    char stringpool_str297[sizeof("-webkit-mask-repeat-x")];
    char stringpool_str298[sizeof("border-bottom-style")];
    char stringpool_str299[sizeof("-webkit-border-after-width")];
    char stringpool_str300[sizeof("-webkit-font-smoothing")];
    char stringpool_str301[sizeof("-webkit-margin-after-collapse")];
    char stringpool_str302[sizeof("-webkit-box-sizing")];
    char stringpool_str303[sizeof("-webkit-border-before-width")];
    char stringpool_str304[sizeof("-webkit-animation-delay")];
    char stringpool_str305[sizeof("-webkit-margin-before-collapse")];
    char stringpool_str306[sizeof("opacity")];
    char stringpool_str307[sizeof("-webkit-border-top-left-radius")];
    char stringpool_str308[sizeof("-webkit-column-width")];
    char stringpool_str309[sizeof("-webkit-column-break-after")];
    char stringpool_str310[sizeof("-webkit-column-break-before")];
    char stringpool_str311[sizeof("display")];
    char stringpool_str312[sizeof("-webkit-mask-repeat-y")];
    char stringpool_str313[sizeof("-webkit-mask-position-x")];
    char stringpool_str314[sizeof("background-attachment")];
    char stringpool_str315[sizeof("-webkit-mask-box-image-repeat")];
    char stringpool_str316[sizeof("visibility")];
    char stringpool_str317[sizeof("-webkit-transition-delay")];
    char stringpool_str318[sizeof("-webkit-box-decoration-break")];
    char stringpool_str319[sizeof("border-left-width")];
    char stringpool_str320[sizeof("-webkit-border-end-style")];
    char stringpool_str321[sizeof("text-align-last")];
    char stringpool_str322[sizeof("font-stretch")];
    char stringpool_str323[sizeof("line-height")];
    char stringpool_str324[sizeof("-webkit-mask-position-y")];
    char stringpool_str325[sizeof("transition-property")];
    char stringpool_str326[sizeof("text-decoration-line")];
    char stringpool_str327[sizeof("-webkit-box-pack")];
    char stringpool_str328[sizeof("border-top-style")];
    char stringpool_str329[sizeof("-webkit-opacity")];
    char stringpool_str330[sizeof("baseline-shift")];
    char stringpool_str331[sizeof("color-interpolation-filters")];
    char stringpool_str332[sizeof("-webkit-border-start-style")];
    char stringpool_str333[sizeof("list-style")];
    char stringpool_str334[sizeof("animation-play-state")];
    char stringpool_str335[sizeof("stroke-opacity")];
    char stringpool_str336[sizeof("-webkit-border-horizontal-spacing")];
    char stringpool_str337[sizeof("-webkit-line-box-contain")];
    char stringpool_str338[sizeof("list-style-image")];
    char stringpool_str339[sizeof("column-fill")];
    char stringpool_str340[sizeof("buffered-rendering")];
    char stringpool_str341[sizeof("-webkit-transition-property")];
    char stringpool_str342[sizeof("-webkit-column-rule-color")];
    char stringpool_str343[sizeof("-webkit-animation-play-state")];
    char stringpool_str344[sizeof("stop-opacity")];
    char stringpool_str345[sizeof("-webkit-text-stroke-color")];
    char stringpool_str346[sizeof("-epub-text-orientation")];
    char stringpool_str347[sizeof("font-feature-settings")];
    char stringpool_str348[sizeof("text-decoration-color")];
    char stringpool_str349[sizeof("vector-effect")];
    char stringpool_str350[sizeof("-webkit-mask-box-image-slice")];
    char stringpool_str351[sizeof("-webkit-mask-box-image-outset")];
    char stringpool_str352[sizeof("outline-offset")];
    char stringpool_str353[sizeof("-webkit-ruby-position")];
    char stringpool_str354[sizeof("max-width")];
    char stringpool_str355[sizeof("list-style-position")];
    char stringpool_str356[sizeof("text-transform")];
    char stringpool_str357[sizeof("-webkit-font-feature-settings")];
    char stringpool_str358[sizeof("-epub-text-combine")];
    char stringpool_str359[sizeof("table-layout")];
    char stringpool_str360[sizeof("box-shadow")];
    char stringpool_str361[sizeof("empty-cells")];
    char stringpool_str362[sizeof("flex")];
    char stringpool_str363[sizeof("text-shadow")];
    char stringpool_str364[sizeof("background-repeat-x")];
    char stringpool_str365[sizeof("flex-grow")];
    char stringpool_str366[sizeof("outline-style")];
    char stringpool_str367[sizeof("text-underline-position")];
    char stringpool_str368[sizeof("-webkit-box-ordinal-group")];
    char stringpool_str369[sizeof("-webkit-perspective-origin-x")];
    char stringpool_str370[sizeof("-webkit-transform-origin-x")];
    char stringpool_str371[sizeof("background-repeat-y")];
    char stringpool_str372[sizeof("-webkit-mask-box-image-source")];
    char stringpool_str373[sizeof("background-position-x")];
    char stringpool_str374[sizeof("-webkit-box-shadow")];
    char stringpool_str375[sizeof("-webkit-flex")];
    char stringpool_str376[sizeof("-webkit-perspective-origin-y")];
    char stringpool_str377[sizeof("-webkit-transform-origin-y")];
    char stringpool_str378[sizeof("background-position-y")];
    char stringpool_str379[sizeof("mask-source-type")];
    char stringpool_str380[sizeof("flex-basis")];
    char stringpool_str381[sizeof("-webkit-flex-grow")];
    char stringpool_str382[sizeof("-webkit-logical-height")];
    char stringpool_str383[sizeof("font-style")];
    char stringpool_str384[sizeof("text-anchor")];
    char stringpool_str385[sizeof("-webkit-column-rule-width")];
    char stringpool_str386[sizeof("stroke-dasharray")];
    char stringpool_str387[sizeof("-webkit-min-logical-height")];
    char stringpool_str388[sizeof("flex-wrap")];
    char stringpool_str389[sizeof("-webkit-text-stroke-width")];
    char stringpool_str390[sizeof("overflow-x")];
    char stringpool_str391[sizeof("-webkit-mask-box-image-width")];
    char stringpool_str392[sizeof("text-overflow")];
    char stringpool_str393[sizeof("transform-style")];
    char stringpool_str394[sizeof("scroll-snap-type")];
    char stringpool_str395[sizeof("-webkit-flex-basis")];
    char stringpool_str396[sizeof("overflow-y")];
    char stringpool_str397[sizeof("scroll-snap-points-x")];
    char stringpool_str398[sizeof("stroke-dashoffset")];
    char stringpool_str399[sizeof("flex-direction")];
    char stringpool_str400[sizeof("-webkit-flex-wrap")];
    char stringpool_str401[sizeof("border-right-style")];
    char stringpool_str402[sizeof("scroll-snap-points-y")];
    char stringpool_str403[sizeof("-webkit-text-emphasis")];
    char stringpool_str404[sizeof("-webkit-border-after-style")];
    char stringpool_str405[sizeof("-webkit-border-before-style")];
    char stringpool_str406[sizeof("-webkit-transform-style")];
    char stringpool_str407[sizeof("-webkit-box-reflect")];
    char stringpool_str408[sizeof("border-left-style")];
    char stringpool_str409[sizeof("-webkit-flex-direction")];
    char stringpool_str410[sizeof("justify-items")];
    char stringpool_str411[sizeof("-webkit-user-modify")];
    char stringpool_str412[sizeof("flood-opacity")];
    char stringpool_str413[sizeof("-webkit-highlight")];
    char stringpool_str414[sizeof("-epub-text-transform")];
    char stringpool_str415[sizeof("-webkit-text-emphasis-position")];
    char stringpool_str416[sizeof("justify-content")];
    char stringpool_str417[sizeof("fill-opacity")];
    char stringpool_str418[sizeof("-webkit-max-logical-width")];
    char stringpool_str419[sizeof("-webkit-justify-content")];
    char stringpool_str420[sizeof("max-height")];
    char stringpool_str421[sizeof("shape-image-threshold")];
    char stringpool_str422[sizeof("-webkit-text-emphasis-color")];
    char stringpool_str423[sizeof("-webkit-text-fill-color")];
    char stringpool_str424[sizeof("font-family")];
    char stringpool_str425[sizeof("-epub-text-emphasis")];
    char stringpool_str426[sizeof("backface-visibility")];
    char stringpool_str427[sizeof("-webkit-shape-image-threshold")];
    char stringpool_str428[sizeof("flex-shrink")];
    char stringpool_str429[sizeof("flex-flow")];
    char stringpool_str430[sizeof("-webkit-backface-visibility")];
    char stringpool_str431[sizeof("-webkit-column-rule-style")];
    char stringpool_str432[sizeof("glyph-orientation-vertical")];
    char stringpool_str433[sizeof("-webkit-flex-shrink")];
    char stringpool_str434[sizeof("-webkit-flex-flow")];
    char stringpool_str435[sizeof("text-decoration-style")];
    char stringpool_str436[sizeof("-webkit-tap-highlight-color")];
    char stringpool_str437[sizeof("list-style-type")];
    char stringpool_str438[sizeof("-epub-text-emphasis-color")];
    char stringpool_str439[sizeof("-webkit-text-decorations-in-effect")];
    char stringpool_str440[sizeof("-webkit-text-security")];
    char stringpool_str441[sizeof("justify-self")];
    char stringpool_str442[sizeof("-webkit-box-flex")];
    char stringpool_str443[sizeof("-webkit-max-logical-height")];
    char stringpool_str444[sizeof("text-justify")];
    char stringpool_str445[sizeof("glyph-orientation-horizontal")];
    char stringpool_str446[sizeof("-webkit-text-emphasis-style")];
    char stringpool_str447[sizeof("-webkit-box-flex-group")];
    char stringpool_str448[sizeof("-webkit-hyphenate-character")];
    char stringpool_str449[sizeof("-epub-text-emphasis-style")];
  };
static const struct stringpool_t stringpool_contents =
  {
    "r",
    "grid",
    "order",
    "margin",
    "motion",
    "rotate",
    "bottom",
    "border",
    "grid-row",
    "ime-mode",
    "animation",
    "grid-area",
    "marker",
    "orientation",
    "widows",
    "writing-mode",
    "grid-row-end",
    "border-image",
    "margin-bottom",
    "mask",
    "animation-name",
    "border-bottom",
    "marker-mid",
    "marker-end",
    "word-break",
    "motion-rotation",
    "image-rendering",
    "transition",
    "stroke",
    "image-orientation",
    "-webkit-order",
    "grid-row-start",
    "-webkit-animation",
    "top",
    "marker-start",
    "page",
    "-webkit-margin-end",
    "-webkit-border-end",
    "-webkit-writing-mode",
    "-webkit-border-image",
    "padding",
    "-webkit-mask",
    "-webkit-animation-name",
    "word-wrap",
    "-webkit-transition",
    "margin-top",
    "border-top",
    "paint-order",
    "-webkit-margin-start",
    "grid-row-gap",
    "-webkit-border-start",
    "zoom",
    "position",
    "-webkit-mask-image",
    "content",
    "-webkit-mask-origin",
    "speak",
    "padding-bottom",
    "src",
    "direction",
    "min-zoom",
    "isolation",
    "translate",
    "size",
    "border-image-repeat",
    "align-items",
    "resize",
    "tab-size",
    "-webkit-padding-end",
    "animation-direction",
    "dominant-baseline",
    "page-break-inside",
    "-webkit-rtl-ordering",
    "stroke-linejoin",
    "all",
    "padding-top",
    "stroke-miterlimit",
    "-webkit-line-break",
    "-webkit-padding-start",
    "-webkit-align-items",
    "-webkit-mask-repeat",
    "color",
    "clear",
    "grid-template",
    "pointer-events",
    "-webkit-animation-direction",
    "scale",
    "-webkit-mask-position",
    "quotes",
    "-webkit-app-region",
    "border-color",
    "align-content",
    "word-spacing",
    "caption-side",
    "color-rendering",
    "border-spacing",
    "-webkit-mask-size",
    "grid-template-rows",
    "object-position",
    "animation-duration",
    "grid-template-areas",
    "border-radius",
    "grid-auto-rows",
    "border-bottom-color",
    "alignment-baseline",
    "clip",
    "border-image-slice",
    "transition-duration",
    "border-image-outset",
    "-webkit-align-content",
    "-webkit-user-drag",
    "outline",
    "-webkit-border-end-color",
    "-webkit-animation-duration",
    "-webkit-border-radius",
    "stop-color",
    "border-top-color",
    "cursor",
    "-webkit-border-start-color",
    "-epub-writing-mode",
    "-webkit-transition-duration",
    "-webkit-appearance",
    "-webkit-mask-composite",
    "unicode-bidi",
    "letter-spacing",
    "unicode-range",
    "-epub-word-break",
    "background",
    "-webkit-locale",
    "font",
    "user-zoom",
    "stroke-linecap",
    "counter-reset",
    "background-image",
    "background-origin",
    "-webkit-mask-clip",
    "perspective",
    "animation-iteration-count",
    "border-image-source",
    "transform",
    "right",
    "width",
    "font-kerning",
    "vertical-align",
    "perspective-origin",
    "min-width",
    "grid-column",
    "transform-origin",
    "-webkit-background-origin",
    "margin-right",
    "color-interpolation",
    "border-right",
    "border-width",
    "border-collapse",
    "-webkit-margin-after",
    "-webkit-perspective",
    "-webkit-animation-iteration-count",
    "grid-column-end",
    "-webkit-border-after",
    "-webkit-line-clamp",
    "-webkit-margin-before",
    "left",
    "-webkit-border-before",
    "-webkit-transform",
    "float",
    "background-repeat",
    "filter",
    "border-image-width",
    "font-variant",
    "counter-increment",
    "border-bottom-width",
    "-webkit-perspective-origin",
    "enable-background",
    "stroke-width",
    "grid-column-start",
    "margin-left",
    "-webkit-transform-origin",
    "border-left",
    "-webkit-columns",
    "background-position",
    "-webkit-margin-collapse",
    "object-fit",
    "background-blend-mode",
    "page-break-after",
    "motion-path",
    "-webkit-border-end-width",
    "background-size",
    "page-break-before",
    "orphans",
    "-webkit-filter",
    "padding-right",
    "font-size",
    "scroll-snap-destination",
    "grid-column-gap",
    "-webkit-margin-bottom-collapse",
    "-webkit-padding-after",
    "-webkit-user-select",
    "outline-color",
    "border-top-width",
    "shape-margin",
    "-webkit-padding-before",
    "-webkit-border-start-width",
    "overflow",
    "fill",
    "-epub-caption-side",
    "shape-rendering",
    "-webkit-column-gap",
    "-webkit-column-break-inside",
    "-webkit-background-size",
    "padding-left",
    "background-color",
    "-webkit-column-span",
    "-webkit-shape-margin",
    "-webkit-margin-top-collapse",
    "clip-rule",
    "align-self",
    "scroll-snap-coordinate",
    "-webkit-transform-origin-z",
    "scroll-blocks-on",
    "animation-fill-mode",
    "-webkit-print-color-adjust",
    "-webkit-border-vertical-spacing",
    "white-space",
    "overflow-wrap",
    "-webkit-align-self",
    "background-clip",
    "x",
    "-webkit-background-composite",
    "rx",
    "border-right-color",
    "-webkit-animation-fill-mode",
    "grid-template-columns",
    "-webkit-border-after-color",
    "y",
    "grid-auto-columns",
    "-webkit-border-before-color",
    "ry",
    "flood-color",
    "-webkit-font-size-delta",
    "grid-auto-flow",
    "text-indent",
    "-webkit-background-clip",
    "backdrop-filter",
    "text-rendering",
    "border-left-color",
    "border-bottom-right-radius",
    "will-change",
    "outline-width",
    "lighting-color",
    "-webkit-box-orient",
    "animation-timing-function",
    "cx",
    "shape-outside",
    "-webkit-text-orientation",
    "border-bottom-left-radius",
    "-webkit-border-bottom-right-radius",
    "font-size-adjust",
    "-webkit-column-rule",
    "clip-path",
    "motion-offset",
    "transition-timing-function",
    "text-align",
    "border-top-right-radius",
    "-webkit-logical-width",
    "cy",
    "-webkit-text-stroke",
    "font-weight",
    "-webkit-column-count",
    "z-index",
    "max-zoom",
    "-webkit-mask-box-image",
    "-webkit-min-logical-width",
    "mix-blend-mode",
    "-webkit-animation-timing-function",
    "height",
    "-webkit-shape-outside",
    "text-decoration",
    "-webkit-border-bottom-left-radius",
    "min-height",
    "font-variant-ligatures",
    "box-sizing",
    "mask-type",
    "animation-delay",
    "-webkit-clip-path",
    "-webkit-box-align",
    "-webkit-transition-timing-function",
    "border-top-left-radius",
    "-webkit-border-top-right-radius",
    "scroll-behavior",
    "border-style",
    "touch-action",
    "-webkit-box-lines",
    "transition-delay",
    "-webkit-text-combine",
    "fill-rule",
    "-webkit-box-direction",
    "border-right-width",
    "-webkit-mask-repeat-x",
    "border-bottom-style",
    "-webkit-border-after-width",
    "-webkit-font-smoothing",
    "-webkit-margin-after-collapse",
    "-webkit-box-sizing",
    "-webkit-border-before-width",
    "-webkit-animation-delay",
    "-webkit-margin-before-collapse",
    "opacity",
    "-webkit-border-top-left-radius",
    "-webkit-column-width",
    "-webkit-column-break-after",
    "-webkit-column-break-before",
    "display",
    "-webkit-mask-repeat-y",
    "-webkit-mask-position-x",
    "background-attachment",
    "-webkit-mask-box-image-repeat",
    "visibility",
    "-webkit-transition-delay",
    "-webkit-box-decoration-break",
    "border-left-width",
    "-webkit-border-end-style",
    "text-align-last",
    "font-stretch",
    "line-height",
    "-webkit-mask-position-y",
    "transition-property",
    "text-decoration-line",
    "-webkit-box-pack",
    "border-top-style",
    "-webkit-opacity",
    "baseline-shift",
    "color-interpolation-filters",
    "-webkit-border-start-style",
    "list-style",
    "animation-play-state",
    "stroke-opacity",
    "-webkit-border-horizontal-spacing",
    "-webkit-line-box-contain",
    "list-style-image",
    "column-fill",
    "buffered-rendering",
    "-webkit-transition-property",
    "-webkit-column-rule-color",
    "-webkit-animation-play-state",
    "stop-opacity",
    "-webkit-text-stroke-color",
    "-epub-text-orientation",
    "font-feature-settings",
    "text-decoration-color",
    "vector-effect",
    "-webkit-mask-box-image-slice",
    "-webkit-mask-box-image-outset",
    "outline-offset",
    "-webkit-ruby-position",
    "max-width",
    "list-style-position",
    "text-transform",
    "-webkit-font-feature-settings",
    "-epub-text-combine",
    "table-layout",
    "box-shadow",
    "empty-cells",
    "flex",
    "text-shadow",
    "background-repeat-x",
    "flex-grow",
    "outline-style",
    "text-underline-position",
    "-webkit-box-ordinal-group",
    "-webkit-perspective-origin-x",
    "-webkit-transform-origin-x",
    "background-repeat-y",
    "-webkit-mask-box-image-source",
    "background-position-x",
    "-webkit-box-shadow",
    "-webkit-flex",
    "-webkit-perspective-origin-y",
    "-webkit-transform-origin-y",
    "background-position-y",
    "mask-source-type",
    "flex-basis",
    "-webkit-flex-grow",
    "-webkit-logical-height",
    "font-style",
    "text-anchor",
    "-webkit-column-rule-width",
    "stroke-dasharray",
    "-webkit-min-logical-height",
    "flex-wrap",
    "-webkit-text-stroke-width",
    "overflow-x",
    "-webkit-mask-box-image-width",
    "text-overflow",
    "transform-style",
    "scroll-snap-type",
    "-webkit-flex-basis",
    "overflow-y",
    "scroll-snap-points-x",
    "stroke-dashoffset",
    "flex-direction",
    "-webkit-flex-wrap",
    "border-right-style",
    "scroll-snap-points-y",
    "-webkit-text-emphasis",
    "-webkit-border-after-style",
    "-webkit-border-before-style",
    "-webkit-transform-style",
    "-webkit-box-reflect",
    "border-left-style",
    "-webkit-flex-direction",
    "justify-items",
    "-webkit-user-modify",
    "flood-opacity",
    "-webkit-highlight",
    "-epub-text-transform",
    "-webkit-text-emphasis-position",
    "justify-content",
    "fill-opacity",
    "-webkit-max-logical-width",
    "-webkit-justify-content",
    "max-height",
    "shape-image-threshold",
    "-webkit-text-emphasis-color",
    "-webkit-text-fill-color",
    "font-family",
    "-epub-text-emphasis",
    "backface-visibility",
    "-webkit-shape-image-threshold",
    "flex-shrink",
    "flex-flow",
    "-webkit-backface-visibility",
    "-webkit-column-rule-style",
    "glyph-orientation-vertical",
    "-webkit-flex-shrink",
    "-webkit-flex-flow",
    "text-decoration-style",
    "-webkit-tap-highlight-color",
    "list-style-type",
    "-epub-text-emphasis-color",
    "-webkit-text-decorations-in-effect",
    "-webkit-text-security",
    "justify-self",
    "-webkit-box-flex",
    "-webkit-max-logical-height",
    "text-justify",
    "glyph-orientation-horizontal",
    "-webkit-text-emphasis-style",
    "-webkit-box-flex-group",
    "-webkit-hyphenate-character",
    "-epub-text-emphasis-style"
  };
#define stringpool ((const char *) &stringpool_contents)

static const struct Property property_word_list[] =
  {
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str0, CSSPropertyR},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str1, CSSPropertyGrid},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str2, CSSPropertyOrder},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str3, CSSPropertyMargin},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str4, CSSPropertyMotion},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str5, CSSPropertyRotate},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str6, CSSPropertyBottom},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str7, CSSPropertyBorder},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str8, CSSPropertyGridRow},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str9, CSSPropertyImeMode},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str10, CSSPropertyAnimation},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str11, CSSPropertyGridArea},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str12, CSSPropertyMarker},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str13, CSSPropertyOrientation},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str14, CSSPropertyWidows},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str15, CSSPropertyWritingMode},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str16, CSSPropertyGridRowEnd},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str17, CSSPropertyBorderImage},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str18, CSSPropertyMarginBottom},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str19, CSSPropertyMask},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str20, CSSPropertyAnimationName},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str21, CSSPropertyBorderBottom},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str22, CSSPropertyMarkerMid},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str23, CSSPropertyMarkerEnd},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str24, CSSPropertyWordBreak},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str25, CSSPropertyMotionRotation},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str26, CSSPropertyImageRendering},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str27, CSSPropertyTransition},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str28, CSSPropertyStroke},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str29, CSSPropertyImageOrientation},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str30, CSSPropertyAliasWebkitOrder},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str31, CSSPropertyGridRowStart},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str32, CSSPropertyAliasWebkitAnimation},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str33, CSSPropertyTop},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str34, CSSPropertyMarkerStart},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str35, CSSPropertyPage},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str36, CSSPropertyWebkitMarginEnd},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str37, CSSPropertyWebkitBorderEnd},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str38, CSSPropertyWebkitWritingMode},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str39, CSSPropertyWebkitBorderImage},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str40, CSSPropertyPadding},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str41, CSSPropertyWebkitMask},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str42, CSSPropertyAliasWebkitAnimationName},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str43, CSSPropertyWordWrap},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str44, CSSPropertyAliasWebkitTransition},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str45, CSSPropertyMarginTop},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str46, CSSPropertyBorderTop},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str47, CSSPropertyPaintOrder},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str48, CSSPropertyWebkitMarginStart},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str49, CSSPropertyGridRowGap},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str50, CSSPropertyWebkitBorderStart},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str51, CSSPropertyZoom},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str52, CSSPropertyPosition},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str53, CSSPropertyWebkitMaskImage},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str54, CSSPropertyContent},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str55, CSSPropertyWebkitMaskOrigin},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str56, CSSPropertySpeak},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str57, CSSPropertyPaddingBottom},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str58, CSSPropertySrc},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str59, CSSPropertyDirection},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str60, CSSPropertyMinZoom},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str61, CSSPropertyIsolation},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str62, CSSPropertyTranslate},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str63, CSSPropertySize},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str64, CSSPropertyBorderImageRepeat},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str65, CSSPropertyAlignItems},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str66, CSSPropertyResize},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str67, CSSPropertyTabSize},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str68, CSSPropertyWebkitPaddingEnd},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str69, CSSPropertyAnimationDirection},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str70, CSSPropertyDominantBaseline},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str71, CSSPropertyPageBreakInside},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str72, CSSPropertyWebkitRtlOrdering},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str73, CSSPropertyStrokeLinejoin},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str74, CSSPropertyAll},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str75, CSSPropertyPaddingTop},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str76, CSSPropertyStrokeMiterlimit},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str77, CSSPropertyWebkitLineBreak},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str78, CSSPropertyWebkitPaddingStart},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str79, CSSPropertyAliasWebkitAlignItems},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str80, CSSPropertyWebkitMaskRepeat},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str81, CSSPropertyColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str82, CSSPropertyClear},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str83, CSSPropertyGridTemplate},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str84, CSSPropertyPointerEvents},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str85, CSSPropertyAliasWebkitAnimationDirection},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str86, CSSPropertyScale},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str87, CSSPropertyWebkitMaskPosition},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str88, CSSPropertyQuotes},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str89, CSSPropertyWebkitAppRegion},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str90, CSSPropertyBorderColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str91, CSSPropertyAlignContent},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str92, CSSPropertyWordSpacing},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str93, CSSPropertyCaptionSide},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str94, CSSPropertyColorRendering},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str95, CSSPropertyBorderSpacing},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str96, CSSPropertyWebkitMaskSize},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str97, CSSPropertyGridTemplateRows},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str98, CSSPropertyObjectPosition},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str99, CSSPropertyAnimationDuration},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str100, CSSPropertyGridTemplateAreas},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str101, CSSPropertyBorderRadius},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str102, CSSPropertyGridAutoRows},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str103, CSSPropertyBorderBottomColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str104, CSSPropertyAlignmentBaseline},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str105, CSSPropertyClip},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str106, CSSPropertyBorderImageSlice},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str107, CSSPropertyTransitionDuration},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str108, CSSPropertyBorderImageOutset},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str109, CSSPropertyAliasWebkitAlignContent},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str110, CSSPropertyWebkitUserDrag},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str111, CSSPropertyOutline},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str112, CSSPropertyWebkitBorderEndColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str113, CSSPropertyAliasWebkitAnimationDuration},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str114, CSSPropertyAliasWebkitBorderRadius},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str115, CSSPropertyStopColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str116, CSSPropertyBorderTopColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str117, CSSPropertyCursor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str118, CSSPropertyWebkitBorderStartColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str119, CSSPropertyAliasEpubWritingMode},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str120, CSSPropertyAliasWebkitTransitionDuration},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str121, CSSPropertyWebkitAppearance},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str122, CSSPropertyWebkitMaskComposite},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str123, CSSPropertyUnicodeBidi},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str124, CSSPropertyLetterSpacing},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str125, CSSPropertyUnicodeRange},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str126, CSSPropertyAliasEpubWordBreak},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str127, CSSPropertyBackground},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str128, CSSPropertyWebkitLocale},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str129, CSSPropertyFont},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str130, CSSPropertyUserZoom},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str131, CSSPropertyStrokeLinecap},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str132, CSSPropertyCounterReset},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str133, CSSPropertyBackgroundImage},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str134, CSSPropertyBackgroundOrigin},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str135, CSSPropertyWebkitMaskClip},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str136, CSSPropertyPerspective},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str137, CSSPropertyAnimationIterationCount},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str138, CSSPropertyBorderImageSource},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str139, CSSPropertyTransform},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str140, CSSPropertyRight},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str141, CSSPropertyWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str142, CSSPropertyFontKerning},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str143, CSSPropertyVerticalAlign},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str144, CSSPropertyPerspectiveOrigin},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str145, CSSPropertyMinWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str146, CSSPropertyGridColumn},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str147, CSSPropertyTransformOrigin},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str148, CSSPropertyWebkitBackgroundOrigin},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str149, CSSPropertyMarginRight},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str150, CSSPropertyColorInterpolation},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str151, CSSPropertyBorderRight},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str152, CSSPropertyBorderWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str153, CSSPropertyBorderCollapse},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str154, CSSPropertyWebkitMarginAfter},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str155, CSSPropertyAliasWebkitPerspective},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str156, CSSPropertyAliasWebkitAnimationIterationCount},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str157, CSSPropertyGridColumnEnd},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str158, CSSPropertyWebkitBorderAfter},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str159, CSSPropertyWebkitLineClamp},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str160, CSSPropertyWebkitMarginBefore},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str161, CSSPropertyLeft},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str162, CSSPropertyWebkitBorderBefore},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str163, CSSPropertyAliasWebkitTransform},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str164, CSSPropertyFloat},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str165, CSSPropertyBackgroundRepeat},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str166, CSSPropertyFilter},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str167, CSSPropertyBorderImageWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str168, CSSPropertyFontVariant},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str169, CSSPropertyCounterIncrement},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str170, CSSPropertyBorderBottomWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str171, CSSPropertyAliasWebkitPerspectiveOrigin},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str172, CSSPropertyEnableBackground},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str173, CSSPropertyStrokeWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str174, CSSPropertyGridColumnStart},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str175, CSSPropertyMarginLeft},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str176, CSSPropertyAliasWebkitTransformOrigin},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str177, CSSPropertyBorderLeft},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str178, CSSPropertyWebkitColumns},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str179, CSSPropertyBackgroundPosition},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str180, CSSPropertyWebkitMarginCollapse},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str181, CSSPropertyObjectFit},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str182, CSSPropertyBackgroundBlendMode},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str183, CSSPropertyPageBreakAfter},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str184, CSSPropertyMotionPath},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str185, CSSPropertyWebkitBorderEndWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str186, CSSPropertyBackgroundSize},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str187, CSSPropertyPageBreakBefore},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str188, CSSPropertyOrphans},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str189, CSSPropertyWebkitFilter},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str190, CSSPropertyPaddingRight},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str191, CSSPropertyFontSize},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str192, CSSPropertyScrollSnapDestination},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str193, CSSPropertyGridColumnGap},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str194, CSSPropertyWebkitMarginBottomCollapse},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str195, CSSPropertyWebkitPaddingAfter},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str196, CSSPropertyWebkitUserSelect},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str197, CSSPropertyOutlineColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str198, CSSPropertyBorderTopWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str199, CSSPropertyShapeMargin},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str200, CSSPropertyWebkitPaddingBefore},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str201, CSSPropertyWebkitBorderStartWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str202, CSSPropertyOverflow},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str203, CSSPropertyFill},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str204, CSSPropertyAliasEpubCaptionSide},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str205, CSSPropertyShapeRendering},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str206, CSSPropertyWebkitColumnGap},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str207, CSSPropertyWebkitColumnBreakInside},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str208, CSSPropertyAliasWebkitBackgroundSize},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str209, CSSPropertyPaddingLeft},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str210, CSSPropertyBackgroundColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str211, CSSPropertyWebkitColumnSpan},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str212, CSSPropertyAliasWebkitShapeMargin},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str213, CSSPropertyWebkitMarginTopCollapse},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str214, CSSPropertyClipRule},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str215, CSSPropertyAlignSelf},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str216, CSSPropertyScrollSnapCoordinate},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str217, CSSPropertyWebkitTransformOriginZ},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str218, CSSPropertyScrollBlocksOn},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str219, CSSPropertyAnimationFillMode},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str220, CSSPropertyWebkitPrintColorAdjust},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str221, CSSPropertyWebkitBorderVerticalSpacing},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str222, CSSPropertyWhiteSpace},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str223, CSSPropertyOverflowWrap},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str224, CSSPropertyAliasWebkitAlignSelf},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str225, CSSPropertyBackgroundClip},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str226, CSSPropertyX},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str227, CSSPropertyWebkitBackgroundComposite},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str228, CSSPropertyRx},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str229, CSSPropertyBorderRightColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str230, CSSPropertyAliasWebkitAnimationFillMode},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str231, CSSPropertyGridTemplateColumns},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str232, CSSPropertyWebkitBorderAfterColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str233, CSSPropertyY},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str234, CSSPropertyGridAutoColumns},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str235, CSSPropertyWebkitBorderBeforeColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str236, CSSPropertyRy},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str237, CSSPropertyFloodColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str238, CSSPropertyWebkitFontSizeDelta},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str239, CSSPropertyGridAutoFlow},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str240, CSSPropertyTextIndent},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str241, CSSPropertyWebkitBackgroundClip},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str242, CSSPropertyBackdropFilter},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str243, CSSPropertyTextRendering},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str244, CSSPropertyBorderLeftColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str245, CSSPropertyBorderBottomRightRadius},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str246, CSSPropertyWillChange},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str247, CSSPropertyOutlineWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str248, CSSPropertyLightingColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str249, CSSPropertyWebkitBoxOrient},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str250, CSSPropertyAnimationTimingFunction},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str251, CSSPropertyCx},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str252, CSSPropertyShapeOutside},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str253, CSSPropertyWebkitTextOrientation},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str254, CSSPropertyBorderBottomLeftRadius},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str255, CSSPropertyAliasWebkitBorderBottomRightRadius},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str256, CSSPropertyFontSizeAdjust},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str257, CSSPropertyWebkitColumnRule},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str258, CSSPropertyClipPath},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str259, CSSPropertyMotionOffset},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str260, CSSPropertyTransitionTimingFunction},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str261, CSSPropertyTextAlign},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str262, CSSPropertyBorderTopRightRadius},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str263, CSSPropertyWebkitLogicalWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str264, CSSPropertyCy},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str265, CSSPropertyWebkitTextStroke},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str266, CSSPropertyFontWeight},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str267, CSSPropertyWebkitColumnCount},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str268, CSSPropertyZIndex},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str269, CSSPropertyMaxZoom},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str270, CSSPropertyWebkitMaskBoxImage},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str271, CSSPropertyWebkitMinLogicalWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str272, CSSPropertyMixBlendMode},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str273, CSSPropertyAliasWebkitAnimationTimingFunction},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str274, CSSPropertyHeight},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str275, CSSPropertyAliasWebkitShapeOutside},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str276, CSSPropertyTextDecoration},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str277, CSSPropertyAliasWebkitBorderBottomLeftRadius},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str278, CSSPropertyMinHeight},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str279, CSSPropertyFontVariantLigatures},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str280, CSSPropertyBoxSizing},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str281, CSSPropertyMaskType},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str282, CSSPropertyAnimationDelay},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str283, CSSPropertyWebkitClipPath},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str284, CSSPropertyWebkitBoxAlign},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str285, CSSPropertyAliasWebkitTransitionTimingFunction},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str286, CSSPropertyBorderTopLeftRadius},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str287, CSSPropertyAliasWebkitBorderTopRightRadius},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str288, CSSPropertyScrollBehavior},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str289, CSSPropertyBorderStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str290, CSSPropertyTouchAction},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str291, CSSPropertyWebkitBoxLines},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str292, CSSPropertyTransitionDelay},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str293, CSSPropertyWebkitTextCombine},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str294, CSSPropertyFillRule},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str295, CSSPropertyWebkitBoxDirection},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str296, CSSPropertyBorderRightWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str297, CSSPropertyWebkitMaskRepeatX},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str298, CSSPropertyBorderBottomStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str299, CSSPropertyWebkitBorderAfterWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str300, CSSPropertyWebkitFontSmoothing},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str301, CSSPropertyWebkitMarginAfterCollapse},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str302, CSSPropertyAliasWebkitBoxSizing},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str303, CSSPropertyWebkitBorderBeforeWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str304, CSSPropertyAliasWebkitAnimationDelay},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str305, CSSPropertyWebkitMarginBeforeCollapse},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str306, CSSPropertyOpacity},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str307, CSSPropertyAliasWebkitBorderTopLeftRadius},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str308, CSSPropertyWebkitColumnWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str309, CSSPropertyWebkitColumnBreakAfter},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str310, CSSPropertyWebkitColumnBreakBefore},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str311, CSSPropertyDisplay},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str312, CSSPropertyWebkitMaskRepeatY},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str313, CSSPropertyWebkitMaskPositionX},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str314, CSSPropertyBackgroundAttachment},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str315, CSSPropertyWebkitMaskBoxImageRepeat},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str316, CSSPropertyVisibility},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str317, CSSPropertyAliasWebkitTransitionDelay},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str318, CSSPropertyWebkitBoxDecorationBreak},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str319, CSSPropertyBorderLeftWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str320, CSSPropertyWebkitBorderEndStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str321, CSSPropertyTextAlignLast},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str322, CSSPropertyFontStretch},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str323, CSSPropertyLineHeight},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str324, CSSPropertyWebkitMaskPositionY},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str325, CSSPropertyTransitionProperty},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str326, CSSPropertyTextDecorationLine},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str327, CSSPropertyWebkitBoxPack},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str328, CSSPropertyBorderTopStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str329, CSSPropertyAliasWebkitOpacity},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str330, CSSPropertyBaselineShift},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str331, CSSPropertyColorInterpolationFilters},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str332, CSSPropertyWebkitBorderStartStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str333, CSSPropertyListStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str334, CSSPropertyAnimationPlayState},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str335, CSSPropertyStrokeOpacity},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str336, CSSPropertyWebkitBorderHorizontalSpacing},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str337, CSSPropertyWebkitLineBoxContain},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str338, CSSPropertyListStyleImage},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str339, CSSPropertyColumnFill},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str340, CSSPropertyBufferedRendering},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str341, CSSPropertyAliasWebkitTransitionProperty},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str342, CSSPropertyWebkitColumnRuleColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str343, CSSPropertyAliasWebkitAnimationPlayState},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str344, CSSPropertyStopOpacity},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str345, CSSPropertyWebkitTextStrokeColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str346, CSSPropertyAliasEpubTextOrientation},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str347, CSSPropertyFontFeatureSettings},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str348, CSSPropertyTextDecorationColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str349, CSSPropertyVectorEffect},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str350, CSSPropertyWebkitMaskBoxImageSlice},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str351, CSSPropertyWebkitMaskBoxImageOutset},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str352, CSSPropertyOutlineOffset},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str353, CSSPropertyWebkitRubyPosition},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str354, CSSPropertyMaxWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str355, CSSPropertyListStylePosition},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str356, CSSPropertyTextTransform},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str357, CSSPropertyWebkitFontFeatureSettings},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str358, CSSPropertyAliasEpubTextCombine},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str359, CSSPropertyTableLayout},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str360, CSSPropertyBoxShadow},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str361, CSSPropertyEmptyCells},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str362, CSSPropertyFlex},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str363, CSSPropertyTextShadow},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str364, CSSPropertyBackgroundRepeatX},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str365, CSSPropertyFlexGrow},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str366, CSSPropertyOutlineStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str367, CSSPropertyTextUnderlinePosition},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str368, CSSPropertyWebkitBoxOrdinalGroup},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str369, CSSPropertyWebkitPerspectiveOriginX},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str370, CSSPropertyWebkitTransformOriginX},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str371, CSSPropertyBackgroundRepeatY},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str372, CSSPropertyWebkitMaskBoxImageSource},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str373, CSSPropertyBackgroundPositionX},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str374, CSSPropertyAliasWebkitBoxShadow},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str375, CSSPropertyAliasWebkitFlex},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str376, CSSPropertyWebkitPerspectiveOriginY},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str377, CSSPropertyWebkitTransformOriginY},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str378, CSSPropertyBackgroundPositionY},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str379, CSSPropertyMaskSourceType},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str380, CSSPropertyFlexBasis},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str381, CSSPropertyAliasWebkitFlexGrow},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str382, CSSPropertyWebkitLogicalHeight},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str383, CSSPropertyFontStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str384, CSSPropertyTextAnchor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str385, CSSPropertyWebkitColumnRuleWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str386, CSSPropertyStrokeDasharray},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str387, CSSPropertyWebkitMinLogicalHeight},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str388, CSSPropertyFlexWrap},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str389, CSSPropertyWebkitTextStrokeWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str390, CSSPropertyOverflowX},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str391, CSSPropertyWebkitMaskBoxImageWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str392, CSSPropertyTextOverflow},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str393, CSSPropertyTransformStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str394, CSSPropertyScrollSnapType},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str395, CSSPropertyAliasWebkitFlexBasis},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str396, CSSPropertyOverflowY},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str397, CSSPropertyScrollSnapPointsX},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str398, CSSPropertyStrokeDashoffset},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str399, CSSPropertyFlexDirection},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str400, CSSPropertyAliasWebkitFlexWrap},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str401, CSSPropertyBorderRightStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str402, CSSPropertyScrollSnapPointsY},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str403, CSSPropertyWebkitTextEmphasis},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str404, CSSPropertyWebkitBorderAfterStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str405, CSSPropertyWebkitBorderBeforeStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str406, CSSPropertyAliasWebkitTransformStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str407, CSSPropertyWebkitBoxReflect},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str408, CSSPropertyBorderLeftStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str409, CSSPropertyAliasWebkitFlexDirection},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str410, CSSPropertyJustifyItems},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str411, CSSPropertyWebkitUserModify},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str412, CSSPropertyFloodOpacity},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str413, CSSPropertyWebkitHighlight},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str414, CSSPropertyAliasEpubTextTransform},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str415, CSSPropertyWebkitTextEmphasisPosition},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str416, CSSPropertyJustifyContent},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str417, CSSPropertyFillOpacity},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str418, CSSPropertyWebkitMaxLogicalWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str419, CSSPropertyAliasWebkitJustifyContent},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str420, CSSPropertyMaxHeight},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str421, CSSPropertyShapeImageThreshold},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str422, CSSPropertyWebkitTextEmphasisColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str423, CSSPropertyWebkitTextFillColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str424, CSSPropertyFontFamily},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str425, CSSPropertyAliasEpubTextEmphasis},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str426, CSSPropertyBackfaceVisibility},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str427, CSSPropertyAliasWebkitShapeImageThreshold},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str428, CSSPropertyFlexShrink},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str429, CSSPropertyFlexFlow},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str430, CSSPropertyAliasWebkitBackfaceVisibility},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str431, CSSPropertyWebkitColumnRuleStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str432, CSSPropertyGlyphOrientationVertical},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str433, CSSPropertyAliasWebkitFlexShrink},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str434, CSSPropertyAliasWebkitFlexFlow},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str435, CSSPropertyTextDecorationStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str436, CSSPropertyWebkitTapHighlightColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str437, CSSPropertyListStyleType},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str438, CSSPropertyAliasEpubTextEmphasisColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str439, CSSPropertyWebkitTextDecorationsInEffect},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str440, CSSPropertyWebkitTextSecurity},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str441, CSSPropertyJustifySelf},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str442, CSSPropertyWebkitBoxFlex},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str443, CSSPropertyWebkitMaxLogicalHeight},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str444, CSSPropertyTextJustify},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str445, CSSPropertyGlyphOrientationHorizontal},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str446, CSSPropertyWebkitTextEmphasisStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str447, CSSPropertyWebkitBoxFlexGroup},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str448, CSSPropertyWebkitHyphenateCharacter},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str449, CSSPropertyAliasEpubTextEmphasisStyle}
  };

static const short lookup[] =
  {
     -1,  -1,  -1,  -1,  -1,   0,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,   1,  -1,  -1,  -1,  -1,  -1,   2,  -1,
      3,   4,  -1,   5,   6,   7,  -1,  -1,  -1,  -1,
     -1,  -1,   8,  -1,   9,  -1,  10,  -1,  11,  -1,
     -1,  -1,  -1,  12,  -1,  -1,  -1,  -1,  13,  -1,
     -1,  14,  -1,  -1,  15,  16,  17,  -1,  -1,  18,
     -1,  19,  20,  -1,  21,  22,  23,  24,  25,  -1,
     26,  27,  -1,  28,  -1,  -1,  -1,  -1,  -1,  29,
     -1,  -1,  -1,  -1,  30,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  31,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  32,  33,  34,  -1,  -1,  -1,  35,  36,
     -1,  -1,  -1,  -1,  37,  -1,  -1,  -1,  -1,  -1,
     38,  -1,  39,  -1,  -1,  -1,  40,  41,  42,  -1,
     -1,  -1,  -1,  -1,  43,  -1,  -1,  44,  45,  -1,
     -1,  -1,  -1,  46,  -1,  -1,  47,  48,  -1,  49,
     -1,  -1,  50,  -1,  -1,  51,  -1,  52,  53,  -1,
     -1,  54,  55,  -1,  56,  57,  -1,  58,  -1,  -1,
     -1,  -1,  59,  -1,  -1,  60,  -1,  -1,  -1,  -1,
     -1,  -1,  61,  -1,  62,  63,  -1,  -1,  -1,  -1,
     -1,  64,  -1,  65,  -1,  -1,  66,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  67,  -1,
     -1,  -1,  -1,  -1,  -1,  68,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  69,  -1,  -1,  -1,  -1,  70,  -1,
     71,  -1,  -1,  -1,  -1,  -1,  -1,  72,  73,  74,
     -1,  -1,  -1,  -1,  75,  -1,  -1,  -1,  76,  77,
     -1,  -1,  -1,  78,  -1,  -1,  -1,  -1,  -1,  79,
     -1,  -1,  80,  -1,  -1,  81,  82,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  83,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  84,  -1,  -1,  85,
     -1,  -1,  -1,  -1,  -1,  86,  -1,  -1,  -1,  87,
     88,  89,  -1,  -1,  -1,  90,  -1,  -1,  91,  -1,
     -1,  92,  -1,  93,  -1,  -1,  -1,  -1,  -1,  94,
     -1,  -1,  -1,  -1,  95,  -1,  -1,  96,  -1,  97,
     -1,  98,  -1,  99,  -1, 100,  -1,  -1,  -1,  -1,
    101, 102,  -1,  -1, 103,  -1,  -1,  -1,  -1, 104,
     -1,  -1,  -1,  -1,  -1,  -1,  -1, 105,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1, 106,  -1, 107,  -1,
     -1,  -1, 108,  -1, 109,  -1,  -1,  -1,  -1,  -1,
     -1,  -1, 110,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1, 111, 112,  -1,  -1,  -1,  -1, 113,
     -1,  -1,  -1,  -1,  -1,  -1, 114,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1, 115,  -1,  -1,
     -1,  -1,  -1, 116,  -1,  -1,  -1,  -1,  -1,  -1,
     -1, 117, 118, 119, 120, 121,  -1,  -1, 122, 123,
    124, 125,  -1,  -1,  -1,  -1, 126,  -1, 127,  -1,
     -1,  -1,  -1,  -1, 128,  -1,  -1,  -1, 129, 130,
     -1, 131,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1, 132,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 133,
     -1,  -1,  -1, 134,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 135,
    136,  -1, 137, 138,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1, 139,  -1,  -1, 140,  -1, 141,  -1,  -1,
     -1, 142,  -1,  -1, 143,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1, 144,  -1, 145,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1, 146, 147,  -1, 148,
    149,  -1, 150,  -1,  -1, 151,  -1, 152,  -1,  -1,
     -1,  -1,  -1,  -1, 153, 154, 155,  -1, 156, 157,
    158, 159, 160,  -1,  -1,  -1, 161, 162, 163,  -1,
    164,  -1,  -1, 165,  -1,  -1, 166,  -1, 167,  -1,
     -1,  -1,  -1,  -1, 168, 169, 170,  -1,  -1,  -1,
     -1, 171,  -1,  -1, 172, 173,  -1, 174,  -1,  -1,
     -1, 175,  -1, 176,  -1,  -1, 177,  -1,  -1, 178,
    179,  -1,  -1,  -1,  -1, 180,  -1, 181, 182,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1, 183, 184,  -1,  -1, 185,  -1, 186, 187,
    188,  -1, 189,  -1,  -1,  -1, 190,  -1, 191,  -1,
     -1, 192,  -1, 193, 194,  -1,  -1,  -1,  -1,  -1,
     -1, 195, 196, 197,  -1, 198, 199,  -1, 200,  -1,
     -1,  -1,  -1,  -1, 201,  -1,  -1,  -1,  -1,  -1,
    202, 203, 204,  -1,  -1, 205,  -1,  -1,  -1,  -1,
     -1,  -1, 206,  -1,  -1,  -1,  -1, 207,  -1,  -1,
     -1,  -1,  -1,  -1, 208,  -1,  -1, 209,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 210,  -1,
     -1,  -1,  -1,  -1,  -1,  -1, 211,  -1,  -1,  -1,
     -1,  -1, 212, 213,  -1,  -1,  -1,  -1,  -1, 214,
     -1, 215,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 216,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 217, 218,
    219,  -1,  -1,  -1,  -1,  -1,  -1, 220,  -1,  -1,
     -1,  -1, 221,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1, 222,  -1,  -1,  -1,
     -1,  -1, 223,  -1,  -1,  -1,  -1, 224,  -1,  -1,
    225, 226,  -1,  -1,  -1, 227, 228,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1, 229, 230,  -1,  -1,  -1,
     -1,  -1,  -1, 231,  -1,  -1,  -1,  -1,  -1,  -1,
    232,  -1,  -1, 233,  -1, 234,  -1, 235, 236,  -1,
     -1, 237,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1, 238, 239,  -1, 240,  -1,
     -1,  -1,  -1,  -1,  -1,  -1, 241, 242,  -1,  -1,
     -1,  -1,  -1, 243,  -1,  -1, 244,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 245,
     -1,  -1, 246,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1, 247,  -1, 248,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1, 249,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1, 250,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 251,
     -1,  -1,  -1, 252,  -1,  -1,  -1,  -1, 253,  -1,
    254,  -1,  -1,  -1,  -1, 255, 256, 257,  -1, 258,
     -1, 259, 260,  -1,  -1,  -1, 261,  -1, 262,  -1,
    263, 264,  -1, 265, 266,  -1,  -1,  -1, 267, 268,
     -1, 269,  -1,  -1,  -1,  -1, 270,  -1,  -1,  -1,
    271, 272,  -1, 273,  -1,  -1,  -1,  -1,  -1,  -1,
    274,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 275,
     -1, 276,  -1,  -1,  -1,  -1, 277,  -1,  -1,  -1,
    278, 279, 280, 281, 282, 283, 284,  -1, 285, 286,
     -1,  -1,  -1,  -1, 287,  -1,  -1,  -1, 288,  -1,
     -1, 289,  -1,  -1,  -1,  -1, 290,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1, 291,  -1,  -1, 292,
     -1, 293,  -1, 294,  -1,  -1, 295, 296,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 297,  -1,
    298,  -1, 299, 300, 301,  -1,  -1,  -1, 302, 303,
    304, 305,  -1,  -1, 306, 307,  -1, 308,  -1, 309,
     -1,  -1,  -1,  -1,  -1,  -1, 310,  -1, 311,  -1,
    312,  -1,  -1,  -1,  -1, 313,  -1,  -1,  -1, 314,
     -1, 315, 316,  -1,  -1, 317, 318,  -1, 319,  -1,
    320,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 321,  -1,
     -1,  -1, 322, 323,  -1,  -1,  -1, 324,  -1,  -1,
    325,  -1,  -1,  -1, 326,  -1, 327,  -1,  -1, 328,
    329,  -1,  -1,  -1,  -1,  -1, 330, 331, 332,  -1,
     -1,  -1,  -1, 333,  -1, 334,  -1,  -1,  -1,  -1,
     -1,  -1, 335,  -1,  -1,  -1,  -1, 336,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1, 337,  -1,  -1,  -1,
     -1,  -1,  -1,  -1, 338, 339,  -1,  -1,  -1,  -1,
     -1,  -1,  -1, 340,  -1,  -1, 341,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1, 342,  -1,  -1,
     -1, 343,  -1,  -1,  -1,  -1, 344,  -1,  -1,  -1,
     -1,  -1,  -1, 345,  -1,  -1,  -1,  -1,  -1,  -1,
     -1, 346,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1, 347,  -1,  -1,  -1,  -1,  -1,  -1,
     -1, 348,  -1,  -1,  -1, 349, 350,  -1,  -1,  -1,
     -1,  -1, 351,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1, 352,  -1,  -1,  -1, 353,  -1,  -1,
     -1,  -1,  -1, 354,  -1, 355, 356,  -1,  -1, 357,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1, 358,  -1, 359,  -1, 360, 361,
     -1, 362,  -1,  -1, 363,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 364,
     -1,  -1,  -1,  -1,  -1,  -1, 365,  -1,  -1, 366,
     -1, 367,  -1,  -1, 368,  -1,  -1, 369,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 370,
     -1, 371,  -1, 372,  -1,  -1, 373,  -1,  -1,  -1,
     -1,  -1,  -1,  -1, 374,  -1,  -1, 375,  -1, 376,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1, 377,  -1,  -1,  -1,  -1,  -1,  -1, 378,  -1,
    379, 380, 381, 382, 383,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1, 384,  -1,  -1, 385,
     -1, 386,  -1, 387,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1, 388,  -1, 389, 390,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 391,  -1,
     -1,  -1,  -1,  -1, 392,  -1,  -1,  -1, 393, 394,
     -1,  -1,  -1,  -1,  -1,  -1,  -1, 395, 396,  -1,
     -1,  -1,  -1,  -1,  -1, 397,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1, 398,  -1,  -1,  -1,  -1, 399, 400,
     -1, 401,  -1,  -1,  -1,  -1,  -1, 402,  -1,  -1,
     -1,  -1,  -1,  -1,  -1, 403, 404,  -1,  -1,  -1,
     -1,  -1,  -1, 405, 406,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 407,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1, 408,  -1, 409,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1, 410,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1, 411,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    412,  -1,  -1,  -1,  -1,  -1, 413,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1, 414,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1, 415,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1, 416,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    417,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1, 418,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1, 419,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1, 420,  -1,  -1,  -1,
    421,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1, 422,  -1,  -1,  -1,  -1,
     -1, 423,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1, 424,  -1,  -1, 425,  -1,
     -1,  -1,  -1,  -1,  -1, 426,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1, 427,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1, 428,  -1,  -1,  -1,
    429, 430,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1, 431,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1, 432,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1, 433,  -1,  -1,  -1, 434,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1, 435,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1, 436,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1, 437,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 438,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1, 439,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1, 440,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    441,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1, 442,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 443,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1, 444, 445,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1, 446,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1, 447,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1, 448,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1, 449
  };

const struct Property *
CSSPropertyNamesHash::findPropertyImpl (register const char *str, register unsigned int len)
{
  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = property_hash_function (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register int index = lookup[key];

          if (index >= 0)
            {
              register const char *s = property_word_list[index].nameOffset + stringpool;

              if (*str == *s && !strncmp (str + 1, s + 1, len - 1) && s[len] == '\0')
                return &property_word_list[index];
            }
        }
    }
  return 0;
}

const Property* findProperty(register const char* str, register unsigned int len)
{
    return CSSPropertyNamesHash::findPropertyImpl(str, len);
}

const char* getPropertyName(CSSPropertyID id)
{
    if (CSSPropertyVariable == id)
        return "variable";

    ASSERT(id >= firstCSSProperty && id <= lastUnresolvedCSSProperty);
    int index = id - firstCSSProperty;
    return propertyNameStringsPool + propertyNameStringsOffsets[index];
}

const AtomicString& getPropertyNameAtomicString(CSSPropertyID id)
{
    ASSERT(id >= firstCSSProperty && id <= lastUnresolvedCSSProperty);
    int index = id - firstCSSProperty;
    static AtomicString* propertyStrings = new AtomicString[lastUnresolvedCSSProperty]; // Intentionally never destroyed.
    AtomicString& propertyString = propertyStrings[index];
    if (propertyString.isNull()) {
        const char* propertyName = propertyNameStringsPool + propertyNameStringsOffsets[index];
        propertyString = AtomicString(propertyName, strlen(propertyName), AtomicString::ConstructFromLiteral);
    }
    return propertyString;
}

String getPropertyNameString(CSSPropertyID id)
{
    // We share the StringImpl with the AtomicStrings.
    return getPropertyNameAtomicString(id).string();
}

String getJSPropertyName(CSSPropertyID id)
{
    char result[maxCSSPropertyNameLength + 1];
    const char* cssPropertyName = getPropertyName(id);
    const char* propertyNamePointer = cssPropertyName;
    if (!propertyNamePointer)
        return emptyString();

    char* resultPointer = result;
    while (char character = *propertyNamePointer++) {
        if (character == '-') {
            char nextCharacter = *propertyNamePointer++;
            if (!nextCharacter)
                break;
            character = (propertyNamePointer - 2 != cssPropertyName) ? toASCIIUpper(nextCharacter) : nextCharacter;
        }
        *resultPointer++ = character;
    }
    *resultPointer = '\0';
    return String(result);
}

CSSPropertyID cssPropertyID(const String& string)
{
    return resolveCSSPropertyID(unresolvedCSSPropertyID(string));
}

} // namespace blink
