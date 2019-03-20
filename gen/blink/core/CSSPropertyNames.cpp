/* C++ code produced by gperf version 3.0.1 */
/* Command-line: ../../../../third_party/gperf/bin/gperf.exe --key-positions='*' -P -n -m 50 -D  */

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
    "-webkit-font-smoothing\0"
    "-webkit-locale\0"
    "text-orientation\0"
    "-webkit-text-orientation\0"
    "writing-mode\0"
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
    "grid-auto-columns\0"
    "grid-auto-flow\0"
    "grid-auto-rows\0"
    "grid-column-end\0"
    "grid-column-gap\0"
    "grid-column-start\0"
    "grid-row-end\0"
    "grid-row-gap\0"
    "grid-row-start\0"
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
    "text-combine-upright\0"
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
    "-webkit-caret-color\0"
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
    "-bb-rubberbandable\0"
    "white-space\0"
    "widows\0"
    "width\0"
    "will-change\0"
    "word-break\0"
    "word-spacing\0"
    "word-wrap\0"
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
    "grid-gap\0"
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
    "-webkit-font-feature-settings\0"
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
    185,
    200,
    217,
    242,
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
    1616,
    1631,
    1646,
    1662,
    1678,
    1696,
    1709,
    1722,
    1737,
    1757,
    1779,
    1798,
    1805,
    1821,
    1839,
    1849,
    1865,
    1879,
    1892,
    1897,
    1912,
    1927,
    1939,
    1956,
    1976,
    1992,
    2006,
    2018,
    2031,
    2042,
    2053,
    2064,
    2077,
    2082,
    2099,
    2109,
    2120,
    2130,
    2141,
    2151,
    2166,
    2180,
    2192,
    2208,
    2219,
    2235,
    2243,
    2249,
    2257,
    2271,
    2286,
    2300,
    2314,
    2328,
    2339,
    2350,
    2365,
    2378,
    2392,
    2404,
    2421,
    2439,
    2457,
    2469,
    2481,
    2500,
    2515,
    2524,
    2531,
    2538,
    2544,
    2546,
    2549,
    2552,
    2568,
    2585,
    2606,
    2627,
    2651,
    2674,
    2696,
    2709,
    2723,
    2739,
    2744,
    2750,
    2761,
    2774,
    2781,
    2798,
    2816,
    2831,
    2847,
    2865,
    2880,
    2893,
    2906,
    2915,
    2926,
    2942,
    2954,
    2975,
    2991,
    3013,
    3034,
    3056,
    3068,
    3081,
    3095,
    3107,
    3122,
    3146,
    3150,
    3163,
    3173,
    3190,
    3206,
    3216,
    3223,
    3229,
    3246,
    3266,
    3286,
    3313,
    3326,
    3340,
    3355,
    3366,
    3368,
    3370,
    3389,
    3408,
    3432,
    3461,
    3487,
    3521,
    3542,
    3574,
    3592,
    3621,
    3643,
    3660,
    3683,
    3701,
    3727,
    3746,
    3763,
    3783,
    3803,
    3821,
    3848,
    3876,
    3904,
    3925,
    3944,
    3970,
    3996,
    4022,
    4042,
    4063,
    4078,
    4096,
    4124,
    4143,
    4162,
    4192,
    4223,
    4254,
    4282,
    4312,
    4342,
    4371,
    4401,
    4430,
    4448,
    4471,
    4490,
    4510,
    4534,
    4558,
    4580,
    4602,
    4620,
    4649,
    4678,
    4705,
    4726,
    4748,
    4776,
    4797,
    4825,
    4856,
    4884,
    4908,
    4930,
    4956,
    4982,
    5009,
    5036,
    5063,
    5081,
    5101,
    5121,
    5140,
    5152,
    5159,
    5165,
    5177,
    5188,
    5201,
    5211,
    5219,
    5244,
    5269,
    5294,
    5321,
    5348,
    5375,
    5403,
    5431,
    5459,
    5486,
    5513,
    5540,
    5559,
    5580,
    5602,
    5623,
    5643,
    5665,
    5688,
    5710,
    5732,
    5755,
    5781,
    5808,
    5834,
    5861,
    5865,
    5874,
    5883,
    5895,
    5900,
    5904,
    5918,
    5928,
    5952,
    5987,
    5997,
    6008,
    6028,
    6046,
    6053,
    6067,
    6080,
    6093,
    6105,
    6119,
    6132,
    6147,
    6160,
    6171,
    6184,
    6189,
    6199,
    6204,
    6209,
    6219,
    6231,
    6240,
    6249,
    6263,
    6274,
    6281,
    6288,
    6295,
    6303,
    6312,
    6320,
    6331,
    6352,
    6374,
    6393,
    6414,
    6434,
    6450,
    6474,
    6487,
    6510,
    6532,
    6552,
    6574,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6594,
    6624,
    6624,
    6624,
    6624,
    6647,
    6647,
    6666,
    6666,
    6666,
    6688,
    6708,
    6708,
    6727,
    6751,
    6779,
    6806,
    6834,
    6868,
    6891,
    6920,
    6954,
    6954,
    6982,
    6982,
    6982,
    6982,
    6982,
    6982,
    6982,
    6982,
    6982,
    6982,
    6982,
    7006,
    7006,
    7006,
    7040,
    7075,
    7075,
    7075,
    7075,
    7075,
    7075,
    7075,
    7075,
    7075,
    7075,
    7075,
    7075,
    7075,
    7075,
    7075,
    7075,
    7106,
    7138,
    7138,
    7138,
    7138,
    7157,
    7176,
    7176,
    7195,
    7195,
    7195,
    7195,
    7195,
    7195,
    7195,
    7195,
    7195,
    7195,
    7195,
    7195,
    7195,
    7195,
    7195,
    7195,
    7195,
    7195,
    7195,
    7195,
    7195,
    7195,
    7214,
    7237,
    7255,
    7275,
    7293,
    7293,
    7293,
    7293,
    7293,
    7293,
    7293,
    7293,
    7293,
    7293,
    7293,
    7293,
    7293,
    7293,
    7293,
    7293,
    7293,
    7293,
    7293,
    7293,
    7317,
    7317,
    7317,
    7317,
    7317,
    7317,
    7317,
    7317,
    7317,
    7317,
    7317,
    7317,
    7317,
    7317,
    7317,
    7317,
    7317,
    7317,
    7317,
    7317,
    7317,
    7317,
    7317,
    7317,
    7317,
    7317,
    7317,
    7317,
    7317,
    7317,
    7333,
    7347,
    7347,
    7347,
    7347,
    7347,
    7347,
    7347,
    7347,
    7347,
    7347,
    7347,
    7347,
    7347,
    7347,
    7347,
    7347,
    7347,
    7367,
    7394,
    7394,
    7394,
    7394,
    7394,
    7394,
    7394,
    7394,
    7394,
    7394,
    7394,
    7394,
    7394,
    7394,
    7394,
    7424,
    7445,
    7467,
    7467,
    7467,
    7467,
    7467,
    7467,
    7467,
    7467,
    7467,
    7467,
    7467,
    7467,
    7467,
    7467,
    7467,
    7467,
    7467,
    7467,
    7467,
    7467,
    7467,
    7467,
    7467,
    7467,
    7467,
    7467,
    7467,
    7467,
    7488,
    7488,
    7488,
    7488,
    7506,
    7531,
    7555,
    7555,
    7555,
    7555,
    7580,
    7608,
    7636,
    7671,
    7671,
    7671,
    7671,
    7671,
    7671,
    7671,
    7671,
    7671,
    7671,
    7671,
    7671,
    7671,
    7671,
    7671,
    7671,
    7671,
    7671,
    7671,
    7671,
    7671,
    7671,
    7671,
    7671,
    7671,
    7671,
    7671,
    7671,
    7671,
    7671,
    7671,
    7671,
    7671,
    7671,
    7671,
    7671,
    7671,
    7671,
    7671,
    7671,
    7671,
    7671,
    7671,
    7671,
    7671,
    7671,
    7671,
    7671,
    7671,
    7671,
    7671,
    7671,
    7671,
    7671,
    7671,
    7671,
    7671,
    7671,
    7671,
    7671,
    7671,
    7671,
    7671,
    7671,
    7671,
    7671,
    7690,
    7716,
    7716,
    7742,
    7742,
    7742,
    7742,
    7742,
    7742,
    7742,
    7742,
    7742,
    7742,
    7742,
    7742,
    7742,
    7742,
    7742,
    7742,
    7759,
    7759,
    7759,
    7759,
    7759,
    7759,
    7759,
    7759,
    7759,
    7759,
    7759,
    7759,
    7759,
    7759,
    7759,
    7759,
    7759,
    7759,
    7759,
    7759,
    7759,
    7759,
    7759,
    7759,
    7759,
    7759,
    7759,
    7759,
    7759,
    7759,
    7759,
    7759,
    7759,
    7759,
    7759,
    7759,
    7759,
    7759,
    7759,
    7759,
    7777,
    7777,
    7777,
    7777,
    7777,
    7777,
    7777,
    7777,
    7777,
    7799,
    7799,
    7799,
    7799,
    7799,
    7799,
    7812,
    7830,
    7830,
    7830,
    7830,
    7830,
    7830,
    7830,
    7830,
    7830,
    7830,
    7830,
    7830,
    7830,
    7830,
    7830,
    7849,
    7849,
    7849,
    7849,
    7849,
    7849,
    7849,
    7849,
    7849,
    7849,
    7849,
    7849,
};

enum
  {
    TOTAL_KEYWORDS = 449,
    MIN_WORD_LENGTH = 1,
    MAX_WORD_LENGTH = 34,
    MIN_HASH_VALUE = 2,
    MAX_HASH_VALUE = 3035
  };

/* maximum key range = 3034, duplicates = 0 */

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
      3036, 3036, 3036, 3036, 3036, 3036, 3036, 3036, 3036, 3036,
      3036, 3036, 3036, 3036, 3036, 3036, 3036, 3036, 3036, 3036,
      3036, 3036, 3036, 3036, 3036, 3036, 3036, 3036, 3036, 3036,
      3036, 3036, 3036, 3036, 3036, 3036, 3036, 3036, 3036, 3036,
      3036, 3036, 3036, 3036, 3036,    2, 3036, 3036, 3036, 3036,
      3036, 3036, 3036, 3036, 3036, 3036, 3036, 3036, 3036, 3036,
      3036, 3036, 3036, 3036, 3036, 3036, 3036, 3036, 3036, 3036,
      3036, 3036, 3036, 3036, 3036, 3036, 3036, 3036, 3036, 3036,
      3036, 3036, 3036, 3036, 3036, 3036, 3036, 3036, 3036, 3036,
      3036, 3036, 3036, 3036, 3036, 3036, 3036,    3,   13,  100,
         3,    2,  447,    2,  434,    2,    9,   30,  121,    4,
         2,    2,  100,    2,    2,   41,    2,  255,   41,    5,
       839, 1019,  120, 3036, 3036, 3036, 3036, 3036, 3036, 3036,
      3036, 3036, 3036, 3036, 3036, 3036, 3036, 3036, 3036, 3036,
      3036, 3036, 3036, 3036, 3036, 3036, 3036, 3036, 3036, 3036,
      3036, 3036, 3036, 3036, 3036, 3036, 3036, 3036, 3036, 3036,
      3036, 3036, 3036, 3036, 3036, 3036, 3036, 3036, 3036, 3036,
      3036, 3036, 3036, 3036, 3036, 3036, 3036, 3036, 3036, 3036,
      3036, 3036, 3036, 3036, 3036, 3036, 3036, 3036, 3036, 3036,
      3036, 3036, 3036, 3036, 3036, 3036, 3036, 3036, 3036, 3036,
      3036, 3036, 3036, 3036, 3036, 3036, 3036, 3036, 3036, 3036,
      3036, 3036, 3036, 3036, 3036, 3036, 3036, 3036, 3036, 3036,
      3036, 3036, 3036, 3036, 3036, 3036, 3036, 3036, 3036, 3036,
      3036, 3036, 3036, 3036, 3036, 3036, 3036, 3036, 3036, 3036,
      3036, 3036, 3036, 3036, 3036, 3036, 3036, 3036, 3036, 3036,
      3036, 3036, 3036, 3036, 3036, 3036
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
    char stringpool_str3[sizeof("rotate")];
    char stringpool_str4[sizeof("motion")];
    char stringpool_str5[sizeof("margin")];
    char stringpool_str6[sizeof("grid-row")];
    char stringpool_str7[sizeof("grid-area")];
    char stringpool_str8[sizeof("animation")];
    char stringpool_str9[sizeof("orientation")];
    char stringpool_str10[sizeof("border")];
    char stringpool_str11[sizeof("bottom")];
    char stringpool_str12[sizeof("grid-row-end")];
    char stringpool_str13[sizeof("writing-mode")];
    char stringpool_str14[sizeof("motion-rotation")];
    char stringpool_str15[sizeof("image-rendering")];
    char stringpool_str16[sizeof("animation-name")];
    char stringpool_str17[sizeof("image-orientation")];
    char stringpool_str18[sizeof("border-image")];
    char stringpool_str19[sizeof("margin-bottom")];
    char stringpool_str20[sizeof("marker")];
    char stringpool_str21[sizeof("border-bottom")];
    char stringpool_str22[sizeof("marker-end")];
    char stringpool_str23[sizeof("marker-mid")];
    char stringpool_str24[sizeof("widows")];
    char stringpool_str25[sizeof("transition")];
    char stringpool_str26[sizeof("word-break")];
    char stringpool_str27[sizeof("-webkit-order")];
    char stringpool_str28[sizeof("grid-row-start")];
    char stringpool_str29[sizeof("mask")];
    char stringpool_str30[sizeof("stroke")];
    char stringpool_str31[sizeof("-webkit-animation")];
    char stringpool_str32[sizeof("-webkit-margin-end")];
    char stringpool_str33[sizeof("-webkit-writing-mode")];
    char stringpool_str34[sizeof("-webkit-border-end")];
    char stringpool_str35[sizeof("-webkit-animation-name")];
    char stringpool_str36[sizeof("marker-start")];
    char stringpool_str37[sizeof("-webkit-border-image")];
    char stringpool_str38[sizeof("top")];
    char stringpool_str39[sizeof("page")];
    char stringpool_str40[sizeof("content")];
    char stringpool_str41[sizeof("padding")];
    char stringpool_str42[sizeof("grid-gap")];
    char stringpool_str43[sizeof("direction")];
    char stringpool_str44[sizeof("-webkit-transition")];
    char stringpool_str45[sizeof("margin-top")];
    char stringpool_str46[sizeof("paint-order")];
    char stringpool_str47[sizeof("word-wrap")];
    char stringpool_str48[sizeof("-webkit-margin-start")];
    char stringpool_str49[sizeof("grid-row-gap")];
    char stringpool_str50[sizeof("zoom")];
    char stringpool_str51[sizeof("border-top")];
    char stringpool_str52[sizeof("-webkit-border-start")];
    char stringpool_str53[sizeof("-webkit-mask")];
    char stringpool_str54[sizeof("min-zoom")];
    char stringpool_str55[sizeof("animation-direction")];
    char stringpool_str56[sizeof("padding-bottom")];
    char stringpool_str57[sizeof("src")];
    char stringpool_str58[sizeof("-webkit-mask-origin")];
    char stringpool_str59[sizeof("-webkit-mask-image")];
    char stringpool_str60[sizeof("border-image-repeat")];
    char stringpool_str61[sizeof("position")];
    char stringpool_str62[sizeof("size")];
    char stringpool_str63[sizeof("resize")];
    char stringpool_str64[sizeof("speak")];
    char stringpool_str65[sizeof("isolation")];
    char stringpool_str66[sizeof("translate")];
    char stringpool_str67[sizeof("-webkit-padding-end")];
    char stringpool_str68[sizeof("align-items")];
    char stringpool_str69[sizeof("tab-size")];
    char stringpool_str70[sizeof("-webkit-animation-direction")];
    char stringpool_str71[sizeof("-webkit-rtl-ordering")];
    char stringpool_str72[sizeof("pointer-events")];
    char stringpool_str73[sizeof("dominant-baseline")];
    char stringpool_str74[sizeof("page-break-inside")];
    char stringpool_str75[sizeof("padding-top")];
    char stringpool_str76[sizeof("stroke-linejoin")];
    char stringpool_str77[sizeof("stroke-miterlimit")];
    char stringpool_str78[sizeof("-webkit-padding-start")];
    char stringpool_str79[sizeof("color")];
    char stringpool_str80[sizeof("clear")];
    char stringpool_str81[sizeof("-webkit-line-break")];
    char stringpool_str82[sizeof("-webkit-align-items")];
    char stringpool_str83[sizeof("align-content")];
    char stringpool_str84[sizeof("all")];
    char stringpool_str85[sizeof("grid-template")];
    char stringpool_str86[sizeof("color-rendering")];
    char stringpool_str87[sizeof("-webkit-mask-repeat")];
    char stringpool_str88[sizeof("border-color")];
    char stringpool_str89[sizeof("caption-side")];
    char stringpool_str90[sizeof("word-spacing")];
    char stringpool_str91[sizeof("scale")];
    char stringpool_str92[sizeof("-webkit-app-region")];
    char stringpool_str93[sizeof("border-spacing")];
    char stringpool_str94[sizeof("border-bottom-color")];
    char stringpool_str95[sizeof("object-position")];
    char stringpool_str96[sizeof("-webkit-mask-position")];
    char stringpool_str97[sizeof("animation-duration")];
    char stringpool_str98[sizeof("grid-template-rows")];
    char stringpool_str99[sizeof("grid-template-areas")];
    char stringpool_str100[sizeof("-webkit-align-content")];
    char stringpool_str101[sizeof("-webkit-mask-size")];
    char stringpool_str102[sizeof("quotes")];
    char stringpool_str103[sizeof("border-image-slice")];
    char stringpool_str104[sizeof("-webkit-border-end-color")];
    char stringpool_str105[sizeof("clip")];
    char stringpool_str106[sizeof("grid-auto-rows")];
    char stringpool_str107[sizeof("alignment-baseline")];
    char stringpool_str108[sizeof("border-radius")];
    char stringpool_str109[sizeof("transition-duration")];
    char stringpool_str110[sizeof("border-image-outset")];
    char stringpool_str111[sizeof("-webkit-animation-duration")];
    char stringpool_str112[sizeof("border-top-color")];
    char stringpool_str113[sizeof("-webkit-border-start-color")];
    char stringpool_str114[sizeof("-webkit-user-drag")];
    char stringpool_str115[sizeof("stop-color")];
    char stringpool_str116[sizeof("-webkit-appearance")];
    char stringpool_str117[sizeof("unicode-range")];
    char stringpool_str118[sizeof("letter-spacing")];
    char stringpool_str119[sizeof("outline")];
    char stringpool_str120[sizeof("unicode-bidi")];
    char stringpool_str121[sizeof("-webkit-border-radius")];
    char stringpool_str122[sizeof("-webkit-transition-duration")];
    char stringpool_str123[sizeof("-webkit-mask-composite")];
    char stringpool_str124[sizeof("perspective")];
    char stringpool_str125[sizeof("-webkit-caret-color")];
    char stringpool_str126[sizeof("cursor")];
    char stringpool_str127[sizeof("-epub-writing-mode")];
    char stringpool_str128[sizeof("vertical-align")];
    char stringpool_str129[sizeof("animation-iteration-count")];
    char stringpool_str130[sizeof("-webkit-locale")];
    char stringpool_str131[sizeof("perspective-origin")];
    char stringpool_str132[sizeof("stroke-linecap")];
    char stringpool_str133[sizeof("background")];
    char stringpool_str134[sizeof("counter-reset")];
    char stringpool_str135[sizeof("background-origin")];
    char stringpool_str136[sizeof("background-image")];
    char stringpool_str137[sizeof("user-zoom")];
    char stringpool_str138[sizeof("-epub-word-break")];
    char stringpool_str139[sizeof("right")];
    char stringpool_str140[sizeof("border-image-source")];
    char stringpool_str141[sizeof("width")];
    char stringpool_str142[sizeof("-webkit-perspective")];
    char stringpool_str143[sizeof("font")];
    char stringpool_str144[sizeof("min-width")];
    char stringpool_str145[sizeof("margin-right")];
    char stringpool_str146[sizeof("-webkit-mask-clip")];
    char stringpool_str147[sizeof("-webkit-animation-iteration-count")];
    char stringpool_str148[sizeof("-webkit-perspective-origin")];
    char stringpool_str149[sizeof("border-right")];
    char stringpool_str150[sizeof("border-width")];
    char stringpool_str151[sizeof("color-interpolation")];
    char stringpool_str152[sizeof("-bb-rubberbandable")];
    char stringpool_str153[sizeof("-webkit-background-origin")];
    char stringpool_str154[sizeof("counter-increment")];
    char stringpool_str155[sizeof("border-image-width")];
    char stringpool_str156[sizeof("grid-column")];
    char stringpool_str157[sizeof("font-kerning")];
    char stringpool_str158[sizeof("border-bottom-width")];
    char stringpool_str159[sizeof("grid-column-end")];
    char stringpool_str160[sizeof("transform")];
    char stringpool_str161[sizeof("font-variant")];
    char stringpool_str162[sizeof("-webkit-line-clamp")];
    char stringpool_str163[sizeof("border-collapse")];
    char stringpool_str164[sizeof("transform-origin")];
    char stringpool_str165[sizeof("background-repeat")];
    char stringpool_str166[sizeof("stroke-width")];
    char stringpool_str167[sizeof("-webkit-margin-after")];
    char stringpool_str168[sizeof("-webkit-border-end-width")];
    char stringpool_str169[sizeof("-webkit-border-after")];
    char stringpool_str170[sizeof("-webkit-margin-before")];
    char stringpool_str171[sizeof("grid-column-start")];
    char stringpool_str172[sizeof("-webkit-border-before")];
    char stringpool_str173[sizeof("motion-path")];
    char stringpool_str174[sizeof("padding-right")];
    char stringpool_str175[sizeof("-webkit-transform")];
    char stringpool_str176[sizeof("-webkit-margin-collapse")];
    char stringpool_str177[sizeof("background-position")];
    char stringpool_str178[sizeof("background-blend-mode")];
    char stringpool_str179[sizeof("left")];
    char stringpool_str180[sizeof("float")];
    char stringpool_str181[sizeof("filter")];
    char stringpool_str182[sizeof("-webkit-transform-origin")];
    char stringpool_str183[sizeof("border-top-width")];
    char stringpool_str184[sizeof("background-size")];
    char stringpool_str185[sizeof("object-fit")];
    char stringpool_str186[sizeof("-webkit-border-start-width")];
    char stringpool_str187[sizeof("-webkit-columns")];
    char stringpool_str188[sizeof("orphans")];
    char stringpool_str189[sizeof("margin-left")];
    char stringpool_str190[sizeof("-webkit-margin-bottom-collapse")];
    char stringpool_str191[sizeof("shape-margin")];
    char stringpool_str192[sizeof("border-left")];
    char stringpool_str193[sizeof("scroll-snap-destination")];
    char stringpool_str194[sizeof("shape-rendering")];
    char stringpool_str195[sizeof("grid-column-gap")];
    char stringpool_str196[sizeof("-webkit-border-vertical-spacing")];
    char stringpool_str197[sizeof("outline-color")];
    char stringpool_str198[sizeof("page-break-after")];
    char stringpool_str199[sizeof("font-size")];
    char stringpool_str200[sizeof("overflow")];
    char stringpool_str201[sizeof("-webkit-user-select")];
    char stringpool_str202[sizeof("page-break-before")];
    char stringpool_str203[sizeof("-webkit-padding-after")];
    char stringpool_str204[sizeof("-webkit-filter")];
    char stringpool_str205[sizeof("-epub-caption-side")];
    char stringpool_str206[sizeof("-webkit-background-size")];
    char stringpool_str207[sizeof("background-color")];
    char stringpool_str208[sizeof("-webkit-padding-before")];
    char stringpool_str209[sizeof("-webkit-column-break-inside")];
    char stringpool_str210[sizeof("-webkit-column-gap")];
    char stringpool_str211[sizeof("-webkit-shape-margin")];
    char stringpool_str212[sizeof("scroll-snap-coordinate")];
    char stringpool_str213[sizeof("-webkit-margin-top-collapse")];
    char stringpool_str214[sizeof("padding-left")];
    char stringpool_str215[sizeof("-webkit-column-span")];
    char stringpool_str216[sizeof("fill")];
    char stringpool_str217[sizeof("white-space")];
    char stringpool_str218[sizeof("border-right-color")];
    char stringpool_str219[sizeof("-webkit-transform-origin-z")];
    char stringpool_str220[sizeof("clip-rule")];
    char stringpool_str221[sizeof("-webkit-print-color-adjust")];
    char stringpool_str222[sizeof("-webkit-background-composite")];
    char stringpool_str223[sizeof("animation-fill-mode")];
    char stringpool_str224[sizeof("overflow-wrap")];
    char stringpool_str225[sizeof("background-clip")];
    char stringpool_str226[sizeof("align-self")];
    char stringpool_str227[sizeof("-webkit-border-after-color")];
    char stringpool_str228[sizeof("grid-template-columns")];
    char stringpool_str229[sizeof("-webkit-border-before-color")];
    char stringpool_str230[sizeof("-webkit-animation-fill-mode")];
    char stringpool_str231[sizeof("will-change")];
    char stringpool_str232[sizeof("-webkit-background-clip")];
    char stringpool_str233[sizeof("lighting-color")];
    char stringpool_str234[sizeof("grid-auto-columns")];
    char stringpool_str235[sizeof("-webkit-align-self")];
    char stringpool_str236[sizeof("border-bottom-right-radius")];
    char stringpool_str237[sizeof("flood-color")];
    char stringpool_str238[sizeof("-webkit-font-size-delta")];
    char stringpool_str239[sizeof("border-left-color")];
    char stringpool_str240[sizeof("backdrop-filter")];
    char stringpool_str241[sizeof("outline-width")];
    char stringpool_str242[sizeof("x")];
    char stringpool_str243[sizeof("rx")];
    char stringpool_str244[sizeof("grid-auto-flow")];
    char stringpool_str245[sizeof("animation-timing-function")];
    char stringpool_str246[sizeof("-webkit-logical-width")];
    char stringpool_str247[sizeof("text-indent")];
    char stringpool_str248[sizeof("-webkit-border-bottom-right-radius")];
    char stringpool_str249[sizeof("clip-path")];
    char stringpool_str250[sizeof("text-rendering")];
    char stringpool_str251[sizeof("-webkit-min-logical-width")];
    char stringpool_str252[sizeof("text-orientation")];
    char stringpool_str253[sizeof("height")];
    char stringpool_str254[sizeof("border-top-right-radius")];
    char stringpool_str255[sizeof("min-height")];
    char stringpool_str256[sizeof("scroll-behavior")];
    char stringpool_str257[sizeof("shape-outside")];
    char stringpool_str258[sizeof("transition-timing-function")];
    char stringpool_str259[sizeof("font-weight")];
    char stringpool_str260[sizeof("-webkit-column-count")];
    char stringpool_str261[sizeof("touch-action")];
    char stringpool_str262[sizeof("-webkit-animation-timing-function")];
    char stringpool_str263[sizeof("border-right-width")];
    char stringpool_str264[sizeof("-webkit-clip-path")];
    char stringpool_str265[sizeof("-webkit-column-rule")];
    char stringpool_str266[sizeof("-webkit-box-orient")];
    char stringpool_str267[sizeof("-webkit-text-orientation")];
    char stringpool_str268[sizeof("border-bottom-left-radius")];
    char stringpool_str269[sizeof("font-size-adjust")];
    char stringpool_str270[sizeof("cx")];
    char stringpool_str271[sizeof("-webkit-border-top-right-radius")];
    char stringpool_str272[sizeof("font-variant-ligatures")];
    char stringpool_str273[sizeof("-webkit-shape-outside")];
    char stringpool_str274[sizeof("-webkit-transition-timing-function")];
    char stringpool_str275[sizeof("motion-offset")];
    char stringpool_str276[sizeof("text-decoration")];
    char stringpool_str277[sizeof("background-attachment")];
    char stringpool_str278[sizeof("z-index")];
    char stringpool_str279[sizeof("max-zoom")];
    char stringpool_str280[sizeof("text-align")];
    char stringpool_str281[sizeof("-webkit-text-stroke")];
    char stringpool_str282[sizeof("-webkit-border-after-width")];
    char stringpool_str283[sizeof("-webkit-column-width")];
    char stringpool_str284[sizeof("-webkit-border-bottom-left-radius")];
    char stringpool_str285[sizeof("-webkit-border-before-width")];
    char stringpool_str286[sizeof("mix-blend-mode")];
    char stringpool_str287[sizeof("-webkit-font-smoothing")];
    char stringpool_str288[sizeof("line-height")];
    char stringpool_str289[sizeof("-webkit-mask-box-image")];
    char stringpool_str290[sizeof("border-top-left-radius")];
    char stringpool_str291[sizeof("y")];
    char stringpool_str292[sizeof("ry")];
    char stringpool_str293[sizeof("-webkit-margin-after-collapse")];
    char stringpool_str294[sizeof("box-sizing")];
    char stringpool_str295[sizeof("-webkit-border-horizontal-spacing")];
    char stringpool_str296[sizeof("-webkit-text-combine")];
    char stringpool_str297[sizeof("-webkit-box-direction")];
    char stringpool_str298[sizeof("-webkit-margin-before-collapse")];
    char stringpool_str299[sizeof("font-stretch")];
    char stringpool_str300[sizeof("-webkit-box-align")];
    char stringpool_str301[sizeof("border-left-width")];
    char stringpool_str302[sizeof("-webkit-column-break-after")];
    char stringpool_str303[sizeof("-webkit-column-break-before")];
    char stringpool_str304[sizeof("-webkit-border-top-left-radius")];
    char stringpool_str305[sizeof("fill-rule")];
    char stringpool_str306[sizeof("-webkit-box-lines")];
    char stringpool_str307[sizeof("-webkit-box-sizing")];
    char stringpool_str308[sizeof("-webkit-box-decoration-break")];
    char stringpool_str309[sizeof("-webkit-mask-repeat-x")];
    char stringpool_str310[sizeof("color-interpolation-filters")];
    char stringpool_str311[sizeof("text-decoration-line")];
    char stringpool_str312[sizeof("baseline-shift")];
    char stringpool_str313[sizeof("cy")];
    char stringpool_str314[sizeof("-webkit-mask-box-image-repeat")];
    char stringpool_str315[sizeof("-webkit-mask-position-x")];
    char stringpool_str316[sizeof("text-align-last")];
    char stringpool_str317[sizeof("-webkit-box-pack")];
    char stringpool_str318[sizeof("vector-effect")];
    char stringpool_str319[sizeof("-webkit-column-rule-color")];
    char stringpool_str320[sizeof("animation-delay")];
    char stringpool_str321[sizeof("column-fill")];
    char stringpool_str322[sizeof("buffered-rendering")];
    char stringpool_str323[sizeof("text-decoration-color")];
    char stringpool_str324[sizeof("mask-type")];
    char stringpool_str325[sizeof("transition-delay")];
    char stringpool_str326[sizeof("border-style")];
    char stringpool_str327[sizeof("-webkit-text-stroke-color")];
    char stringpool_str328[sizeof("opacity")];
    char stringpool_str329[sizeof("-webkit-animation-delay")];
    char stringpool_str330[sizeof("border-bottom-style")];
    char stringpool_str331[sizeof("-epub-text-orientation")];
    char stringpool_str332[sizeof("visibility")];
    char stringpool_str333[sizeof("font-feature-settings")];
    char stringpool_str334[sizeof("-webkit-transition-delay")];
    char stringpool_str335[sizeof("-webkit-mask-repeat-y")];
    char stringpool_str336[sizeof("-webkit-mask-box-image-slice")];
    char stringpool_str337[sizeof("-webkit-border-end-style")];
    char stringpool_str338[sizeof("-webkit-opacity")];
    char stringpool_str339[sizeof("-webkit-logical-height")];
    char stringpool_str340[sizeof("display")];
    char stringpool_str341[sizeof("transition-property")];
    char stringpool_str342[sizeof("max-width")];
    char stringpool_str343[sizeof("-webkit-min-logical-height")];
    char stringpool_str344[sizeof("-webkit-perspective-origin-x")];
    char stringpool_str345[sizeof("stroke-opacity")];
    char stringpool_str346[sizeof("-webkit-mask-position-y")];
    char stringpool_str347[sizeof("-webkit-mask-box-image-outset")];
    char stringpool_str348[sizeof("border-top-style")];
    char stringpool_str349[sizeof("animation-play-state")];
    char stringpool_str350[sizeof("-webkit-border-start-style")];
    char stringpool_str351[sizeof("-webkit-font-feature-settings")];
    char stringpool_str352[sizeof("outline-offset")];
    char stringpool_str353[sizeof("text-shadow")];
    char stringpool_str354[sizeof("box-shadow")];
    char stringpool_str355[sizeof("-epub-text-combine")];
    char stringpool_str356[sizeof("-webkit-transition-property")];
    char stringpool_str357[sizeof("text-transform")];
    char stringpool_str358[sizeof("list-style")];
    char stringpool_str359[sizeof("background-repeat-x")];
    char stringpool_str360[sizeof("list-style-image")];
    char stringpool_str361[sizeof("-webkit-column-rule-width")];
    char stringpool_str362[sizeof("stop-opacity")];
    char stringpool_str363[sizeof("-webkit-animation-play-state")];
    char stringpool_str364[sizeof("text-anchor")];
    char stringpool_str365[sizeof("text-underline-position")];
    char stringpool_str366[sizeof("-webkit-box-shadow")];
    char stringpool_str367[sizeof("background-position-x")];
    char stringpool_str368[sizeof("flex")];
    char stringpool_str369[sizeof("-webkit-mask-box-image-source")];
    char stringpool_str370[sizeof("-webkit-box-ordinal-group")];
    char stringpool_str371[sizeof("-webkit-transform-origin-x")];
    char stringpool_str372[sizeof("flex-grow")];
    char stringpool_str373[sizeof("-webkit-text-stroke-width")];
    char stringpool_str374[sizeof("-webkit-mask-box-image-width")];
    char stringpool_str375[sizeof("overflow-x")];
    char stringpool_str376[sizeof("-webkit-flex")];
    char stringpool_str377[sizeof("text-overflow")];
    char stringpool_str378[sizeof("-webkit-flex-grow")];
    char stringpool_str379[sizeof("-webkit-perspective-origin-y")];
    char stringpool_str380[sizeof("-webkit-highlight")];
    char stringpool_str381[sizeof("-webkit-ruby-position")];
    char stringpool_str382[sizeof("stroke-dashoffset")];
    char stringpool_str383[sizeof("list-style-position")];
    char stringpool_str384[sizeof("flex-basis")];
    char stringpool_str385[sizeof("empty-cells")];
    char stringpool_str386[sizeof("flex-wrap")];
    char stringpool_str387[sizeof("scroll-snap-points-x")];
    char stringpool_str388[sizeof("flex-direction")];
    char stringpool_str389[sizeof("-webkit-text-emphasis")];
    char stringpool_str390[sizeof("table-layout")];
    char stringpool_str391[sizeof("background-repeat-y")];
    char stringpool_str392[sizeof("-webkit-flex-basis")];
    char stringpool_str393[sizeof("outline-style")];
    char stringpool_str394[sizeof("-webkit-flex-wrap")];
    char stringpool_str395[sizeof("-webkit-flex-direction")];
    char stringpool_str396[sizeof("background-position-y")];
    char stringpool_str397[sizeof("-webkit-box-reflect")];
    char stringpool_str398[sizeof("stroke-dasharray")];
    char stringpool_str399[sizeof("-webkit-transform-origin-y")];
    char stringpool_str400[sizeof("mask-source-type")];
    char stringpool_str401[sizeof("shape-image-threshold")];
    char stringpool_str402[sizeof("font-style")];
    char stringpool_str403[sizeof("overflow-y")];
    char stringpool_str404[sizeof("border-right-style")];
    char stringpool_str405[sizeof("scroll-snap-type")];
    char stringpool_str406[sizeof("-webkit-text-emphasis-position")];
    char stringpool_str407[sizeof("transform-style")];
    char stringpool_str408[sizeof("-webkit-shape-image-threshold")];
    char stringpool_str409[sizeof("-webkit-max-logical-width")];
    char stringpool_str410[sizeof("scroll-snap-points-y")];
    char stringpool_str411[sizeof("max-height")];
    char stringpool_str412[sizeof("-epub-text-transform")];
    char stringpool_str413[sizeof("-webkit-border-after-style")];
    char stringpool_str414[sizeof("-webkit-border-before-style")];
    char stringpool_str415[sizeof("-webkit-transform-style")];
    char stringpool_str416[sizeof("-webkit-text-emphasis-color")];
    char stringpool_str417[sizeof("text-combine-upright")];
    char stringpool_str418[sizeof("border-left-style")];
    char stringpool_str419[sizeof("flood-opacity")];
    char stringpool_str420[sizeof("-webkit-text-fill-color")];
    char stringpool_str421[sizeof("-webkit-tap-highlight-color")];
    char stringpool_str422[sizeof("justify-items")];
    char stringpool_str423[sizeof("-webkit-user-modify")];
    char stringpool_str424[sizeof("-epub-text-emphasis")];
    char stringpool_str425[sizeof("justify-content")];
    char stringpool_str426[sizeof("fill-opacity")];
    char stringpool_str427[sizeof("flex-shrink")];
    char stringpool_str428[sizeof("backface-visibility")];
    char stringpool_str429[sizeof("-webkit-justify-content")];
    char stringpool_str430[sizeof("-webkit-flex-shrink")];
    char stringpool_str431[sizeof("flex-flow")];
    char stringpool_str432[sizeof("-webkit-backface-visibility")];
    char stringpool_str433[sizeof("-webkit-flex-flow")];
    char stringpool_str434[sizeof("font-family")];
    char stringpool_str435[sizeof("-webkit-text-decorations-in-effect")];
    char stringpool_str436[sizeof("-epub-text-emphasis-color")];
    char stringpool_str437[sizeof("-webkit-column-rule-style")];
    char stringpool_str438[sizeof("-webkit-max-logical-height")];
    char stringpool_str439[sizeof("text-decoration-style")];
    char stringpool_str440[sizeof("-webkit-box-flex")];
    char stringpool_str441[sizeof("-webkit-text-security")];
    char stringpool_str442[sizeof("justify-self")];
    char stringpool_str443[sizeof("list-style-type")];
    char stringpool_str444[sizeof("text-justify")];
    char stringpool_str445[sizeof("-webkit-box-flex-group")];
    char stringpool_str446[sizeof("-webkit-hyphenate-character")];
    char stringpool_str447[sizeof("-webkit-text-emphasis-style")];
    char stringpool_str448[sizeof("-epub-text-emphasis-style")];
  };
static const struct stringpool_t stringpool_contents =
  {
    "r",
    "grid",
    "order",
    "rotate",
    "motion",
    "margin",
    "grid-row",
    "grid-area",
    "animation",
    "orientation",
    "border",
    "bottom",
    "grid-row-end",
    "writing-mode",
    "motion-rotation",
    "image-rendering",
    "animation-name",
    "image-orientation",
    "border-image",
    "margin-bottom",
    "marker",
    "border-bottom",
    "marker-end",
    "marker-mid",
    "widows",
    "transition",
    "word-break",
    "-webkit-order",
    "grid-row-start",
    "mask",
    "stroke",
    "-webkit-animation",
    "-webkit-margin-end",
    "-webkit-writing-mode",
    "-webkit-border-end",
    "-webkit-animation-name",
    "marker-start",
    "-webkit-border-image",
    "top",
    "page",
    "content",
    "padding",
    "grid-gap",
    "direction",
    "-webkit-transition",
    "margin-top",
    "paint-order",
    "word-wrap",
    "-webkit-margin-start",
    "grid-row-gap",
    "zoom",
    "border-top",
    "-webkit-border-start",
    "-webkit-mask",
    "min-zoom",
    "animation-direction",
    "padding-bottom",
    "src",
    "-webkit-mask-origin",
    "-webkit-mask-image",
    "border-image-repeat",
    "position",
    "size",
    "resize",
    "speak",
    "isolation",
    "translate",
    "-webkit-padding-end",
    "align-items",
    "tab-size",
    "-webkit-animation-direction",
    "-webkit-rtl-ordering",
    "pointer-events",
    "dominant-baseline",
    "page-break-inside",
    "padding-top",
    "stroke-linejoin",
    "stroke-miterlimit",
    "-webkit-padding-start",
    "color",
    "clear",
    "-webkit-line-break",
    "-webkit-align-items",
    "align-content",
    "all",
    "grid-template",
    "color-rendering",
    "-webkit-mask-repeat",
    "border-color",
    "caption-side",
    "word-spacing",
    "scale",
    "-webkit-app-region",
    "border-spacing",
    "border-bottom-color",
    "object-position",
    "-webkit-mask-position",
    "animation-duration",
    "grid-template-rows",
    "grid-template-areas",
    "-webkit-align-content",
    "-webkit-mask-size",
    "quotes",
    "border-image-slice",
    "-webkit-border-end-color",
    "clip",
    "grid-auto-rows",
    "alignment-baseline",
    "border-radius",
    "transition-duration",
    "border-image-outset",
    "-webkit-animation-duration",
    "border-top-color",
    "-webkit-border-start-color",
    "-webkit-user-drag",
    "stop-color",
    "-webkit-appearance",
    "unicode-range",
    "letter-spacing",
    "outline",
    "unicode-bidi",
    "-webkit-border-radius",
    "-webkit-transition-duration",
    "-webkit-mask-composite",
    "perspective",
    "-webkit-caret-color",
    "cursor",
    "-epub-writing-mode",
    "vertical-align",
    "animation-iteration-count",
    "-webkit-locale",
    "perspective-origin",
    "stroke-linecap",
    "background",
    "counter-reset",
    "background-origin",
    "background-image",
    "user-zoom",
    "-epub-word-break",
    "right",
    "border-image-source",
    "width",
    "-webkit-perspective",
    "font",
    "min-width",
    "margin-right",
    "-webkit-mask-clip",
    "-webkit-animation-iteration-count",
    "-webkit-perspective-origin",
    "border-right",
    "border-width",
    "color-interpolation",
    "-bb-rubberbandable",
    "-webkit-background-origin",
    "counter-increment",
    "border-image-width",
    "grid-column",
    "font-kerning",
    "border-bottom-width",
    "grid-column-end",
    "transform",
    "font-variant",
    "-webkit-line-clamp",
    "border-collapse",
    "transform-origin",
    "background-repeat",
    "stroke-width",
    "-webkit-margin-after",
    "-webkit-border-end-width",
    "-webkit-border-after",
    "-webkit-margin-before",
    "grid-column-start",
    "-webkit-border-before",
    "motion-path",
    "padding-right",
    "-webkit-transform",
    "-webkit-margin-collapse",
    "background-position",
    "background-blend-mode",
    "left",
    "float",
    "filter",
    "-webkit-transform-origin",
    "border-top-width",
    "background-size",
    "object-fit",
    "-webkit-border-start-width",
    "-webkit-columns",
    "orphans",
    "margin-left",
    "-webkit-margin-bottom-collapse",
    "shape-margin",
    "border-left",
    "scroll-snap-destination",
    "shape-rendering",
    "grid-column-gap",
    "-webkit-border-vertical-spacing",
    "outline-color",
    "page-break-after",
    "font-size",
    "overflow",
    "-webkit-user-select",
    "page-break-before",
    "-webkit-padding-after",
    "-webkit-filter",
    "-epub-caption-side",
    "-webkit-background-size",
    "background-color",
    "-webkit-padding-before",
    "-webkit-column-break-inside",
    "-webkit-column-gap",
    "-webkit-shape-margin",
    "scroll-snap-coordinate",
    "-webkit-margin-top-collapse",
    "padding-left",
    "-webkit-column-span",
    "fill",
    "white-space",
    "border-right-color",
    "-webkit-transform-origin-z",
    "clip-rule",
    "-webkit-print-color-adjust",
    "-webkit-background-composite",
    "animation-fill-mode",
    "overflow-wrap",
    "background-clip",
    "align-self",
    "-webkit-border-after-color",
    "grid-template-columns",
    "-webkit-border-before-color",
    "-webkit-animation-fill-mode",
    "will-change",
    "-webkit-background-clip",
    "lighting-color",
    "grid-auto-columns",
    "-webkit-align-self",
    "border-bottom-right-radius",
    "flood-color",
    "-webkit-font-size-delta",
    "border-left-color",
    "backdrop-filter",
    "outline-width",
    "x",
    "rx",
    "grid-auto-flow",
    "animation-timing-function",
    "-webkit-logical-width",
    "text-indent",
    "-webkit-border-bottom-right-radius",
    "clip-path",
    "text-rendering",
    "-webkit-min-logical-width",
    "text-orientation",
    "height",
    "border-top-right-radius",
    "min-height",
    "scroll-behavior",
    "shape-outside",
    "transition-timing-function",
    "font-weight",
    "-webkit-column-count",
    "touch-action",
    "-webkit-animation-timing-function",
    "border-right-width",
    "-webkit-clip-path",
    "-webkit-column-rule",
    "-webkit-box-orient",
    "-webkit-text-orientation",
    "border-bottom-left-radius",
    "font-size-adjust",
    "cx",
    "-webkit-border-top-right-radius",
    "font-variant-ligatures",
    "-webkit-shape-outside",
    "-webkit-transition-timing-function",
    "motion-offset",
    "text-decoration",
    "background-attachment",
    "z-index",
    "max-zoom",
    "text-align",
    "-webkit-text-stroke",
    "-webkit-border-after-width",
    "-webkit-column-width",
    "-webkit-border-bottom-left-radius",
    "-webkit-border-before-width",
    "mix-blend-mode",
    "-webkit-font-smoothing",
    "line-height",
    "-webkit-mask-box-image",
    "border-top-left-radius",
    "y",
    "ry",
    "-webkit-margin-after-collapse",
    "box-sizing",
    "-webkit-border-horizontal-spacing",
    "-webkit-text-combine",
    "-webkit-box-direction",
    "-webkit-margin-before-collapse",
    "font-stretch",
    "-webkit-box-align",
    "border-left-width",
    "-webkit-column-break-after",
    "-webkit-column-break-before",
    "-webkit-border-top-left-radius",
    "fill-rule",
    "-webkit-box-lines",
    "-webkit-box-sizing",
    "-webkit-box-decoration-break",
    "-webkit-mask-repeat-x",
    "color-interpolation-filters",
    "text-decoration-line",
    "baseline-shift",
    "cy",
    "-webkit-mask-box-image-repeat",
    "-webkit-mask-position-x",
    "text-align-last",
    "-webkit-box-pack",
    "vector-effect",
    "-webkit-column-rule-color",
    "animation-delay",
    "column-fill",
    "buffered-rendering",
    "text-decoration-color",
    "mask-type",
    "transition-delay",
    "border-style",
    "-webkit-text-stroke-color",
    "opacity",
    "-webkit-animation-delay",
    "border-bottom-style",
    "-epub-text-orientation",
    "visibility",
    "font-feature-settings",
    "-webkit-transition-delay",
    "-webkit-mask-repeat-y",
    "-webkit-mask-box-image-slice",
    "-webkit-border-end-style",
    "-webkit-opacity",
    "-webkit-logical-height",
    "display",
    "transition-property",
    "max-width",
    "-webkit-min-logical-height",
    "-webkit-perspective-origin-x",
    "stroke-opacity",
    "-webkit-mask-position-y",
    "-webkit-mask-box-image-outset",
    "border-top-style",
    "animation-play-state",
    "-webkit-border-start-style",
    "-webkit-font-feature-settings",
    "outline-offset",
    "text-shadow",
    "box-shadow",
    "-epub-text-combine",
    "-webkit-transition-property",
    "text-transform",
    "list-style",
    "background-repeat-x",
    "list-style-image",
    "-webkit-column-rule-width",
    "stop-opacity",
    "-webkit-animation-play-state",
    "text-anchor",
    "text-underline-position",
    "-webkit-box-shadow",
    "background-position-x",
    "flex",
    "-webkit-mask-box-image-source",
    "-webkit-box-ordinal-group",
    "-webkit-transform-origin-x",
    "flex-grow",
    "-webkit-text-stroke-width",
    "-webkit-mask-box-image-width",
    "overflow-x",
    "-webkit-flex",
    "text-overflow",
    "-webkit-flex-grow",
    "-webkit-perspective-origin-y",
    "-webkit-highlight",
    "-webkit-ruby-position",
    "stroke-dashoffset",
    "list-style-position",
    "flex-basis",
    "empty-cells",
    "flex-wrap",
    "scroll-snap-points-x",
    "flex-direction",
    "-webkit-text-emphasis",
    "table-layout",
    "background-repeat-y",
    "-webkit-flex-basis",
    "outline-style",
    "-webkit-flex-wrap",
    "-webkit-flex-direction",
    "background-position-y",
    "-webkit-box-reflect",
    "stroke-dasharray",
    "-webkit-transform-origin-y",
    "mask-source-type",
    "shape-image-threshold",
    "font-style",
    "overflow-y",
    "border-right-style",
    "scroll-snap-type",
    "-webkit-text-emphasis-position",
    "transform-style",
    "-webkit-shape-image-threshold",
    "-webkit-max-logical-width",
    "scroll-snap-points-y",
    "max-height",
    "-epub-text-transform",
    "-webkit-border-after-style",
    "-webkit-border-before-style",
    "-webkit-transform-style",
    "-webkit-text-emphasis-color",
    "text-combine-upright",
    "border-left-style",
    "flood-opacity",
    "-webkit-text-fill-color",
    "-webkit-tap-highlight-color",
    "justify-items",
    "-webkit-user-modify",
    "-epub-text-emphasis",
    "justify-content",
    "fill-opacity",
    "flex-shrink",
    "backface-visibility",
    "-webkit-justify-content",
    "-webkit-flex-shrink",
    "flex-flow",
    "-webkit-backface-visibility",
    "-webkit-flex-flow",
    "font-family",
    "-webkit-text-decorations-in-effect",
    "-epub-text-emphasis-color",
    "-webkit-column-rule-style",
    "-webkit-max-logical-height",
    "text-decoration-style",
    "-webkit-box-flex",
    "-webkit-text-security",
    "justify-self",
    "list-style-type",
    "text-justify",
    "-webkit-box-flex-group",
    "-webkit-hyphenate-character",
    "-webkit-text-emphasis-style",
    "-epub-text-emphasis-style"
  };
#define stringpool ((const char *) &stringpool_contents)

static const struct Property property_word_list[] =
  {
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str0, CSSPropertyR},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str1, CSSPropertyGrid},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str2, CSSPropertyOrder},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str3, CSSPropertyRotate},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str4, CSSPropertyMotion},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str5, CSSPropertyMargin},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str6, CSSPropertyGridRow},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str7, CSSPropertyGridArea},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str8, CSSPropertyAnimation},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str9, CSSPropertyOrientation},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str10, CSSPropertyBorder},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str11, CSSPropertyBottom},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str12, CSSPropertyGridRowEnd},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str13, CSSPropertyWritingMode},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str14, CSSPropertyMotionRotation},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str15, CSSPropertyImageRendering},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str16, CSSPropertyAnimationName},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str17, CSSPropertyImageOrientation},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str18, CSSPropertyBorderImage},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str19, CSSPropertyMarginBottom},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str20, CSSPropertyMarker},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str21, CSSPropertyBorderBottom},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str22, CSSPropertyMarkerEnd},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str23, CSSPropertyMarkerMid},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str24, CSSPropertyWidows},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str25, CSSPropertyTransition},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str26, CSSPropertyWordBreak},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str27, CSSPropertyAliasWebkitOrder},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str28, CSSPropertyGridRowStart},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str29, CSSPropertyMask},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str30, CSSPropertyStroke},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str31, CSSPropertyAliasWebkitAnimation},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str32, CSSPropertyWebkitMarginEnd},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str33, CSSPropertyWebkitWritingMode},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str34, CSSPropertyWebkitBorderEnd},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str35, CSSPropertyAliasWebkitAnimationName},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str36, CSSPropertyMarkerStart},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str37, CSSPropertyWebkitBorderImage},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str38, CSSPropertyTop},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str39, CSSPropertyPage},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str40, CSSPropertyContent},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str41, CSSPropertyPadding},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str42, CSSPropertyGridGap},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str43, CSSPropertyDirection},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str44, CSSPropertyAliasWebkitTransition},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str45, CSSPropertyMarginTop},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str46, CSSPropertyPaintOrder},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str47, CSSPropertyWordWrap},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str48, CSSPropertyWebkitMarginStart},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str49, CSSPropertyGridRowGap},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str50, CSSPropertyZoom},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str51, CSSPropertyBorderTop},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str52, CSSPropertyWebkitBorderStart},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str53, CSSPropertyWebkitMask},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str54, CSSPropertyMinZoom},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str55, CSSPropertyAnimationDirection},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str56, CSSPropertyPaddingBottom},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str57, CSSPropertySrc},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str58, CSSPropertyWebkitMaskOrigin},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str59, CSSPropertyWebkitMaskImage},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str60, CSSPropertyBorderImageRepeat},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str61, CSSPropertyPosition},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str62, CSSPropertySize},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str63, CSSPropertyResize},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str64, CSSPropertySpeak},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str65, CSSPropertyIsolation},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str66, CSSPropertyTranslate},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str67, CSSPropertyWebkitPaddingEnd},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str68, CSSPropertyAlignItems},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str69, CSSPropertyTabSize},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str70, CSSPropertyAliasWebkitAnimationDirection},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str71, CSSPropertyWebkitRtlOrdering},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str72, CSSPropertyPointerEvents},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str73, CSSPropertyDominantBaseline},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str74, CSSPropertyPageBreakInside},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str75, CSSPropertyPaddingTop},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str76, CSSPropertyStrokeLinejoin},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str77, CSSPropertyStrokeMiterlimit},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str78, CSSPropertyWebkitPaddingStart},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str79, CSSPropertyColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str80, CSSPropertyClear},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str81, CSSPropertyWebkitLineBreak},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str82, CSSPropertyAliasWebkitAlignItems},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str83, CSSPropertyAlignContent},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str84, CSSPropertyAll},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str85, CSSPropertyGridTemplate},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str86, CSSPropertyColorRendering},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str87, CSSPropertyWebkitMaskRepeat},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str88, CSSPropertyBorderColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str89, CSSPropertyCaptionSide},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str90, CSSPropertyWordSpacing},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str91, CSSPropertyScale},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str92, CSSPropertyWebkitAppRegion},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str93, CSSPropertyBorderSpacing},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str94, CSSPropertyBorderBottomColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str95, CSSPropertyObjectPosition},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str96, CSSPropertyWebkitMaskPosition},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str97, CSSPropertyAnimationDuration},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str98, CSSPropertyGridTemplateRows},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str99, CSSPropertyGridTemplateAreas},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str100, CSSPropertyAliasWebkitAlignContent},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str101, CSSPropertyWebkitMaskSize},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str102, CSSPropertyQuotes},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str103, CSSPropertyBorderImageSlice},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str104, CSSPropertyWebkitBorderEndColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str105, CSSPropertyClip},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str106, CSSPropertyGridAutoRows},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str107, CSSPropertyAlignmentBaseline},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str108, CSSPropertyBorderRadius},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str109, CSSPropertyTransitionDuration},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str110, CSSPropertyBorderImageOutset},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str111, CSSPropertyAliasWebkitAnimationDuration},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str112, CSSPropertyBorderTopColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str113, CSSPropertyWebkitBorderStartColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str114, CSSPropertyWebkitUserDrag},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str115, CSSPropertyStopColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str116, CSSPropertyWebkitAppearance},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str117, CSSPropertyUnicodeRange},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str118, CSSPropertyLetterSpacing},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str119, CSSPropertyOutline},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str120, CSSPropertyUnicodeBidi},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str121, CSSPropertyAliasWebkitBorderRadius},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str122, CSSPropertyAliasWebkitTransitionDuration},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str123, CSSPropertyWebkitMaskComposite},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str124, CSSPropertyPerspective},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str125, CSSPropertyWebkitCaretColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str126, CSSPropertyCursor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str127, CSSPropertyAliasEpubWritingMode},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str128, CSSPropertyVerticalAlign},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str129, CSSPropertyAnimationIterationCount},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str130, CSSPropertyWebkitLocale},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str131, CSSPropertyPerspectiveOrigin},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str132, CSSPropertyStrokeLinecap},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str133, CSSPropertyBackground},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str134, CSSPropertyCounterReset},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str135, CSSPropertyBackgroundOrigin},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str136, CSSPropertyBackgroundImage},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str137, CSSPropertyUserZoom},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str138, CSSPropertyAliasEpubWordBreak},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str139, CSSPropertyRight},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str140, CSSPropertyBorderImageSource},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str141, CSSPropertyWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str142, CSSPropertyAliasWebkitPerspective},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str143, CSSPropertyFont},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str144, CSSPropertyMinWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str145, CSSPropertyMarginRight},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str146, CSSPropertyWebkitMaskClip},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str147, CSSPropertyAliasWebkitAnimationIterationCount},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str148, CSSPropertyAliasWebkitPerspectiveOrigin},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str149, CSSPropertyBorderRight},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str150, CSSPropertyBorderWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str151, CSSPropertyColorInterpolation},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str152, CSSPropertyBbRubberbandable},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str153, CSSPropertyWebkitBackgroundOrigin},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str154, CSSPropertyCounterIncrement},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str155, CSSPropertyBorderImageWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str156, CSSPropertyGridColumn},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str157, CSSPropertyFontKerning},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str158, CSSPropertyBorderBottomWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str159, CSSPropertyGridColumnEnd},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str160, CSSPropertyTransform},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str161, CSSPropertyFontVariant},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str162, CSSPropertyWebkitLineClamp},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str163, CSSPropertyBorderCollapse},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str164, CSSPropertyTransformOrigin},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str165, CSSPropertyBackgroundRepeat},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str166, CSSPropertyStrokeWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str167, CSSPropertyWebkitMarginAfter},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str168, CSSPropertyWebkitBorderEndWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str169, CSSPropertyWebkitBorderAfter},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str170, CSSPropertyWebkitMarginBefore},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str171, CSSPropertyGridColumnStart},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str172, CSSPropertyWebkitBorderBefore},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str173, CSSPropertyMotionPath},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str174, CSSPropertyPaddingRight},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str175, CSSPropertyAliasWebkitTransform},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str176, CSSPropertyWebkitMarginCollapse},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str177, CSSPropertyBackgroundPosition},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str178, CSSPropertyBackgroundBlendMode},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str179, CSSPropertyLeft},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str180, CSSPropertyFloat},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str181, CSSPropertyFilter},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str182, CSSPropertyAliasWebkitTransformOrigin},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str183, CSSPropertyBorderTopWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str184, CSSPropertyBackgroundSize},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str185, CSSPropertyObjectFit},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str186, CSSPropertyWebkitBorderStartWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str187, CSSPropertyWebkitColumns},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str188, CSSPropertyOrphans},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str189, CSSPropertyMarginLeft},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str190, CSSPropertyWebkitMarginBottomCollapse},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str191, CSSPropertyShapeMargin},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str192, CSSPropertyBorderLeft},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str193, CSSPropertyScrollSnapDestination},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str194, CSSPropertyShapeRendering},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str195, CSSPropertyGridColumnGap},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str196, CSSPropertyWebkitBorderVerticalSpacing},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str197, CSSPropertyOutlineColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str198, CSSPropertyPageBreakAfter},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str199, CSSPropertyFontSize},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str200, CSSPropertyOverflow},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str201, CSSPropertyWebkitUserSelect},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str202, CSSPropertyPageBreakBefore},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str203, CSSPropertyWebkitPaddingAfter},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str204, CSSPropertyWebkitFilter},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str205, CSSPropertyAliasEpubCaptionSide},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str206, CSSPropertyAliasWebkitBackgroundSize},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str207, CSSPropertyBackgroundColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str208, CSSPropertyWebkitPaddingBefore},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str209, CSSPropertyWebkitColumnBreakInside},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str210, CSSPropertyWebkitColumnGap},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str211, CSSPropertyAliasWebkitShapeMargin},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str212, CSSPropertyScrollSnapCoordinate},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str213, CSSPropertyWebkitMarginTopCollapse},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str214, CSSPropertyPaddingLeft},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str215, CSSPropertyWebkitColumnSpan},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str216, CSSPropertyFill},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str217, CSSPropertyWhiteSpace},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str218, CSSPropertyBorderRightColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str219, CSSPropertyWebkitTransformOriginZ},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str220, CSSPropertyClipRule},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str221, CSSPropertyWebkitPrintColorAdjust},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str222, CSSPropertyWebkitBackgroundComposite},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str223, CSSPropertyAnimationFillMode},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str224, CSSPropertyOverflowWrap},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str225, CSSPropertyBackgroundClip},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str226, CSSPropertyAlignSelf},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str227, CSSPropertyWebkitBorderAfterColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str228, CSSPropertyGridTemplateColumns},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str229, CSSPropertyWebkitBorderBeforeColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str230, CSSPropertyAliasWebkitAnimationFillMode},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str231, CSSPropertyWillChange},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str232, CSSPropertyWebkitBackgroundClip},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str233, CSSPropertyLightingColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str234, CSSPropertyGridAutoColumns},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str235, CSSPropertyAliasWebkitAlignSelf},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str236, CSSPropertyBorderBottomRightRadius},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str237, CSSPropertyFloodColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str238, CSSPropertyWebkitFontSizeDelta},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str239, CSSPropertyBorderLeftColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str240, CSSPropertyBackdropFilter},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str241, CSSPropertyOutlineWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str242, CSSPropertyX},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str243, CSSPropertyRx},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str244, CSSPropertyGridAutoFlow},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str245, CSSPropertyAnimationTimingFunction},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str246, CSSPropertyWebkitLogicalWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str247, CSSPropertyTextIndent},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str248, CSSPropertyAliasWebkitBorderBottomRightRadius},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str249, CSSPropertyClipPath},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str250, CSSPropertyTextRendering},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str251, CSSPropertyWebkitMinLogicalWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str252, CSSPropertyTextOrientation},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str253, CSSPropertyHeight},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str254, CSSPropertyBorderTopRightRadius},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str255, CSSPropertyMinHeight},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str256, CSSPropertyScrollBehavior},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str257, CSSPropertyShapeOutside},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str258, CSSPropertyTransitionTimingFunction},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str259, CSSPropertyFontWeight},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str260, CSSPropertyWebkitColumnCount},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str261, CSSPropertyTouchAction},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str262, CSSPropertyAliasWebkitAnimationTimingFunction},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str263, CSSPropertyBorderRightWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str264, CSSPropertyWebkitClipPath},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str265, CSSPropertyWebkitColumnRule},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str266, CSSPropertyWebkitBoxOrient},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str267, CSSPropertyWebkitTextOrientation},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str268, CSSPropertyBorderBottomLeftRadius},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str269, CSSPropertyFontSizeAdjust},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str270, CSSPropertyCx},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str271, CSSPropertyAliasWebkitBorderTopRightRadius},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str272, CSSPropertyFontVariantLigatures},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str273, CSSPropertyAliasWebkitShapeOutside},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str274, CSSPropertyAliasWebkitTransitionTimingFunction},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str275, CSSPropertyMotionOffset},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str276, CSSPropertyTextDecoration},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str277, CSSPropertyBackgroundAttachment},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str278, CSSPropertyZIndex},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str279, CSSPropertyMaxZoom},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str280, CSSPropertyTextAlign},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str281, CSSPropertyWebkitTextStroke},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str282, CSSPropertyWebkitBorderAfterWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str283, CSSPropertyWebkitColumnWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str284, CSSPropertyAliasWebkitBorderBottomLeftRadius},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str285, CSSPropertyWebkitBorderBeforeWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str286, CSSPropertyMixBlendMode},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str287, CSSPropertyWebkitFontSmoothing},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str288, CSSPropertyLineHeight},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str289, CSSPropertyWebkitMaskBoxImage},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str290, CSSPropertyBorderTopLeftRadius},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str291, CSSPropertyY},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str292, CSSPropertyRy},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str293, CSSPropertyWebkitMarginAfterCollapse},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str294, CSSPropertyBoxSizing},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str295, CSSPropertyWebkitBorderHorizontalSpacing},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str296, CSSPropertyWebkitTextCombine},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str297, CSSPropertyWebkitBoxDirection},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str298, CSSPropertyWebkitMarginBeforeCollapse},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str299, CSSPropertyFontStretch},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str300, CSSPropertyWebkitBoxAlign},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str301, CSSPropertyBorderLeftWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str302, CSSPropertyWebkitColumnBreakAfter},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str303, CSSPropertyWebkitColumnBreakBefore},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str304, CSSPropertyAliasWebkitBorderTopLeftRadius},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str305, CSSPropertyFillRule},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str306, CSSPropertyWebkitBoxLines},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str307, CSSPropertyAliasWebkitBoxSizing},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str308, CSSPropertyWebkitBoxDecorationBreak},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str309, CSSPropertyWebkitMaskRepeatX},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str310, CSSPropertyColorInterpolationFilters},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str311, CSSPropertyTextDecorationLine},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str312, CSSPropertyBaselineShift},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str313, CSSPropertyCy},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str314, CSSPropertyWebkitMaskBoxImageRepeat},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str315, CSSPropertyWebkitMaskPositionX},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str316, CSSPropertyTextAlignLast},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str317, CSSPropertyWebkitBoxPack},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str318, CSSPropertyVectorEffect},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str319, CSSPropertyWebkitColumnRuleColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str320, CSSPropertyAnimationDelay},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str321, CSSPropertyColumnFill},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str322, CSSPropertyBufferedRendering},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str323, CSSPropertyTextDecorationColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str324, CSSPropertyMaskType},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str325, CSSPropertyTransitionDelay},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str326, CSSPropertyBorderStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str327, CSSPropertyWebkitTextStrokeColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str328, CSSPropertyOpacity},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str329, CSSPropertyAliasWebkitAnimationDelay},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str330, CSSPropertyBorderBottomStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str331, CSSPropertyAliasEpubTextOrientation},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str332, CSSPropertyVisibility},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str333, CSSPropertyFontFeatureSettings},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str334, CSSPropertyAliasWebkitTransitionDelay},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str335, CSSPropertyWebkitMaskRepeatY},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str336, CSSPropertyWebkitMaskBoxImageSlice},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str337, CSSPropertyWebkitBorderEndStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str338, CSSPropertyAliasWebkitOpacity},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str339, CSSPropertyWebkitLogicalHeight},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str340, CSSPropertyDisplay},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str341, CSSPropertyTransitionProperty},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str342, CSSPropertyMaxWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str343, CSSPropertyWebkitMinLogicalHeight},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str344, CSSPropertyWebkitPerspectiveOriginX},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str345, CSSPropertyStrokeOpacity},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str346, CSSPropertyWebkitMaskPositionY},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str347, CSSPropertyWebkitMaskBoxImageOutset},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str348, CSSPropertyBorderTopStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str349, CSSPropertyAnimationPlayState},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str350, CSSPropertyWebkitBorderStartStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str351, CSSPropertyAliasWebkitFontFeatureSettings},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str352, CSSPropertyOutlineOffset},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str353, CSSPropertyTextShadow},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str354, CSSPropertyBoxShadow},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str355, CSSPropertyAliasEpubTextCombine},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str356, CSSPropertyAliasWebkitTransitionProperty},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str357, CSSPropertyTextTransform},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str358, CSSPropertyListStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str359, CSSPropertyBackgroundRepeatX},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str360, CSSPropertyListStyleImage},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str361, CSSPropertyWebkitColumnRuleWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str362, CSSPropertyStopOpacity},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str363, CSSPropertyAliasWebkitAnimationPlayState},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str364, CSSPropertyTextAnchor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str365, CSSPropertyTextUnderlinePosition},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str366, CSSPropertyAliasWebkitBoxShadow},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str367, CSSPropertyBackgroundPositionX},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str368, CSSPropertyFlex},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str369, CSSPropertyWebkitMaskBoxImageSource},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str370, CSSPropertyWebkitBoxOrdinalGroup},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str371, CSSPropertyWebkitTransformOriginX},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str372, CSSPropertyFlexGrow},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str373, CSSPropertyWebkitTextStrokeWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str374, CSSPropertyWebkitMaskBoxImageWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str375, CSSPropertyOverflowX},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str376, CSSPropertyAliasWebkitFlex},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str377, CSSPropertyTextOverflow},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str378, CSSPropertyAliasWebkitFlexGrow},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str379, CSSPropertyWebkitPerspectiveOriginY},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str380, CSSPropertyWebkitHighlight},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str381, CSSPropertyWebkitRubyPosition},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str382, CSSPropertyStrokeDashoffset},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str383, CSSPropertyListStylePosition},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str384, CSSPropertyFlexBasis},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str385, CSSPropertyEmptyCells},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str386, CSSPropertyFlexWrap},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str387, CSSPropertyScrollSnapPointsX},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str388, CSSPropertyFlexDirection},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str389, CSSPropertyWebkitTextEmphasis},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str390, CSSPropertyTableLayout},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str391, CSSPropertyBackgroundRepeatY},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str392, CSSPropertyAliasWebkitFlexBasis},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str393, CSSPropertyOutlineStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str394, CSSPropertyAliasWebkitFlexWrap},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str395, CSSPropertyAliasWebkitFlexDirection},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str396, CSSPropertyBackgroundPositionY},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str397, CSSPropertyWebkitBoxReflect},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str398, CSSPropertyStrokeDasharray},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str399, CSSPropertyWebkitTransformOriginY},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str400, CSSPropertyMaskSourceType},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str401, CSSPropertyShapeImageThreshold},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str402, CSSPropertyFontStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str403, CSSPropertyOverflowY},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str404, CSSPropertyBorderRightStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str405, CSSPropertyScrollSnapType},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str406, CSSPropertyWebkitTextEmphasisPosition},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str407, CSSPropertyTransformStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str408, CSSPropertyAliasWebkitShapeImageThreshold},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str409, CSSPropertyWebkitMaxLogicalWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str410, CSSPropertyScrollSnapPointsY},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str411, CSSPropertyMaxHeight},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str412, CSSPropertyAliasEpubTextTransform},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str413, CSSPropertyWebkitBorderAfterStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str414, CSSPropertyWebkitBorderBeforeStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str415, CSSPropertyAliasWebkitTransformStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str416, CSSPropertyWebkitTextEmphasisColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str417, CSSPropertyTextCombineUpright},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str418, CSSPropertyBorderLeftStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str419, CSSPropertyFloodOpacity},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str420, CSSPropertyWebkitTextFillColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str421, CSSPropertyWebkitTapHighlightColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str422, CSSPropertyJustifyItems},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str423, CSSPropertyWebkitUserModify},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str424, CSSPropertyAliasEpubTextEmphasis},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str425, CSSPropertyJustifyContent},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str426, CSSPropertyFillOpacity},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str427, CSSPropertyFlexShrink},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str428, CSSPropertyBackfaceVisibility},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str429, CSSPropertyAliasWebkitJustifyContent},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str430, CSSPropertyAliasWebkitFlexShrink},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str431, CSSPropertyFlexFlow},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str432, CSSPropertyAliasWebkitBackfaceVisibility},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str433, CSSPropertyAliasWebkitFlexFlow},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str434, CSSPropertyFontFamily},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str435, CSSPropertyWebkitTextDecorationsInEffect},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str436, CSSPropertyAliasEpubTextEmphasisColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str437, CSSPropertyWebkitColumnRuleStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str438, CSSPropertyWebkitMaxLogicalHeight},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str439, CSSPropertyTextDecorationStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str440, CSSPropertyWebkitBoxFlex},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str441, CSSPropertyWebkitTextSecurity},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str442, CSSPropertyJustifySelf},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str443, CSSPropertyListStyleType},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str444, CSSPropertyTextJustify},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str445, CSSPropertyWebkitBoxFlexGroup},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str446, CSSPropertyWebkitHyphenateCharacter},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str447, CSSPropertyWebkitTextEmphasisStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str448, CSSPropertyAliasEpubTextEmphasisStyle}
  };

static const short lookup[] =
  {
     -1,  -1,   0,  -1,  -1,  -1,  -1,  -1,  -1,   1,
     -1,   2,  -1,   3,   4,   5,  -1,  -1,  -1,  -1,
      6,   7,   8,   9,  10,  11,  -1,  -1,  -1,  12,
     13,  -1,  -1,  14,  15,  16,  -1,  -1,  17,  18,
     -1,  -1,  19,  20,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  21,  22,  -1,  23,  -1,  -1,  -1,  24,  -1,
     25,  -1,  -1,  -1,  26,  -1,  -1,  -1,  -1,  27,
     -1,  -1,  28,  -1,  -1,  -1,  -1,  -1,  29,  30,
     31,  -1,  32,  -1,  -1,  -1,  -1,  -1,  33,  -1,
     -1,  34,  -1,  35,  -1,  36,  -1,  37,  -1,  -1,
     -1,  -1,  -1,  -1,  38,  -1,  -1,  39,  -1,  -1,
     -1,  -1,  40,  -1,  -1,  41,  42,  43,  44,  -1,
     -1,  45,  46,  -1,  47,  48,  -1,  49,  50,  -1,
     51,  -1,  -1,  -1,  52,  -1,  53,  -1,  54,  -1,
     -1,  55,  56,  57,  -1,  -1,  -1,  -1,  -1,  -1,
     58,  59,  60,  61,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  62,  -1,  -1,  -1,  63,
     -1,  -1,  -1,  -1,  -1,  -1,  64,  65,  66,  -1,
     -1,  -1,  67,  68,  -1,  69,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  70,
     -1,  -1,  71,  -1,  72,  -1,  -1,  -1,  73,  -1,
     -1,  -1,  -1,  74,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  75,  -1,  76,  77,  78,  -1,  79,  80,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  81,  -1,  -1,
     -1,  82,  -1,  -1,  83,  84,  -1,  85,  86,  87,
     -1,  -1,  -1,  88,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  89,  -1,  -1,  90,  -1,  -1,  91,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  92,  93,  -1,  -1,  -1,
     94,  -1,  -1,  95,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  96,  -1,  -1,  -1,  97,  -1,  -1,  -1,  98,
     99,  -1, 100, 101, 102,  -1,  -1, 103,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    104,  -1,  -1, 105,  -1, 106,  -1,  -1, 107,  -1,
     -1,  -1, 108, 109,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1, 110,  -1,  -1,  -1,  -1,
     -1,  -1,  -1, 111,  -1,  -1,  -1,  -1,  -1, 112,
     -1,  -1,  -1, 113,  -1,  -1,  -1,  -1,  -1,  -1,
    114,  -1,  -1,  -1, 115, 116,  -1,  -1,  -1, 117,
     -1,  -1,  -1, 118,  -1,  -1, 119,  -1, 120,  -1,
    121, 122,  -1, 123, 124,  -1, 125,  -1,  -1,  -1,
     -1,  -1, 126,  -1, 127, 128, 129, 130, 131,  -1,
     -1, 132, 133,  -1,  -1,  -1, 134,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1, 135, 136,  -1,  -1,
    137,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 138,  -1,
     -1,  -1, 139, 140,  -1,  -1, 141,  -1,  -1,  -1,
     -1,  -1, 142, 143,  -1,  -1, 144,  -1,  -1, 145,
     -1, 146,  -1,  -1, 147,  -1, 148,  -1, 149,  -1,
     -1,  -1, 150, 151,  -1,  -1,  -1, 152,  -1,  -1,
     -1,  -1,  -1,  -1, 153, 154,  -1, 155,  -1,  -1,
     -1,  -1,  -1,  -1,  -1, 156,  -1, 157,  -1, 158,
     -1,  -1,  -1,  -1, 159, 160,  -1,  -1,  -1,  -1,
    161,  -1,  -1,  -1,  -1, 162, 163,  -1,  -1, 164,
     -1,  -1,  -1,  -1,  -1, 165,  -1, 166,  -1,  -1,
     -1, 167,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 168,
    169,  -1,  -1, 170,  -1,  -1,  -1, 171,  -1,  -1,
     -1,  -1, 172,  -1,  -1, 173,  -1,  -1,  -1, 174,
     -1,  -1,  -1, 175,  -1, 176,  -1, 177, 178,  -1,
     -1,  -1, 179,  -1,  -1, 180, 181, 182, 183, 184,
     -1, 185, 186, 187, 188,  -1,  -1,  -1,  -1, 189,
     -1,  -1, 190,  -1,  -1,  -1,  -1, 191, 192,  -1,
    193, 194, 195,  -1,  -1,  -1,  -1,  -1,  -1, 196,
     -1,  -1,  -1,  -1,  -1, 197,  -1, 198,  -1,  -1,
    199,  -1, 200,  -1,  -1,  -1,  -1,  -1, 201, 202,
     -1, 203,  -1,  -1, 204, 205,  -1, 206,  -1,  -1,
     -1, 207,  -1, 208,  -1,  -1,  -1,  -1, 209, 210,
     -1,  -1,  -1,  -1,  -1, 211,  -1, 212,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1, 213,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 214,
    215, 216,  -1, 217,  -1,  -1,  -1, 218,  -1, 219,
     -1,  -1,  -1,  -1,  -1, 220,  -1,  -1,  -1,  -1,
    221,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1, 222, 223,  -1,
     -1,  -1,  -1,  -1, 224,  -1,  -1, 225,  -1,  -1,
     -1,  -1,  -1, 226,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 227,
     -1,  -1,  -1,  -1, 228,  -1,  -1,  -1,  -1,  -1,
     -1, 229,  -1,  -1,  -1,  -1, 230,  -1,  -1,  -1,
     -1,  -1,  -1,  -1, 231, 232, 233,  -1,  -1,  -1,
    234, 235,  -1, 236, 237,  -1,  -1,  -1,  -1,  -1,
     -1, 238,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1, 239,  -1,  -1,
     -1, 240,  -1,  -1, 241,  -1,  -1,  -1,  -1, 242,
     -1, 243,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    244,  -1, 245,  -1,  -1,  -1,  -1, 246,  -1,  -1,
    247, 248,  -1,  -1, 249,  -1, 250, 251,  -1,  -1,
    252,  -1,  -1,  -1,  -1,  -1, 253,  -1,  -1,  -1,
     -1,  -1, 254,  -1,  -1,  -1, 255,  -1, 256, 257,
    258,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1, 259,  -1,  -1, 260, 261,  -1,  -1,  -1,
    262,  -1,  -1,  -1,  -1,  -1, 263,  -1,  -1,  -1,
     -1,  -1, 264,  -1, 265,  -1, 266,  -1, 267,  -1,
     -1,  -1,  -1, 268,  -1, 269,  -1,  -1,  -1, 270,
    271,  -1, 272,  -1,  -1,  -1,  -1, 273, 274,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1, 275,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1, 276, 277,  -1,
    278,  -1,  -1,  -1,  -1,  -1, 279, 280,  -1,  -1,
     -1,  -1,  -1,  -1, 281,  -1,  -1,  -1, 282,  -1,
    283, 284,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    285, 286,  -1,  -1, 287, 288,  -1, 289,  -1,  -1,
     -1,  -1, 290,  -1,  -1,  -1,  -1,  -1,  -1, 291,
     -1, 292,  -1, 293,  -1, 294, 295,  -1,  -1,  -1,
    296, 297,  -1,  -1,  -1, 298,  -1,  -1, 299,  -1,
     -1,  -1,  -1,  -1, 300,  -1, 301,  -1,  -1,  -1,
     -1,  -1, 302,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1, 303,  -1,  -1,  -1,  -1,  -1,
    304,  -1,  -1, 305,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1, 306, 307,  -1,  -1, 308,  -1,  -1,  -1,
    309,  -1, 310,  -1,  -1,  -1, 311,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1, 312,  -1,  -1,  -1,  -1, 313,
    314,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1, 315,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1, 316, 317,  -1,  -1,
     -1, 318,  -1, 319,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1, 320,  -1,  -1,  -1,  -1, 321,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1, 322,  -1,  -1,  -1, 323,  -1,  -1,  -1,
     -1,  -1,  -1, 324,  -1,  -1,  -1,  -1,  -1,  -1,
    325, 326,  -1, 327,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 328,  -1,
    329,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 330,  -1,
     -1,  -1,  -1,  -1, 331, 332,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1, 333,  -1,  -1,  -1, 334,  -1,
    335,  -1,  -1,  -1,  -1, 336,  -1,  -1, 337,  -1,
     -1,  -1,  -1,  -1,  -1,  -1, 338, 339,  -1, 340,
     -1, 341,  -1,  -1, 342,  -1,  -1, 343,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1, 344,  -1, 345,
     -1,  -1, 346, 347,  -1,  -1,  -1, 348,  -1, 349,
     -1, 350, 351,  -1,  -1,  -1,  -1,  -1,  -1, 352,
     -1,  -1,  -1,  -1,  -1, 353,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1, 354,  -1, 355,  -1,  -1, 356,
     -1,  -1, 357, 358,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1, 359,  -1, 360,  -1,
     -1,  -1, 361,  -1,  -1, 362,  -1, 363,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    364,  -1,  -1, 365,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1, 366,  -1,  -1,  -1,  -1,  -1, 367, 368,
     -1, 369, 370,  -1,  -1,  -1,  -1,  -1, 371,  -1,
     -1,  -1, 372,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1, 373,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1, 374,  -1,  -1,  -1,  -1,
     -1,  -1,  -1, 375,  -1,  -1,  -1, 376,  -1, 377,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    378,  -1,  -1,  -1,  -1,  -1,  -1, 379,  -1,  -1,
     -1, 380,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1, 381, 382,  -1,  -1,  -1,  -1, 383,  -1,
     -1, 384,  -1,  -1, 385,  -1,  -1,  -1,  -1,  -1,
     -1, 386,  -1,  -1,  -1,  -1,  -1, 387, 388,  -1,
     -1,  -1, 389,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1, 390, 391,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 392,
     -1,  -1,  -1, 393,  -1,  -1,  -1,  -1,  -1, 394,
     -1,  -1,  -1,  -1,  -1,  -1, 395,  -1, 396,  -1,
    397, 398,  -1,  -1,  -1,  -1,  -1,  -1, 399,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1, 400,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 401,  -1,
    402,  -1,  -1, 403,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1, 404,  -1,  -1,  -1,  -1,
    405,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1, 406,  -1,  -1,
     -1,  -1, 407,  -1,  -1,  -1, 408,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1, 409,  -1, 410,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1, 411,  -1, 412, 413,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 414,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    415,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1, 416,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1, 417,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1, 418,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1, 419,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1, 420,  -1, 421, 422,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1, 423,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 424,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 425,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1, 426, 427,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1, 428,  -1, 429,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    430,  -1,  -1,  -1,  -1,  -1, 431,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1, 432,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1, 433,  -1,  -1,  -1,  -1,  -1,
     -1, 434,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1, 435,  -1,  -1, 436,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1, 437,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1, 438,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1, 439,  -1,  -1,  -1,  -1,  -1,
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
     -1,  -1,  -1, 440,  -1,  -1,  -1,  -1, 441,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 442,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 443,  -1,
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
     -1,  -1, 444,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1, 445,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1, 446,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 447,
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
     -1,  -1,  -1,  -1,  -1, 448
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
