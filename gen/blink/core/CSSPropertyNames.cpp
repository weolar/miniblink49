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
    "glyph-orientation-horizontal\0"
    "glyph-orientation-vertical\0"
    "grid-auto-columns\0"
    "grid-auto-flow\0"
    "grid-auto-rows\0"
    "grid-column-end\0"
    "grid-column-start\0"
    "grid-row-end\0"
    "grid-row-start\0"
    "grid-template-areas\0"
    "grid-template-columns\0"
    "grid-template-rows\0"
    "height\0"
    "image-rendering\0"
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
    170,
    193,
    208,
    233,
    254,
    269,
    274,
    288,
    300,
    319,
    330,
    346,
    366,
    385,
    405,
    431,
    446,
    467,
    493,
    513,
    535,
    557,
    573,
    590,
    607,
    625,
    647,
    669,
    689,
    709,
    725,
    740,
    760,
    786,
    813,
    833,
    853,
    869,
    889,
    909,
    928,
    948,
    967,
    985,
    1003,
    1021,
    1040,
    1059,
    1078,
    1095,
    1118,
    1142,
    1159,
    1176,
    1183,
    1194,
    1205,
    1224,
    1237,
    1243,
    1248,
    1258,
    1268,
    1288,
    1316,
    1332,
    1344,
    1352,
    1370,
    1384,
    1391,
    1394,
    1397,
    1405,
    1423,
    1435,
    1440,
    1453,
    1463,
    1470,
    1481,
    1496,
    1506,
    1518,
    1528,
    1534,
    1546,
    1560,
    1589,
    1616,
    1634,
    1649,
    1664,
    1680,
    1698,
    1711,
    1726,
    1746,
    1768,
    1787,
    1794,
    1810,
    1820,
    1836,
    1850,
    1863,
    1868,
    1883,
    1898,
    1910,
    1927,
    1947,
    1963,
    1977,
    1989,
    2002,
    2013,
    2024,
    2035,
    2048,
    2053,
    2070,
    2080,
    2091,
    2101,
    2112,
    2122,
    2137,
    2151,
    2163,
    2179,
    2190,
    2206,
    2214,
    2220,
    2228,
    2242,
    2257,
    2271,
    2285,
    2299,
    2310,
    2321,
    2336,
    2349,
    2363,
    2375,
    2392,
    2410,
    2428,
    2440,
    2452,
    2471,
    2486,
    2495,
    2502,
    2509,
    2515,
    2517,
    2520,
    2523,
    2539,
    2556,
    2573,
    2594,
    2615,
    2639,
    2662,
    2684,
    2697,
    2711,
    2727,
    2732,
    2738,
    2749,
    2762,
    2769,
    2786,
    2804,
    2819,
    2835,
    2853,
    2868,
    2881,
    2894,
    2903,
    2914,
    2930,
    2942,
    2958,
    2980,
    3001,
    3023,
    3035,
    3048,
    3062,
    3074,
    3089,
    3113,
    3117,
    3130,
    3140,
    3157,
    3173,
    3183,
    3190,
    3196,
    3213,
    3233,
    3253,
    3280,
    3293,
    3307,
    3322,
    3333,
    3335,
    3337,
    3356,
    3375,
    3399,
    3428,
    3454,
    3488,
    3509,
    3541,
    3559,
    3588,
    3610,
    3627,
    3650,
    3668,
    3694,
    3713,
    3730,
    3750,
    3768,
    3795,
    3823,
    3851,
    3872,
    3891,
    3917,
    3943,
    3969,
    3989,
    4010,
    4025,
    4043,
    4071,
    4096,
    4115,
    4134,
    4164,
    4195,
    4226,
    4254,
    4284,
    4314,
    4343,
    4373,
    4402,
    4420,
    4443,
    4462,
    4482,
    4506,
    4530,
    4552,
    4574,
    4592,
    4621,
    4650,
    4677,
    4698,
    4720,
    4748,
    4769,
    4797,
    4828,
    4856,
    4880,
    4902,
    4928,
    4954,
    4981,
    5008,
    5035,
    5053,
    5073,
    5093,
    5105,
    5112,
    5118,
    5130,
    5141,
    5154,
    5164,
    5177,
    5185,
    5210,
    5235,
    5260,
    5287,
    5314,
    5341,
    5369,
    5397,
    5425,
    5452,
    5479,
    5506,
    5525,
    5546,
    5568,
    5589,
    5609,
    5631,
    5654,
    5676,
    5698,
    5721,
    5747,
    5774,
    5800,
    5827,
    5831,
    5849,
    5858,
    5867,
    5879,
    5884,
    5888,
    5902,
    5912,
    5936,
    5971,
    5981,
    5992,
    6012,
    6030,
    6037,
    6051,
    6064,
    6077,
    6089,
    6103,
    6116,
    6131,
    6144,
    6155,
    6168,
    6173,
    6183,
    6188,
    6193,
    6203,
    6215,
    6224,
    6238,
    6249,
    6256,
    6263,
    6270,
    6278,
    6287,
    6295,
    6306,
    6327,
    6349,
    6368,
    6389,
    6409,
    6425,
    6449,
    6462,
    6485,
    6507,
    6527,
    6549,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6569,
    6592,
    6611,
    6611,
    6611,
    6633,
    6653,
    6653,
    6672,
    6696,
    6724,
    6751,
    6779,
    6813,
    6836,
    6865,
    6899,
    6927,
    6927,
    6927,
    6927,
    6927,
    6927,
    6927,
    6927,
    6927,
    6927,
    6927,
    6951,
    6951,
    6951,
    6985,
    7020,
    7020,
    7020,
    7020,
    7020,
    7020,
    7020,
    7020,
    7020,
    7020,
    7020,
    7020,
    7020,
    7020,
    7020,
    7020,
    7051,
    7083,
    7083,
    7083,
    7083,
    7102,
    7121,
    7121,
    7140,
    7140,
    7140,
    7140,
    7140,
    7140,
    7140,
    7140,
    7140,
    7140,
    7140,
    7140,
    7140,
    7140,
    7140,
    7140,
    7140,
    7140,
    7140,
    7140,
    7140,
    7140,
    7159,
    7182,
    7200,
    7220,
    7238,
    7238,
    7238,
    7238,
    7238,
    7238,
    7238,
    7238,
    7238,
    7238,
    7238,
    7238,
    7238,
    7238,
    7238,
    7238,
    7238,
    7238,
    7238,
    7262,
    7262,
    7262,
    7262,
    7262,
    7262,
    7262,
    7262,
    7262,
    7262,
    7262,
    7262,
    7262,
    7262,
    7262,
    7262,
    7262,
    7262,
    7262,
    7262,
    7262,
    7262,
    7262,
    7262,
    7262,
    7262,
    7262,
    7262,
    7262,
    7262,
    7278,
    7292,
    7292,
    7292,
    7292,
    7292,
    7292,
    7292,
    7292,
    7292,
    7292,
    7292,
    7292,
    7292,
    7292,
    7292,
    7292,
    7292,
    7312,
    7339,
    7339,
    7339,
    7339,
    7339,
    7339,
    7339,
    7339,
    7339,
    7339,
    7339,
    7339,
    7339,
    7339,
    7339,
    7339,
    7369,
    7390,
    7412,
    7412,
    7412,
    7412,
    7412,
    7412,
    7412,
    7412,
    7412,
    7412,
    7412,
    7412,
    7412,
    7412,
    7412,
    7412,
    7412,
    7412,
    7412,
    7412,
    7412,
    7412,
    7412,
    7412,
    7412,
    7412,
    7412,
    7433,
    7433,
    7433,
    7433,
    7451,
    7476,
    7500,
    7500,
    7500,
    7500,
    7525,
    7553,
    7581,
    7616,
    7616,
    7616,
    7616,
    7616,
    7616,
    7616,
    7616,
    7616,
    7616,
    7616,
    7616,
    7616,
    7616,
    7616,
    7616,
    7616,
    7616,
    7616,
    7616,
    7616,
    7616,
    7616,
    7616,
    7616,
    7616,
    7616,
    7616,
    7616,
    7616,
    7616,
    7616,
    7616,
    7616,
    7616,
    7616,
    7616,
    7616,
    7616,
    7616,
    7616,
    7616,
    7616,
    7616,
    7616,
    7616,
    7616,
    7616,
    7616,
    7616,
    7616,
    7616,
    7616,
    7616,
    7616,
    7616,
    7616,
    7616,
    7616,
    7616,
    7616,
    7616,
    7616,
    7616,
    7616,
    7616,
    7635,
    7661,
    7661,
    7687,
    7687,
    7687,
    7687,
    7687,
    7687,
    7687,
    7687,
    7687,
    7687,
    7687,
    7687,
    7687,
    7687,
    7687,
    7704,
    7704,
    7704,
    7704,
    7704,
    7704,
    7704,
    7704,
    7704,
    7704,
    7704,
    7704,
    7704,
    7704,
    7704,
    7704,
    7704,
    7704,
    7704,
    7704,
    7704,
    7704,
    7704,
    7704,
    7704,
    7704,
    7704,
    7704,
    7704,
    7704,
    7704,
    7704,
    7704,
    7704,
    7704,
    7704,
    7704,
    7704,
    7704,
    7704,
    7704,
    7704,
    7722,
    7722,
    7722,
    7722,
    7722,
    7722,
    7722,
    7722,
    7722,
    7744,
    7744,
    7744,
    7744,
    7744,
    7744,
    7757,
    7775,
    7775,
    7775,
    7775,
    7775,
    7775,
    7775,
    7775,
    7775,
    7775,
    7775,
    7775,
    7775,
    7775,
    7794,
    7794,
    7794,
    7794,
    7794,
    7794,
    7794,
    7794,
    7794,
    7794,
    7794,
    7794,
};

enum
  {
    TOTAL_KEYWORDS = 444,
    MIN_WORD_LENGTH = 1,
    MAX_WORD_LENGTH = 34,
    MIN_HASH_VALUE = 10,
    MAX_HASH_VALUE = 2849
  };

/* maximum key range = 2840, duplicates = 0 */

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
      2850, 2850, 2850, 2850, 2850, 2850, 2850, 2850, 2850, 2850,
      2850, 2850, 2850, 2850, 2850, 2850, 2850, 2850, 2850, 2850,
      2850, 2850, 2850, 2850, 2850, 2850, 2850, 2850, 2850, 2850,
      2850, 2850, 2850, 2850, 2850, 2850, 2850, 2850, 2850, 2850,
      2850, 2850, 2850, 2850, 2850,    7, 2850, 2850, 2850, 2850,
      2850, 2850, 2850, 2850, 2850, 2850, 2850, 2850, 2850, 2850,
      2850, 2850, 2850, 2850, 2850, 2850, 2850, 2850, 2850, 2850,
      2850, 2850, 2850, 2850, 2850, 2850, 2850, 2850, 2850, 2850,
      2850, 2850, 2850, 2850, 2850, 2850, 2850, 2850, 2850, 2850,
      2850, 2850, 2850, 2850, 2850, 2850, 2850,    7,    7,  116,
         7,    7,  451,    7,  328,    7,    7,    7,  169,   13,
         8,    8,   78,    7,   10,   43,    7,  196,   39,    7,
       846,  914,   51, 2850, 2850, 2850, 2850, 2850, 2850, 2850,
      2850, 2850, 2850, 2850, 2850, 2850, 2850, 2850, 2850, 2850,
      2850, 2850, 2850, 2850, 2850, 2850, 2850, 2850, 2850, 2850,
      2850, 2850, 2850, 2850, 2850, 2850, 2850, 2850, 2850, 2850,
      2850, 2850, 2850, 2850, 2850, 2850, 2850, 2850, 2850, 2850,
      2850, 2850, 2850, 2850, 2850, 2850, 2850, 2850, 2850, 2850,
      2850, 2850, 2850, 2850, 2850, 2850, 2850, 2850, 2850, 2850,
      2850, 2850, 2850, 2850, 2850, 2850, 2850, 2850, 2850, 2850,
      2850, 2850, 2850, 2850, 2850, 2850, 2850, 2850, 2850, 2850,
      2850, 2850, 2850, 2850, 2850, 2850, 2850, 2850, 2850, 2850,
      2850, 2850, 2850, 2850, 2850, 2850, 2850, 2850, 2850, 2850,
      2850, 2850, 2850, 2850, 2850, 2850, 2850, 2850, 2850, 2850,
      2850, 2850, 2850, 2850, 2850, 2850, 2850, 2850, 2850, 2850,
      2850, 2850, 2850, 2850, 2850, 2850
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
    char stringpool_str4[sizeof("border")];
    char stringpool_str5[sizeof("bottom")];
    char stringpool_str6[sizeof("motion")];
    char stringpool_str7[sizeof("margin")];
    char stringpool_str8[sizeof("marker")];
    char stringpool_str9[sizeof("grid-row")];
    char stringpool_str10[sizeof("grid-area")];
    char stringpool_str11[sizeof("mask")];
    char stringpool_str12[sizeof("animation")];
    char stringpool_str13[sizeof("word-break")];
    char stringpool_str14[sizeof("widows")];
    char stringpool_str15[sizeof("zoom")];
    char stringpool_str16[sizeof("stroke")];
    char stringpool_str17[sizeof("marker-end")];
    char stringpool_str18[sizeof("orientation")];
    char stringpool_str19[sizeof("marker-mid")];
    char stringpool_str20[sizeof("grid-row-end")];
    char stringpool_str21[sizeof("top")];
    char stringpool_str22[sizeof("writing-mode")];
    char stringpool_str23[sizeof("border-image")];
    char stringpool_str24[sizeof("-webkit-order")];
    char stringpool_str25[sizeof("page")];
    char stringpool_str26[sizeof("border-bottom")];
    char stringpool_str27[sizeof("size")];
    char stringpool_str28[sizeof("margin-bottom")];
    char stringpool_str29[sizeof("transition")];
    char stringpool_str30[sizeof("animation-name")];
    char stringpool_str31[sizeof("min-zoom")];
    char stringpool_str32[sizeof("image-rendering")];
    char stringpool_str33[sizeof("motion-rotation")];
    char stringpool_str34[sizeof("padding")];
    char stringpool_str35[sizeof("resize")];
    char stringpool_str36[sizeof("-webkit-mask")];
    char stringpool_str37[sizeof("-webkit-animation")];
    char stringpool_str38[sizeof("-webkit-border-end")];
    char stringpool_str39[sizeof("marker-start")];
    char stringpool_str40[sizeof("tab-size")];
    char stringpool_str41[sizeof("-webkit-margin-end")];
    char stringpool_str42[sizeof("word-wrap")];
    char stringpool_str43[sizeof("speak")];
    char stringpool_str44[sizeof("grid-row-start")];
    char stringpool_str45[sizeof("border-top")];
    char stringpool_str46[sizeof("-webkit-writing-mode")];
    char stringpool_str47[sizeof("margin-top")];
    char stringpool_str48[sizeof("-webkit-border-image")];
    char stringpool_str49[sizeof("paint-order")];
    char stringpool_str50[sizeof("content")];
    char stringpool_str51[sizeof("position")];
    char stringpool_str52[sizeof("-webkit-transition")];
    char stringpool_str53[sizeof("src")];
    char stringpool_str54[sizeof("-webkit-animation-name")];
    char stringpool_str55[sizeof("-webkit-mask-image")];
    char stringpool_str56[sizeof("direction")];
    char stringpool_str57[sizeof("padding-bottom")];
    char stringpool_str58[sizeof("-webkit-mask-origin")];
    char stringpool_str59[sizeof("-webkit-border-start")];
    char stringpool_str60[sizeof("-webkit-margin-start")];
    char stringpool_str61[sizeof("-webkit-padding-end")];
    char stringpool_str62[sizeof("border-image-repeat")];
    char stringpool_str63[sizeof("padding-top")];
    char stringpool_str64[sizeof("page-break-inside")];
    char stringpool_str65[sizeof("-webkit-mask-size")];
    char stringpool_str66[sizeof("pointer-events")];
    char stringpool_str67[sizeof("-webkit-mask-repeat")];
    char stringpool_str68[sizeof("animation-direction")];
    char stringpool_str69[sizeof("-webkit-padding-start")];
    char stringpool_str70[sizeof("isolation")];
    char stringpool_str71[sizeof("translate")];
    char stringpool_str72[sizeof("quotes")];
    char stringpool_str73[sizeof("-webkit-app-region")];
    char stringpool_str74[sizeof("align-items")];
    char stringpool_str75[sizeof("-webkit-line-break")];
    char stringpool_str76[sizeof("-webkit-mask-position")];
    char stringpool_str77[sizeof("caption-side")];
    char stringpool_str78[sizeof("word-spacing")];
    char stringpool_str79[sizeof("clear")];
    char stringpool_str80[sizeof("stroke-linejoin")];
    char stringpool_str81[sizeof("color")];
    char stringpool_str82[sizeof("-webkit-animation-direction")];
    char stringpool_str83[sizeof("-webkit-rtl-ordering")];
    char stringpool_str84[sizeof("border-spacing")];
    char stringpool_str85[sizeof("object-position")];
    char stringpool_str86[sizeof("border-radius")];
    char stringpool_str87[sizeof("dominant-baseline")];
    char stringpool_str88[sizeof("animation-duration")];
    char stringpool_str89[sizeof("grid-auto-rows")];
    char stringpool_str90[sizeof("grid-template")];
    char stringpool_str91[sizeof("stroke-miterlimit")];
    char stringpool_str92[sizeof("-webkit-align-items")];
    char stringpool_str93[sizeof("scale")];
    char stringpool_str94[sizeof("user-zoom")];
    char stringpool_str95[sizeof("all")];
    char stringpool_str96[sizeof("-webkit-user-drag")];
    char stringpool_str97[sizeof("width")];
    char stringpool_str98[sizeof("right")];
    char stringpool_str99[sizeof("align-content")];
    char stringpool_str100[sizeof("border-color")];
    char stringpool_str101[sizeof("transition-duration")];
    char stringpool_str102[sizeof("clip")];
    char stringpool_str103[sizeof("border-image-outset")];
    char stringpool_str104[sizeof("background")];
    char stringpool_str105[sizeof("-epub-word-break")];
    char stringpool_str106[sizeof("-webkit-appearance")];
    char stringpool_str107[sizeof("-webkit-border-radius")];
    char stringpool_str108[sizeof("cursor")];
    char stringpool_str109[sizeof("unicode-bidi")];
    char stringpool_str110[sizeof("-webkit-animation-duration")];
    char stringpool_str111[sizeof("color-rendering")];
    char stringpool_str112[sizeof("min-width")];
    char stringpool_str113[sizeof("unicode-range")];
    char stringpool_str114[sizeof("-epub-writing-mode")];
    char stringpool_str115[sizeof("perspective")];
    char stringpool_str116[sizeof("outline")];
    char stringpool_str117[sizeof("grid-template-rows")];
    char stringpool_str118[sizeof("border-width")];
    char stringpool_str119[sizeof("grid-template-areas")];
    char stringpool_str120[sizeof("border-right")];
    char stringpool_str121[sizeof("margin-right")];
    char stringpool_str122[sizeof("-webkit-mask-composite")];
    char stringpool_str123[sizeof("background-image")];
    char stringpool_str124[sizeof("-webkit-align-content")];
    char stringpool_str125[sizeof("border-bottom-color")];
    char stringpool_str126[sizeof("-webkit-transition-duration")];
    char stringpool_str127[sizeof("background-origin")];
    char stringpool_str128[sizeof("counter-reset")];
    char stringpool_str129[sizeof("stroke-width")];
    char stringpool_str130[sizeof("border-image-slice")];
    char stringpool_str131[sizeof("-webkit-border-end-color")];
    char stringpool_str132[sizeof("perspective-origin")];
    char stringpool_str133[sizeof("stop-color")];
    char stringpool_str134[sizeof("-webkit-perspective")];
    char stringpool_str135[sizeof("border-image-width")];
    char stringpool_str136[sizeof("border-top-color")];
    char stringpool_str137[sizeof("border-bottom-width")];
    char stringpool_str138[sizeof("font")];
    char stringpool_str139[sizeof("motion-path")];
    char stringpool_str140[sizeof("letter-spacing")];
    char stringpool_str141[sizeof("stroke-linecap")];
    char stringpool_str142[sizeof("orphans")];
    char stringpool_str143[sizeof("-webkit-background-origin")];
    char stringpool_str144[sizeof("border-image-source")];
    char stringpool_str145[sizeof("padding-right")];
    char stringpool_str146[sizeof("background-size")];
    char stringpool_str147[sizeof("animation-iteration-count")];
    char stringpool_str148[sizeof("alignment-baseline")];
    char stringpool_str149[sizeof("background-repeat")];
    char stringpool_str150[sizeof("-webkit-border-end-width")];
    char stringpool_str151[sizeof("-webkit-mask-clip")];
    char stringpool_str152[sizeof("-webkit-border-start-color")];
    char stringpool_str153[sizeof("-webkit-perspective-origin")];
    char stringpool_str154[sizeof("border-top-width")];
    char stringpool_str155[sizeof("shape-margin")];
    char stringpool_str156[sizeof("-webkit-locale")];
    char stringpool_str157[sizeof("font-kerning")];
    char stringpool_str158[sizeof("shape-rendering")];
    char stringpool_str159[sizeof("counter-increment")];
    char stringpool_str160[sizeof("-webkit-background-size")];
    char stringpool_str161[sizeof("-webkit-animation-iteration-count")];
    char stringpool_str162[sizeof("background-position")];
    char stringpool_str163[sizeof("grid-column")];
    char stringpool_str164[sizeof("-webkit-border-start-width")];
    char stringpool_str165[sizeof("transform")];
    char stringpool_str166[sizeof("font-variant")];
    char stringpool_str167[sizeof("vertical-align")];
    char stringpool_str168[sizeof("grid-column-end")];
    char stringpool_str169[sizeof("-webkit-shape-margin")];
    char stringpool_str170[sizeof("enable-background")];
    char stringpool_str171[sizeof("font-size")];
    char stringpool_str172[sizeof("-webkit-border-after")];
    char stringpool_str173[sizeof("-webkit-margin-after")];
    char stringpool_str174[sizeof("-webkit-border-before")];
    char stringpool_str175[sizeof("-epub-caption-side")];
    char stringpool_str176[sizeof("-webkit-margin-before")];
    char stringpool_str177[sizeof("-webkit-columns")];
    char stringpool_str178[sizeof("transform-origin")];
    char stringpool_str179[sizeof("-webkit-transform")];
    char stringpool_str180[sizeof("white-space")];
    char stringpool_str181[sizeof("background-blend-mode")];
    char stringpool_str182[sizeof("object-fit")];
    char stringpool_str183[sizeof("grid-column-start")];
    char stringpool_str184[sizeof("page-break-after")];
    char stringpool_str185[sizeof("left")];
    char stringpool_str186[sizeof("-webkit-line-clamp")];
    char stringpool_str187[sizeof("page-break-before")];
    char stringpool_str188[sizeof("float")];
    char stringpool_str189[sizeof("color-interpolation")];
    char stringpool_str190[sizeof("filter")];
    char stringpool_str191[sizeof("border-collapse")];
    char stringpool_str192[sizeof("-webkit-column-gap")];
    char stringpool_str193[sizeof("-webkit-padding-after")];
    char stringpool_str194[sizeof("-webkit-transform-origin")];
    char stringpool_str195[sizeof("-webkit-user-select")];
    char stringpool_str196[sizeof("-webkit-padding-before")];
    char stringpool_str197[sizeof("height")];
    char stringpool_str198[sizeof("border-left")];
    char stringpool_str199[sizeof("background-color")];
    char stringpool_str200[sizeof("margin-left")];
    char stringpool_str201[sizeof("-webkit-column-break-inside")];
    char stringpool_str202[sizeof("overflow")];
    char stringpool_str203[sizeof("-webkit-filter")];
    char stringpool_str204[sizeof("-webkit-column-span")];
    char stringpool_str205[sizeof("-webkit-margin-collapse")];
    char stringpool_str206[sizeof("min-height")];
    char stringpool_str207[sizeof("outline-color")];
    char stringpool_str208[sizeof("-webkit-background-composite")];
    char stringpool_str209[sizeof("-webkit-transform-origin-z")];
    char stringpool_str210[sizeof("border-right-color")];
    char stringpool_str211[sizeof("shape-outside")];
    char stringpool_str212[sizeof("-webkit-border-vertical-spacing")];
    char stringpool_str213[sizeof("border-bottom-right-radius")];
    char stringpool_str214[sizeof("background-clip")];
    char stringpool_str215[sizeof("-webkit-print-color-adjust")];
    char stringpool_str216[sizeof("clip-rule")];
    char stringpool_str217[sizeof("padding-left")];
    char stringpool_str218[sizeof("outline-width")];
    char stringpool_str219[sizeof("-webkit-margin-bottom-collapse")];
    char stringpool_str220[sizeof("border-right-width")];
    char stringpool_str221[sizeof("scroll-snap-destination")];
    char stringpool_str222[sizeof("border-top-right-radius")];
    char stringpool_str223[sizeof("fill")];
    char stringpool_str224[sizeof("clip-path")];
    char stringpool_str225[sizeof("-webkit-shape-outside")];
    char stringpool_str226[sizeof("-webkit-border-bottom-right-radius")];
    char stringpool_str227[sizeof("-webkit-background-clip")];
    char stringpool_str228[sizeof("overflow-wrap")];
    char stringpool_str229[sizeof("-webkit-margin-top-collapse")];
    char stringpool_str230[sizeof("touch-action")];
    char stringpool_str231[sizeof("grid-auto-columns")];
    char stringpool_str232[sizeof("will-change")];
    char stringpool_str233[sizeof("font-weight")];
    char stringpool_str234[sizeof("x")];
    char stringpool_str235[sizeof("-webkit-border-top-right-radius")];
    char stringpool_str236[sizeof("-webkit-font-size-delta")];
    char stringpool_str237[sizeof("scroll-snap-coordinate")];
    char stringpool_str238[sizeof("-webkit-clip-path")];
    char stringpool_str239[sizeof("rx")];
    char stringpool_str240[sizeof("lighting-color")];
    char stringpool_str241[sizeof("font-size-adjust")];
    char stringpool_str242[sizeof("align-self")];
    char stringpool_str243[sizeof("line-height")];
    char stringpool_str244[sizeof("background-attachment")];
    char stringpool_str245[sizeof("grid-template-columns")];
    char stringpool_str246[sizeof("scroll-blocks-on")];
    char stringpool_str247[sizeof("grid-auto-flow")];
    char stringpool_str248[sizeof("-webkit-logical-width")];
    char stringpool_str249[sizeof("-webkit-column-count")];
    char stringpool_str250[sizeof("-webkit-border-after-color")];
    char stringpool_str251[sizeof("y")];
    char stringpool_str252[sizeof("animation-fill-mode")];
    char stringpool_str253[sizeof("text-indent")];
    char stringpool_str254[sizeof("-webkit-border-before-color")];
    char stringpool_str255[sizeof("ry")];
    char stringpool_str256[sizeof("-webkit-column-width")];
    char stringpool_str257[sizeof("-webkit-align-self")];
    char stringpool_str258[sizeof("z-index")];
    char stringpool_str259[sizeof("scroll-behavior")];
    char stringpool_str260[sizeof("animation-timing-function")];
    char stringpool_str261[sizeof("-webkit-min-logical-width")];
    char stringpool_str262[sizeof("text-rendering")];
    char stringpool_str263[sizeof("max-zoom")];
    char stringpool_str264[sizeof("-webkit-column-rule")];
    char stringpool_str265[sizeof("-webkit-border-after-width")];
    char stringpool_str266[sizeof("flood-color")];
    char stringpool_str267[sizeof("cx")];
    char stringpool_str268[sizeof("-webkit-border-before-width")];
    char stringpool_str269[sizeof("-webkit-font-smoothing")];
    char stringpool_str270[sizeof("-webkit-box-orient")];
    char stringpool_str271[sizeof("-webkit-animation-fill-mode")];
    char stringpool_str272[sizeof("transition-timing-function")];
    char stringpool_str273[sizeof("-webkit-border-horizontal-spacing")];
    char stringpool_str274[sizeof("box-sizing")];
    char stringpool_str275[sizeof("-webkit-animation-timing-function")];
    char stringpool_str276[sizeof("font-stretch")];
    char stringpool_str277[sizeof("border-left-color")];
    char stringpool_str278[sizeof("-webkit-text-stroke")];
    char stringpool_str279[sizeof("-webkit-text-orientation")];
    char stringpool_str280[sizeof("border-bottom-left-radius")];
    char stringpool_str281[sizeof("motion-offset")];
    char stringpool_str282[sizeof("font-variant-ligatures")];
    char stringpool_str283[sizeof("cy")];
    char stringpool_str284[sizeof("-webkit-transition-timing-function")];
    char stringpool_str285[sizeof("-webkit-mask-box-image")];
    char stringpool_str286[sizeof("-webkit-box-sizing")];
    char stringpool_str287[sizeof("border-left-width")];
    char stringpool_str288[sizeof("text-decoration")];
    char stringpool_str289[sizeof("border-top-left-radius")];
    char stringpool_str290[sizeof("text-align")];
    char stringpool_str291[sizeof("-webkit-border-bottom-left-radius")];
    char stringpool_str292[sizeof("mask-type")];
    char stringpool_str293[sizeof("-webkit-text-combine")];
    char stringpool_str294[sizeof("baseline-shift")];
    char stringpool_str295[sizeof("-webkit-column-break-after")];
    char stringpool_str296[sizeof("-webkit-box-direction")];
    char stringpool_str297[sizeof("-webkit-mask-repeat-x")];
    char stringpool_str298[sizeof("-webkit-column-break-before")];
    char stringpool_str299[sizeof("mix-blend-mode")];
    char stringpool_str300[sizeof("-webkit-box-align")];
    char stringpool_str301[sizeof("-webkit-border-top-left-radius")];
    char stringpool_str302[sizeof("-webkit-box-pack")];
    char stringpool_str303[sizeof("opacity")];
    char stringpool_str304[sizeof("-webkit-mask-position-x")];
    char stringpool_str305[sizeof("-webkit-box-decoration-break")];
    char stringpool_str306[sizeof("-webkit-box-lines")];
    char stringpool_str307[sizeof("-webkit-mask-box-image-repeat")];
    char stringpool_str308[sizeof("-webkit-mask-repeat-y")];
    char stringpool_str309[sizeof("animation-delay")];
    char stringpool_str310[sizeof("fill-rule")];
    char stringpool_str311[sizeof("-webkit-opacity")];
    char stringpool_str312[sizeof("border-style")];
    char stringpool_str313[sizeof("-webkit-margin-after-collapse")];
    char stringpool_str314[sizeof("visibility")];
    char stringpool_str315[sizeof("-webkit-margin-before-collapse")];
    char stringpool_str316[sizeof("buffered-rendering")];
    char stringpool_str317[sizeof("-webkit-mask-position-y")];
    char stringpool_str318[sizeof("transition-delay")];
    char stringpool_str319[sizeof("display")];
    char stringpool_str320[sizeof("stroke-opacity")];
    char stringpool_str321[sizeof("max-width")];
    char stringpool_str322[sizeof("-webkit-logical-height")];
    char stringpool_str323[sizeof("transition-property")];
    char stringpool_str324[sizeof("vector-effect")];
    char stringpool_str325[sizeof("-webkit-animation-delay")];
    char stringpool_str326[sizeof("-webkit-highlight")];
    char stringpool_str327[sizeof("border-bottom-style")];
    char stringpool_str328[sizeof("text-decoration-line")];
    char stringpool_str329[sizeof("-epub-text-orientation")];
    char stringpool_str330[sizeof("-webkit-min-logical-height")];
    char stringpool_str331[sizeof("box-shadow")];
    char stringpool_str332[sizeof("-webkit-column-rule-color")];
    char stringpool_str333[sizeof("text-shadow")];
    char stringpool_str334[sizeof("-webkit-transition-delay")];
    char stringpool_str335[sizeof("stop-opacity")];
    char stringpool_str336[sizeof("-webkit-border-end-style")];
    char stringpool_str337[sizeof("-webkit-line-box-contain")];
    char stringpool_str338[sizeof("-webkit-transition-property")];
    char stringpool_str339[sizeof("border-top-style")];
    char stringpool_str340[sizeof("text-align-last")];
    char stringpool_str341[sizeof("column-fill")];
    char stringpool_str342[sizeof("-webkit-mask-box-image-outset")];
    char stringpool_str343[sizeof("-webkit-column-rule-width")];
    char stringpool_str344[sizeof("-webkit-box-shadow")];
    char stringpool_str345[sizeof("animation-play-state")];
    char stringpool_str346[sizeof("-webkit-text-stroke-color")];
    char stringpool_str347[sizeof("-webkit-border-start-style")];
    char stringpool_str348[sizeof("-epub-text-combine")];
    char stringpool_str349[sizeof("background-repeat-x")];
    char stringpool_str350[sizeof("color-interpolation-filters")];
    char stringpool_str351[sizeof("text-anchor")];
    char stringpool_str352[sizeof("-webkit-ruby-position")];
    char stringpool_str353[sizeof("-webkit-font-feature-settings")];
    char stringpool_str354[sizeof("-webkit-perspective-origin-x")];
    char stringpool_str355[sizeof("list-style")];
    char stringpool_str356[sizeof("-webkit-text-stroke-width")];
    char stringpool_str357[sizeof("outline-offset")];
    char stringpool_str358[sizeof("text-decoration-color")];
    char stringpool_str359[sizeof("-webkit-animation-play-state")];
    char stringpool_str360[sizeof("-webkit-mask-box-image-slice")];
    char stringpool_str361[sizeof("background-position-x")];
    char stringpool_str362[sizeof("-webkit-mask-box-image-width")];
    char stringpool_str363[sizeof("background-repeat-y")];
    char stringpool_str364[sizeof("list-style-image")];
    char stringpool_str365[sizeof("stroke-dasharray")];
    char stringpool_str366[sizeof("shape-image-threshold")];
    char stringpool_str367[sizeof("-webkit-mask-box-image-source")];
    char stringpool_str368[sizeof("-webkit-perspective-origin-y")];
    char stringpool_str369[sizeof("text-transform")];
    char stringpool_str370[sizeof("stroke-dashoffset")];
    char stringpool_str371[sizeof("-webkit-box-ordinal-group")];
    char stringpool_str372[sizeof("-webkit-text-emphasis")];
    char stringpool_str373[sizeof("text-underline-position")];
    char stringpool_str374[sizeof("background-position-y")];
    char stringpool_str375[sizeof("mask-source-type")];
    char stringpool_str376[sizeof("flex")];
    char stringpool_str377[sizeof("-webkit-shape-image-threshold")];
    char stringpool_str378[sizeof("table-layout")];
    char stringpool_str379[sizeof("flex-grow")];
    char stringpool_str380[sizeof("-webkit-transform-origin-x")];
    char stringpool_str381[sizeof("-webkit-flex")];
    char stringpool_str382[sizeof("empty-cells")];
    char stringpool_str383[sizeof("list-style-position")];
    char stringpool_str384[sizeof("outline-style")];
    char stringpool_str385[sizeof("overflow-x")];
    char stringpool_str386[sizeof("max-height")];
    char stringpool_str387[sizeof("border-right-style")];
    char stringpool_str388[sizeof("-webkit-flex-grow")];
    char stringpool_str389[sizeof("text-overflow")];
    char stringpool_str390[sizeof("flex-wrap")];
    char stringpool_str391[sizeof("flex-basis")];
    char stringpool_str392[sizeof("-webkit-transform-origin-y")];
    char stringpool_str393[sizeof("overflow-y")];
    char stringpool_str394[sizeof("font-style")];
    char stringpool_str395[sizeof("-webkit-text-emphasis-position")];
    char stringpool_str396[sizeof("-webkit-flex-wrap")];
    char stringpool_str397[sizeof("-webkit-flex-basis")];
    char stringpool_str398[sizeof("flex-direction")];
    char stringpool_str399[sizeof("-webkit-tap-highlight-color")];
    char stringpool_str400[sizeof("scroll-snap-points-x")];
    char stringpool_str401[sizeof("scroll-snap-type")];
    char stringpool_str402[sizeof("-webkit-box-reflect")];
    char stringpool_str403[sizeof("-epub-text-emphasis")];
    char stringpool_str404[sizeof("transform-style")];
    char stringpool_str405[sizeof("justify-items")];
    char stringpool_str406[sizeof("-webkit-flex-direction")];
    char stringpool_str407[sizeof("-webkit-user-modify")];
    char stringpool_str408[sizeof("-epub-text-transform")];
    char stringpool_str409[sizeof("scroll-snap-points-y")];
    char stringpool_str410[sizeof("-webkit-border-after-style")];
    char stringpool_str411[sizeof("-webkit-border-before-style")];
    char stringpool_str412[sizeof("-webkit-transform-style")];
    char stringpool_str413[sizeof("-webkit-text-emphasis-color")];
    char stringpool_str414[sizeof("-webkit-max-logical-width")];
    char stringpool_str415[sizeof("flood-opacity")];
    char stringpool_str416[sizeof("justify-content")];
    char stringpool_str417[sizeof("border-left-style")];
    char stringpool_str418[sizeof("-webkit-justify-content")];
    char stringpool_str419[sizeof("flex-shrink")];
    char stringpool_str420[sizeof("backface-visibility")];
    char stringpool_str421[sizeof("-webkit-flex-shrink")];
    char stringpool_str422[sizeof("fill-opacity")];
    char stringpool_str423[sizeof("glyph-orientation-vertical")];
    char stringpool_str424[sizeof("-webkit-backface-visibility")];
    char stringpool_str425[sizeof("-epub-text-emphasis-color")];
    char stringpool_str426[sizeof("font-family")];
    char stringpool_str427[sizeof("-webkit-text-fill-color")];
    char stringpool_str428[sizeof("-webkit-column-rule-style")];
    char stringpool_str429[sizeof("-webkit-max-logical-height")];
    char stringpool_str430[sizeof("flex-flow")];
    char stringpool_str431[sizeof("-webkit-flex-flow")];
    char stringpool_str432[sizeof("glyph-orientation-horizontal")];
    char stringpool_str433[sizeof("text-decoration-style")];
    char stringpool_str434[sizeof("-webkit-text-decorations-in-effect")];
    char stringpool_str435[sizeof("-webkit-text-security")];
    char stringpool_str436[sizeof("justify-self")];
    char stringpool_str437[sizeof("-webkit-hyphenate-character")];
    char stringpool_str438[sizeof("list-style-type")];
    char stringpool_str439[sizeof("-webkit-box-flex")];
    char stringpool_str440[sizeof("text-justify")];
    char stringpool_str441[sizeof("-webkit-text-emphasis-style")];
    char stringpool_str442[sizeof("-webkit-box-flex-group")];
    char stringpool_str443[sizeof("-epub-text-emphasis-style")];
  };
static const struct stringpool_t stringpool_contents =
  {
    "r",
    "grid",
    "order",
    "rotate",
    "border",
    "bottom",
    "motion",
    "margin",
    "marker",
    "grid-row",
    "grid-area",
    "mask",
    "animation",
    "word-break",
    "widows",
    "zoom",
    "stroke",
    "marker-end",
    "orientation",
    "marker-mid",
    "grid-row-end",
    "top",
    "writing-mode",
    "border-image",
    "-webkit-order",
    "page",
    "border-bottom",
    "size",
    "margin-bottom",
    "transition",
    "animation-name",
    "min-zoom",
    "image-rendering",
    "motion-rotation",
    "padding",
    "resize",
    "-webkit-mask",
    "-webkit-animation",
    "-webkit-border-end",
    "marker-start",
    "tab-size",
    "-webkit-margin-end",
    "word-wrap",
    "speak",
    "grid-row-start",
    "border-top",
    "-webkit-writing-mode",
    "margin-top",
    "-webkit-border-image",
    "paint-order",
    "content",
    "position",
    "-webkit-transition",
    "src",
    "-webkit-animation-name",
    "-webkit-mask-image",
    "direction",
    "padding-bottom",
    "-webkit-mask-origin",
    "-webkit-border-start",
    "-webkit-margin-start",
    "-webkit-padding-end",
    "border-image-repeat",
    "padding-top",
    "page-break-inside",
    "-webkit-mask-size",
    "pointer-events",
    "-webkit-mask-repeat",
    "animation-direction",
    "-webkit-padding-start",
    "isolation",
    "translate",
    "quotes",
    "-webkit-app-region",
    "align-items",
    "-webkit-line-break",
    "-webkit-mask-position",
    "caption-side",
    "word-spacing",
    "clear",
    "stroke-linejoin",
    "color",
    "-webkit-animation-direction",
    "-webkit-rtl-ordering",
    "border-spacing",
    "object-position",
    "border-radius",
    "dominant-baseline",
    "animation-duration",
    "grid-auto-rows",
    "grid-template",
    "stroke-miterlimit",
    "-webkit-align-items",
    "scale",
    "user-zoom",
    "all",
    "-webkit-user-drag",
    "width",
    "right",
    "align-content",
    "border-color",
    "transition-duration",
    "clip",
    "border-image-outset",
    "background",
    "-epub-word-break",
    "-webkit-appearance",
    "-webkit-border-radius",
    "cursor",
    "unicode-bidi",
    "-webkit-animation-duration",
    "color-rendering",
    "min-width",
    "unicode-range",
    "-epub-writing-mode",
    "perspective",
    "outline",
    "grid-template-rows",
    "border-width",
    "grid-template-areas",
    "border-right",
    "margin-right",
    "-webkit-mask-composite",
    "background-image",
    "-webkit-align-content",
    "border-bottom-color",
    "-webkit-transition-duration",
    "background-origin",
    "counter-reset",
    "stroke-width",
    "border-image-slice",
    "-webkit-border-end-color",
    "perspective-origin",
    "stop-color",
    "-webkit-perspective",
    "border-image-width",
    "border-top-color",
    "border-bottom-width",
    "font",
    "motion-path",
    "letter-spacing",
    "stroke-linecap",
    "orphans",
    "-webkit-background-origin",
    "border-image-source",
    "padding-right",
    "background-size",
    "animation-iteration-count",
    "alignment-baseline",
    "background-repeat",
    "-webkit-border-end-width",
    "-webkit-mask-clip",
    "-webkit-border-start-color",
    "-webkit-perspective-origin",
    "border-top-width",
    "shape-margin",
    "-webkit-locale",
    "font-kerning",
    "shape-rendering",
    "counter-increment",
    "-webkit-background-size",
    "-webkit-animation-iteration-count",
    "background-position",
    "grid-column",
    "-webkit-border-start-width",
    "transform",
    "font-variant",
    "vertical-align",
    "grid-column-end",
    "-webkit-shape-margin",
    "enable-background",
    "font-size",
    "-webkit-border-after",
    "-webkit-margin-after",
    "-webkit-border-before",
    "-epub-caption-side",
    "-webkit-margin-before",
    "-webkit-columns",
    "transform-origin",
    "-webkit-transform",
    "white-space",
    "background-blend-mode",
    "object-fit",
    "grid-column-start",
    "page-break-after",
    "left",
    "-webkit-line-clamp",
    "page-break-before",
    "float",
    "color-interpolation",
    "filter",
    "border-collapse",
    "-webkit-column-gap",
    "-webkit-padding-after",
    "-webkit-transform-origin",
    "-webkit-user-select",
    "-webkit-padding-before",
    "height",
    "border-left",
    "background-color",
    "margin-left",
    "-webkit-column-break-inside",
    "overflow",
    "-webkit-filter",
    "-webkit-column-span",
    "-webkit-margin-collapse",
    "min-height",
    "outline-color",
    "-webkit-background-composite",
    "-webkit-transform-origin-z",
    "border-right-color",
    "shape-outside",
    "-webkit-border-vertical-spacing",
    "border-bottom-right-radius",
    "background-clip",
    "-webkit-print-color-adjust",
    "clip-rule",
    "padding-left",
    "outline-width",
    "-webkit-margin-bottom-collapse",
    "border-right-width",
    "scroll-snap-destination",
    "border-top-right-radius",
    "fill",
    "clip-path",
    "-webkit-shape-outside",
    "-webkit-border-bottom-right-radius",
    "-webkit-background-clip",
    "overflow-wrap",
    "-webkit-margin-top-collapse",
    "touch-action",
    "grid-auto-columns",
    "will-change",
    "font-weight",
    "x",
    "-webkit-border-top-right-radius",
    "-webkit-font-size-delta",
    "scroll-snap-coordinate",
    "-webkit-clip-path",
    "rx",
    "lighting-color",
    "font-size-adjust",
    "align-self",
    "line-height",
    "background-attachment",
    "grid-template-columns",
    "scroll-blocks-on",
    "grid-auto-flow",
    "-webkit-logical-width",
    "-webkit-column-count",
    "-webkit-border-after-color",
    "y",
    "animation-fill-mode",
    "text-indent",
    "-webkit-border-before-color",
    "ry",
    "-webkit-column-width",
    "-webkit-align-self",
    "z-index",
    "scroll-behavior",
    "animation-timing-function",
    "-webkit-min-logical-width",
    "text-rendering",
    "max-zoom",
    "-webkit-column-rule",
    "-webkit-border-after-width",
    "flood-color",
    "cx",
    "-webkit-border-before-width",
    "-webkit-font-smoothing",
    "-webkit-box-orient",
    "-webkit-animation-fill-mode",
    "transition-timing-function",
    "-webkit-border-horizontal-spacing",
    "box-sizing",
    "-webkit-animation-timing-function",
    "font-stretch",
    "border-left-color",
    "-webkit-text-stroke",
    "-webkit-text-orientation",
    "border-bottom-left-radius",
    "motion-offset",
    "font-variant-ligatures",
    "cy",
    "-webkit-transition-timing-function",
    "-webkit-mask-box-image",
    "-webkit-box-sizing",
    "border-left-width",
    "text-decoration",
    "border-top-left-radius",
    "text-align",
    "-webkit-border-bottom-left-radius",
    "mask-type",
    "-webkit-text-combine",
    "baseline-shift",
    "-webkit-column-break-after",
    "-webkit-box-direction",
    "-webkit-mask-repeat-x",
    "-webkit-column-break-before",
    "mix-blend-mode",
    "-webkit-box-align",
    "-webkit-border-top-left-radius",
    "-webkit-box-pack",
    "opacity",
    "-webkit-mask-position-x",
    "-webkit-box-decoration-break",
    "-webkit-box-lines",
    "-webkit-mask-box-image-repeat",
    "-webkit-mask-repeat-y",
    "animation-delay",
    "fill-rule",
    "-webkit-opacity",
    "border-style",
    "-webkit-margin-after-collapse",
    "visibility",
    "-webkit-margin-before-collapse",
    "buffered-rendering",
    "-webkit-mask-position-y",
    "transition-delay",
    "display",
    "stroke-opacity",
    "max-width",
    "-webkit-logical-height",
    "transition-property",
    "vector-effect",
    "-webkit-animation-delay",
    "-webkit-highlight",
    "border-bottom-style",
    "text-decoration-line",
    "-epub-text-orientation",
    "-webkit-min-logical-height",
    "box-shadow",
    "-webkit-column-rule-color",
    "text-shadow",
    "-webkit-transition-delay",
    "stop-opacity",
    "-webkit-border-end-style",
    "-webkit-line-box-contain",
    "-webkit-transition-property",
    "border-top-style",
    "text-align-last",
    "column-fill",
    "-webkit-mask-box-image-outset",
    "-webkit-column-rule-width",
    "-webkit-box-shadow",
    "animation-play-state",
    "-webkit-text-stroke-color",
    "-webkit-border-start-style",
    "-epub-text-combine",
    "background-repeat-x",
    "color-interpolation-filters",
    "text-anchor",
    "-webkit-ruby-position",
    "-webkit-font-feature-settings",
    "-webkit-perspective-origin-x",
    "list-style",
    "-webkit-text-stroke-width",
    "outline-offset",
    "text-decoration-color",
    "-webkit-animation-play-state",
    "-webkit-mask-box-image-slice",
    "background-position-x",
    "-webkit-mask-box-image-width",
    "background-repeat-y",
    "list-style-image",
    "stroke-dasharray",
    "shape-image-threshold",
    "-webkit-mask-box-image-source",
    "-webkit-perspective-origin-y",
    "text-transform",
    "stroke-dashoffset",
    "-webkit-box-ordinal-group",
    "-webkit-text-emphasis",
    "text-underline-position",
    "background-position-y",
    "mask-source-type",
    "flex",
    "-webkit-shape-image-threshold",
    "table-layout",
    "flex-grow",
    "-webkit-transform-origin-x",
    "-webkit-flex",
    "empty-cells",
    "list-style-position",
    "outline-style",
    "overflow-x",
    "max-height",
    "border-right-style",
    "-webkit-flex-grow",
    "text-overflow",
    "flex-wrap",
    "flex-basis",
    "-webkit-transform-origin-y",
    "overflow-y",
    "font-style",
    "-webkit-text-emphasis-position",
    "-webkit-flex-wrap",
    "-webkit-flex-basis",
    "flex-direction",
    "-webkit-tap-highlight-color",
    "scroll-snap-points-x",
    "scroll-snap-type",
    "-webkit-box-reflect",
    "-epub-text-emphasis",
    "transform-style",
    "justify-items",
    "-webkit-flex-direction",
    "-webkit-user-modify",
    "-epub-text-transform",
    "scroll-snap-points-y",
    "-webkit-border-after-style",
    "-webkit-border-before-style",
    "-webkit-transform-style",
    "-webkit-text-emphasis-color",
    "-webkit-max-logical-width",
    "flood-opacity",
    "justify-content",
    "border-left-style",
    "-webkit-justify-content",
    "flex-shrink",
    "backface-visibility",
    "-webkit-flex-shrink",
    "fill-opacity",
    "glyph-orientation-vertical",
    "-webkit-backface-visibility",
    "-epub-text-emphasis-color",
    "font-family",
    "-webkit-text-fill-color",
    "-webkit-column-rule-style",
    "-webkit-max-logical-height",
    "flex-flow",
    "-webkit-flex-flow",
    "glyph-orientation-horizontal",
    "text-decoration-style",
    "-webkit-text-decorations-in-effect",
    "-webkit-text-security",
    "justify-self",
    "-webkit-hyphenate-character",
    "list-style-type",
    "-webkit-box-flex",
    "text-justify",
    "-webkit-text-emphasis-style",
    "-webkit-box-flex-group",
    "-epub-text-emphasis-style"
  };
#define stringpool ((const char *) &stringpool_contents)

static const struct Property property_word_list[] =
  {
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str0, CSSPropertyR},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str1, CSSPropertyGrid},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str2, CSSPropertyOrder},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str3, CSSPropertyRotate},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str4, CSSPropertyBorder},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str5, CSSPropertyBottom},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str6, CSSPropertyMotion},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str7, CSSPropertyMargin},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str8, CSSPropertyMarker},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str9, CSSPropertyGridRow},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str10, CSSPropertyGridArea},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str11, CSSPropertyMask},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str12, CSSPropertyAnimation},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str13, CSSPropertyWordBreak},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str14, CSSPropertyWidows},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str15, CSSPropertyZoom},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str16, CSSPropertyStroke},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str17, CSSPropertyMarkerEnd},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str18, CSSPropertyOrientation},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str19, CSSPropertyMarkerMid},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str20, CSSPropertyGridRowEnd},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str21, CSSPropertyTop},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str22, CSSPropertyWritingMode},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str23, CSSPropertyBorderImage},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str24, CSSPropertyAliasWebkitOrder},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str25, CSSPropertyPage},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str26, CSSPropertyBorderBottom},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str27, CSSPropertySize},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str28, CSSPropertyMarginBottom},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str29, CSSPropertyTransition},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str30, CSSPropertyAnimationName},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str31, CSSPropertyMinZoom},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str32, CSSPropertyImageRendering},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str33, CSSPropertyMotionRotation},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str34, CSSPropertyPadding},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str35, CSSPropertyResize},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str36, CSSPropertyWebkitMask},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str37, CSSPropertyAliasWebkitAnimation},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str38, CSSPropertyWebkitBorderEnd},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str39, CSSPropertyMarkerStart},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str40, CSSPropertyTabSize},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str41, CSSPropertyWebkitMarginEnd},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str42, CSSPropertyWordWrap},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str43, CSSPropertySpeak},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str44, CSSPropertyGridRowStart},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str45, CSSPropertyBorderTop},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str46, CSSPropertyWebkitWritingMode},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str47, CSSPropertyMarginTop},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str48, CSSPropertyWebkitBorderImage},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str49, CSSPropertyPaintOrder},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str50, CSSPropertyContent},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str51, CSSPropertyPosition},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str52, CSSPropertyAliasWebkitTransition},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str53, CSSPropertySrc},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str54, CSSPropertyAliasWebkitAnimationName},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str55, CSSPropertyWebkitMaskImage},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str56, CSSPropertyDirection},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str57, CSSPropertyPaddingBottom},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str58, CSSPropertyWebkitMaskOrigin},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str59, CSSPropertyWebkitBorderStart},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str60, CSSPropertyWebkitMarginStart},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str61, CSSPropertyWebkitPaddingEnd},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str62, CSSPropertyBorderImageRepeat},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str63, CSSPropertyPaddingTop},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str64, CSSPropertyPageBreakInside},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str65, CSSPropertyWebkitMaskSize},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str66, CSSPropertyPointerEvents},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str67, CSSPropertyWebkitMaskRepeat},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str68, CSSPropertyAnimationDirection},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str69, CSSPropertyWebkitPaddingStart},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str70, CSSPropertyIsolation},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str71, CSSPropertyTranslate},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str72, CSSPropertyQuotes},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str73, CSSPropertyWebkitAppRegion},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str74, CSSPropertyAlignItems},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str75, CSSPropertyWebkitLineBreak},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str76, CSSPropertyWebkitMaskPosition},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str77, CSSPropertyCaptionSide},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str78, CSSPropertyWordSpacing},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str79, CSSPropertyClear},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str80, CSSPropertyStrokeLinejoin},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str81, CSSPropertyColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str82, CSSPropertyAliasWebkitAnimationDirection},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str83, CSSPropertyWebkitRtlOrdering},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str84, CSSPropertyBorderSpacing},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str85, CSSPropertyObjectPosition},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str86, CSSPropertyBorderRadius},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str87, CSSPropertyDominantBaseline},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str88, CSSPropertyAnimationDuration},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str89, CSSPropertyGridAutoRows},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str90, CSSPropertyGridTemplate},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str91, CSSPropertyStrokeMiterlimit},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str92, CSSPropertyAliasWebkitAlignItems},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str93, CSSPropertyScale},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str94, CSSPropertyUserZoom},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str95, CSSPropertyAll},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str96, CSSPropertyWebkitUserDrag},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str97, CSSPropertyWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str98, CSSPropertyRight},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str99, CSSPropertyAlignContent},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str100, CSSPropertyBorderColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str101, CSSPropertyTransitionDuration},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str102, CSSPropertyClip},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str103, CSSPropertyBorderImageOutset},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str104, CSSPropertyBackground},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str105, CSSPropertyAliasEpubWordBreak},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str106, CSSPropertyWebkitAppearance},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str107, CSSPropertyAliasWebkitBorderRadius},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str108, CSSPropertyCursor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str109, CSSPropertyUnicodeBidi},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str110, CSSPropertyAliasWebkitAnimationDuration},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str111, CSSPropertyColorRendering},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str112, CSSPropertyMinWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str113, CSSPropertyUnicodeRange},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str114, CSSPropertyAliasEpubWritingMode},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str115, CSSPropertyPerspective},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str116, CSSPropertyOutline},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str117, CSSPropertyGridTemplateRows},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str118, CSSPropertyBorderWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str119, CSSPropertyGridTemplateAreas},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str120, CSSPropertyBorderRight},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str121, CSSPropertyMarginRight},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str122, CSSPropertyWebkitMaskComposite},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str123, CSSPropertyBackgroundImage},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str124, CSSPropertyAliasWebkitAlignContent},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str125, CSSPropertyBorderBottomColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str126, CSSPropertyAliasWebkitTransitionDuration},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str127, CSSPropertyBackgroundOrigin},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str128, CSSPropertyCounterReset},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str129, CSSPropertyStrokeWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str130, CSSPropertyBorderImageSlice},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str131, CSSPropertyWebkitBorderEndColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str132, CSSPropertyPerspectiveOrigin},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str133, CSSPropertyStopColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str134, CSSPropertyAliasWebkitPerspective},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str135, CSSPropertyBorderImageWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str136, CSSPropertyBorderTopColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str137, CSSPropertyBorderBottomWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str138, CSSPropertyFont},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str139, CSSPropertyMotionPath},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str140, CSSPropertyLetterSpacing},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str141, CSSPropertyStrokeLinecap},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str142, CSSPropertyOrphans},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str143, CSSPropertyWebkitBackgroundOrigin},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str144, CSSPropertyBorderImageSource},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str145, CSSPropertyPaddingRight},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str146, CSSPropertyBackgroundSize},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str147, CSSPropertyAnimationIterationCount},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str148, CSSPropertyAlignmentBaseline},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str149, CSSPropertyBackgroundRepeat},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str150, CSSPropertyWebkitBorderEndWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str151, CSSPropertyWebkitMaskClip},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str152, CSSPropertyWebkitBorderStartColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str153, CSSPropertyAliasWebkitPerspectiveOrigin},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str154, CSSPropertyBorderTopWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str155, CSSPropertyShapeMargin},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str156, CSSPropertyWebkitLocale},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str157, CSSPropertyFontKerning},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str158, CSSPropertyShapeRendering},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str159, CSSPropertyCounterIncrement},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str160, CSSPropertyAliasWebkitBackgroundSize},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str161, CSSPropertyAliasWebkitAnimationIterationCount},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str162, CSSPropertyBackgroundPosition},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str163, CSSPropertyGridColumn},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str164, CSSPropertyWebkitBorderStartWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str165, CSSPropertyTransform},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str166, CSSPropertyFontVariant},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str167, CSSPropertyVerticalAlign},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str168, CSSPropertyGridColumnEnd},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str169, CSSPropertyAliasWebkitShapeMargin},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str170, CSSPropertyEnableBackground},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str171, CSSPropertyFontSize},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str172, CSSPropertyWebkitBorderAfter},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str173, CSSPropertyWebkitMarginAfter},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str174, CSSPropertyWebkitBorderBefore},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str175, CSSPropertyAliasEpubCaptionSide},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str176, CSSPropertyWebkitMarginBefore},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str177, CSSPropertyWebkitColumns},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str178, CSSPropertyTransformOrigin},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str179, CSSPropertyAliasWebkitTransform},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str180, CSSPropertyWhiteSpace},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str181, CSSPropertyBackgroundBlendMode},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str182, CSSPropertyObjectFit},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str183, CSSPropertyGridColumnStart},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str184, CSSPropertyPageBreakAfter},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str185, CSSPropertyLeft},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str186, CSSPropertyWebkitLineClamp},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str187, CSSPropertyPageBreakBefore},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str188, CSSPropertyFloat},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str189, CSSPropertyColorInterpolation},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str190, CSSPropertyFilter},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str191, CSSPropertyBorderCollapse},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str192, CSSPropertyWebkitColumnGap},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str193, CSSPropertyWebkitPaddingAfter},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str194, CSSPropertyAliasWebkitTransformOrigin},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str195, CSSPropertyWebkitUserSelect},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str196, CSSPropertyWebkitPaddingBefore},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str197, CSSPropertyHeight},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str198, CSSPropertyBorderLeft},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str199, CSSPropertyBackgroundColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str200, CSSPropertyMarginLeft},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str201, CSSPropertyWebkitColumnBreakInside},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str202, CSSPropertyOverflow},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str203, CSSPropertyWebkitFilter},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str204, CSSPropertyWebkitColumnSpan},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str205, CSSPropertyWebkitMarginCollapse},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str206, CSSPropertyMinHeight},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str207, CSSPropertyOutlineColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str208, CSSPropertyWebkitBackgroundComposite},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str209, CSSPropertyWebkitTransformOriginZ},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str210, CSSPropertyBorderRightColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str211, CSSPropertyShapeOutside},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str212, CSSPropertyWebkitBorderVerticalSpacing},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str213, CSSPropertyBorderBottomRightRadius},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str214, CSSPropertyBackgroundClip},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str215, CSSPropertyWebkitPrintColorAdjust},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str216, CSSPropertyClipRule},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str217, CSSPropertyPaddingLeft},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str218, CSSPropertyOutlineWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str219, CSSPropertyWebkitMarginBottomCollapse},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str220, CSSPropertyBorderRightWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str221, CSSPropertyScrollSnapDestination},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str222, CSSPropertyBorderTopRightRadius},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str223, CSSPropertyFill},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str224, CSSPropertyClipPath},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str225, CSSPropertyAliasWebkitShapeOutside},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str226, CSSPropertyAliasWebkitBorderBottomRightRadius},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str227, CSSPropertyWebkitBackgroundClip},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str228, CSSPropertyOverflowWrap},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str229, CSSPropertyWebkitMarginTopCollapse},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str230, CSSPropertyTouchAction},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str231, CSSPropertyGridAutoColumns},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str232, CSSPropertyWillChange},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str233, CSSPropertyFontWeight},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str234, CSSPropertyX},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str235, CSSPropertyAliasWebkitBorderTopRightRadius},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str236, CSSPropertyWebkitFontSizeDelta},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str237, CSSPropertyScrollSnapCoordinate},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str238, CSSPropertyWebkitClipPath},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str239, CSSPropertyRx},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str240, CSSPropertyLightingColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str241, CSSPropertyFontSizeAdjust},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str242, CSSPropertyAlignSelf},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str243, CSSPropertyLineHeight},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str244, CSSPropertyBackgroundAttachment},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str245, CSSPropertyGridTemplateColumns},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str246, CSSPropertyScrollBlocksOn},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str247, CSSPropertyGridAutoFlow},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str248, CSSPropertyWebkitLogicalWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str249, CSSPropertyWebkitColumnCount},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str250, CSSPropertyWebkitBorderAfterColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str251, CSSPropertyY},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str252, CSSPropertyAnimationFillMode},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str253, CSSPropertyTextIndent},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str254, CSSPropertyWebkitBorderBeforeColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str255, CSSPropertyRy},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str256, CSSPropertyWebkitColumnWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str257, CSSPropertyAliasWebkitAlignSelf},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str258, CSSPropertyZIndex},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str259, CSSPropertyScrollBehavior},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str260, CSSPropertyAnimationTimingFunction},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str261, CSSPropertyWebkitMinLogicalWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str262, CSSPropertyTextRendering},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str263, CSSPropertyMaxZoom},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str264, CSSPropertyWebkitColumnRule},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str265, CSSPropertyWebkitBorderAfterWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str266, CSSPropertyFloodColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str267, CSSPropertyCx},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str268, CSSPropertyWebkitBorderBeforeWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str269, CSSPropertyWebkitFontSmoothing},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str270, CSSPropertyWebkitBoxOrient},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str271, CSSPropertyAliasWebkitAnimationFillMode},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str272, CSSPropertyTransitionTimingFunction},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str273, CSSPropertyWebkitBorderHorizontalSpacing},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str274, CSSPropertyBoxSizing},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str275, CSSPropertyAliasWebkitAnimationTimingFunction},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str276, CSSPropertyFontStretch},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str277, CSSPropertyBorderLeftColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str278, CSSPropertyWebkitTextStroke},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str279, CSSPropertyWebkitTextOrientation},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str280, CSSPropertyBorderBottomLeftRadius},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str281, CSSPropertyMotionOffset},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str282, CSSPropertyFontVariantLigatures},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str283, CSSPropertyCy},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str284, CSSPropertyAliasWebkitTransitionTimingFunction},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str285, CSSPropertyWebkitMaskBoxImage},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str286, CSSPropertyAliasWebkitBoxSizing},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str287, CSSPropertyBorderLeftWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str288, CSSPropertyTextDecoration},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str289, CSSPropertyBorderTopLeftRadius},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str290, CSSPropertyTextAlign},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str291, CSSPropertyAliasWebkitBorderBottomLeftRadius},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str292, CSSPropertyMaskType},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str293, CSSPropertyWebkitTextCombine},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str294, CSSPropertyBaselineShift},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str295, CSSPropertyWebkitColumnBreakAfter},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str296, CSSPropertyWebkitBoxDirection},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str297, CSSPropertyWebkitMaskRepeatX},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str298, CSSPropertyWebkitColumnBreakBefore},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str299, CSSPropertyMixBlendMode},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str300, CSSPropertyWebkitBoxAlign},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str301, CSSPropertyAliasWebkitBorderTopLeftRadius},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str302, CSSPropertyWebkitBoxPack},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str303, CSSPropertyOpacity},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str304, CSSPropertyWebkitMaskPositionX},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str305, CSSPropertyWebkitBoxDecorationBreak},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str306, CSSPropertyWebkitBoxLines},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str307, CSSPropertyWebkitMaskBoxImageRepeat},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str308, CSSPropertyWebkitMaskRepeatY},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str309, CSSPropertyAnimationDelay},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str310, CSSPropertyFillRule},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str311, CSSPropertyAliasWebkitOpacity},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str312, CSSPropertyBorderStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str313, CSSPropertyWebkitMarginAfterCollapse},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str314, CSSPropertyVisibility},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str315, CSSPropertyWebkitMarginBeforeCollapse},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str316, CSSPropertyBufferedRendering},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str317, CSSPropertyWebkitMaskPositionY},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str318, CSSPropertyTransitionDelay},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str319, CSSPropertyDisplay},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str320, CSSPropertyStrokeOpacity},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str321, CSSPropertyMaxWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str322, CSSPropertyWebkitLogicalHeight},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str323, CSSPropertyTransitionProperty},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str324, CSSPropertyVectorEffect},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str325, CSSPropertyAliasWebkitAnimationDelay},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str326, CSSPropertyWebkitHighlight},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str327, CSSPropertyBorderBottomStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str328, CSSPropertyTextDecorationLine},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str329, CSSPropertyAliasEpubTextOrientation},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str330, CSSPropertyWebkitMinLogicalHeight},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str331, CSSPropertyBoxShadow},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str332, CSSPropertyWebkitColumnRuleColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str333, CSSPropertyTextShadow},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str334, CSSPropertyAliasWebkitTransitionDelay},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str335, CSSPropertyStopOpacity},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str336, CSSPropertyWebkitBorderEndStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str337, CSSPropertyWebkitLineBoxContain},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str338, CSSPropertyAliasWebkitTransitionProperty},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str339, CSSPropertyBorderTopStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str340, CSSPropertyTextAlignLast},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str341, CSSPropertyColumnFill},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str342, CSSPropertyWebkitMaskBoxImageOutset},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str343, CSSPropertyWebkitColumnRuleWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str344, CSSPropertyAliasWebkitBoxShadow},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str345, CSSPropertyAnimationPlayState},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str346, CSSPropertyWebkitTextStrokeColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str347, CSSPropertyWebkitBorderStartStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str348, CSSPropertyAliasEpubTextCombine},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str349, CSSPropertyBackgroundRepeatX},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str350, CSSPropertyColorInterpolationFilters},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str351, CSSPropertyTextAnchor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str352, CSSPropertyWebkitRubyPosition},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str353, CSSPropertyWebkitFontFeatureSettings},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str354, CSSPropertyWebkitPerspectiveOriginX},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str355, CSSPropertyListStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str356, CSSPropertyWebkitTextStrokeWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str357, CSSPropertyOutlineOffset},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str358, CSSPropertyTextDecorationColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str359, CSSPropertyAliasWebkitAnimationPlayState},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str360, CSSPropertyWebkitMaskBoxImageSlice},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str361, CSSPropertyBackgroundPositionX},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str362, CSSPropertyWebkitMaskBoxImageWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str363, CSSPropertyBackgroundRepeatY},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str364, CSSPropertyListStyleImage},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str365, CSSPropertyStrokeDasharray},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str366, CSSPropertyShapeImageThreshold},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str367, CSSPropertyWebkitMaskBoxImageSource},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str368, CSSPropertyWebkitPerspectiveOriginY},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str369, CSSPropertyTextTransform},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str370, CSSPropertyStrokeDashoffset},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str371, CSSPropertyWebkitBoxOrdinalGroup},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str372, CSSPropertyWebkitTextEmphasis},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str373, CSSPropertyTextUnderlinePosition},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str374, CSSPropertyBackgroundPositionY},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str375, CSSPropertyMaskSourceType},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str376, CSSPropertyFlex},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str377, CSSPropertyAliasWebkitShapeImageThreshold},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str378, CSSPropertyTableLayout},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str379, CSSPropertyFlexGrow},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str380, CSSPropertyWebkitTransformOriginX},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str381, CSSPropertyAliasWebkitFlex},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str382, CSSPropertyEmptyCells},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str383, CSSPropertyListStylePosition},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str384, CSSPropertyOutlineStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str385, CSSPropertyOverflowX},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str386, CSSPropertyMaxHeight},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str387, CSSPropertyBorderRightStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str388, CSSPropertyAliasWebkitFlexGrow},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str389, CSSPropertyTextOverflow},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str390, CSSPropertyFlexWrap},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str391, CSSPropertyFlexBasis},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str392, CSSPropertyWebkitTransformOriginY},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str393, CSSPropertyOverflowY},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str394, CSSPropertyFontStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str395, CSSPropertyWebkitTextEmphasisPosition},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str396, CSSPropertyAliasWebkitFlexWrap},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str397, CSSPropertyAliasWebkitFlexBasis},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str398, CSSPropertyFlexDirection},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str399, CSSPropertyWebkitTapHighlightColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str400, CSSPropertyScrollSnapPointsX},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str401, CSSPropertyScrollSnapType},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str402, CSSPropertyWebkitBoxReflect},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str403, CSSPropertyAliasEpubTextEmphasis},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str404, CSSPropertyTransformStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str405, CSSPropertyJustifyItems},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str406, CSSPropertyAliasWebkitFlexDirection},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str407, CSSPropertyWebkitUserModify},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str408, CSSPropertyAliasEpubTextTransform},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str409, CSSPropertyScrollSnapPointsY},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str410, CSSPropertyWebkitBorderAfterStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str411, CSSPropertyWebkitBorderBeforeStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str412, CSSPropertyAliasWebkitTransformStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str413, CSSPropertyWebkitTextEmphasisColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str414, CSSPropertyWebkitMaxLogicalWidth},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str415, CSSPropertyFloodOpacity},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str416, CSSPropertyJustifyContent},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str417, CSSPropertyBorderLeftStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str418, CSSPropertyAliasWebkitJustifyContent},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str419, CSSPropertyFlexShrink},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str420, CSSPropertyBackfaceVisibility},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str421, CSSPropertyAliasWebkitFlexShrink},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str422, CSSPropertyFillOpacity},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str423, CSSPropertyGlyphOrientationVertical},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str424, CSSPropertyAliasWebkitBackfaceVisibility},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str425, CSSPropertyAliasEpubTextEmphasisColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str426, CSSPropertyFontFamily},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str427, CSSPropertyWebkitTextFillColor},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str428, CSSPropertyWebkitColumnRuleStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str429, CSSPropertyWebkitMaxLogicalHeight},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str430, CSSPropertyFlexFlow},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str431, CSSPropertyAliasWebkitFlexFlow},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str432, CSSPropertyGlyphOrientationHorizontal},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str433, CSSPropertyTextDecorationStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str434, CSSPropertyWebkitTextDecorationsInEffect},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str435, CSSPropertyWebkitTextSecurity},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str436, CSSPropertyJustifySelf},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str437, CSSPropertyWebkitHyphenateCharacter},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str438, CSSPropertyListStyleType},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str439, CSSPropertyWebkitBoxFlex},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str440, CSSPropertyTextJustify},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str441, CSSPropertyWebkitTextEmphasisStyle},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str442, CSSPropertyWebkitBoxFlexGroup},
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str443, CSSPropertyAliasEpubTextEmphasisStyle}
  };

static const short lookup[] =
  {
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      0,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,   1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,   2,  -1,  -1,  -1,   3,  -1,  -1,   4,
      5,   6,   7,  -1,   8,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,   9,  -1,  -1,  -1,  -1,  -1,  10,
     11,  -1,  12,  -1,  -1,  -1,  -1,  13,  -1,  14,
     15,  -1,  16,  17,  18,  -1,  -1,  -1,  19,  -1,
     -1,  -1,  20,  21,  -1,  22,  -1,  23,  24,  25,
     -1,  -1,  -1,  -1,  -1,  -1,  26,  -1,  27,  28,
     -1,  -1,  29,  -1,  30,  31,  -1,  -1,  -1,  32,
     33,  34,  -1,  -1,  -1,  35,  36,  -1,  37,  -1,
     -1,  -1,  -1,  -1,  38,  39,  40,  41,  -1,  -1,
     -1,  42,  43,  -1,  44,  -1,  -1,  -1,  -1,  45,
     -1,  46,  47,  48,  -1,  -1,  49,  -1,  -1,  -1,
     -1,  50,  -1,  -1,  -1,  -1,  51,  -1,  52,  53,
     54,  -1,  -1,  -1,  55,  -1,  -1,  56,  57,  -1,
     58,  -1,  -1,  -1,  -1,  -1,  59,  -1,  -1,  60,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  61,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     62,  63,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     64,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  65,  -1,  66,  -1,  -1,  -1,  -1,  -1,  67,
     -1,  -1,  -1,  -1,  -1,  -1,  68,  -1,  69,  -1,
     -1,  -1,  -1,  -1,  70,  71,  -1,  -1,  72,  -1,
     -1,  -1,  -1,  73,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  74,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  75,  -1,  -1,  -1,  -1,  -1,  -1,  76,
     -1,  -1,  77,  -1,  -1,  78,  -1,  -1,  -1,  79,
     80,  81,  82,  83,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  84,  -1,  -1,  85,  86,  87,  -1,  88,
     -1,  89,  -1,  90,  -1,  -1,  91,  -1,  92,  -1,
     -1,  -1,  93,  94,  -1,  95,  -1,  -1,  -1,  -1,
     96,  -1,  -1,  -1,  -1,  -1,  97,  -1,  -1,  98,
     -1,  -1,  -1,  -1,  -1,  -1,  99, 100,  -1, 101,
    102,  -1, 103, 104,  -1,  -1,  -1,  -1,  -1, 105,
     -1, 106, 107, 108, 109, 110,  -1,  -1,  -1, 111,
     -1, 112,  -1,  -1,  -1, 113,  -1, 114,  -1, 115,
     -1,  -1, 116,  -1,  -1,  -1,  -1,  -1, 117,  -1,
     -1,  -1, 118,  -1, 119, 120,  -1,  -1, 121,  -1,
    122, 123, 124,  -1, 125, 126,  -1, 127,  -1,  -1,
     -1,  -1,  -1, 128,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1, 129, 130,  -1,  -1,  -1,
     -1,  -1, 131, 132, 133, 134,  -1,  -1,  -1,  -1,
    135,  -1,  -1,  -1,  -1,  -1,  -1, 136,  -1, 137,
     -1,  -1,  -1,  -1, 138,  -1,  -1,  -1, 139,  -1,
    140, 141, 142, 143, 144,  -1,  -1, 145, 146, 147,
     -1,  -1,  -1,  -1,  -1, 148, 149, 150,  -1,  -1,
     -1,  -1,  -1, 151, 152,  -1,  -1,  -1,  -1, 153,
     -1,  -1, 154,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1, 155,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1, 156,  -1,  -1, 157,  -1,  -1,  -1,  -1,
     -1, 158, 159,  -1, 160, 161, 162,  -1, 163, 164,
     -1,  -1,  -1,  -1,  -1,  -1,  -1, 165,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1, 166, 167,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1, 168, 169,  -1,
     -1,  -1,  -1,  -1,  -1, 170,  -1,  -1,  -1, 171,
     -1,  -1,  -1,  -1, 172,  -1,  -1, 173,  -1,  -1,
     -1,  -1, 174,  -1, 175, 176,  -1,  -1,  -1, 177,
     -1, 178,  -1, 179, 180,  -1,  -1,  -1,  -1,  -1,
    181,  -1,  -1,  -1, 182,  -1,  -1,  -1,  -1, 183,
     -1,  -1,  -1, 184, 185,  -1,  -1, 186,  -1,  -1,
     -1, 187, 188,  -1,  -1,  -1,  -1,  -1,  -1, 189,
     -1, 190,  -1, 191,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1, 192, 193, 194, 195,  -1,
     -1,  -1,  -1,  -1, 196,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1, 197,  -1,  -1,  -1,  -1,  -1,
    198, 199,  -1, 200,  -1,  -1,  -1, 201,  -1, 202,
     -1,  -1,  -1,  -1,  -1,  -1,  -1, 203,  -1, 204,
     -1,  -1, 205,  -1,  -1,  -1,  -1,  -1,  -1, 206,
    207,  -1,  -1, 208,  -1, 209,  -1,  -1,  -1,  -1,
     -1,  -1,  -1, 210,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1, 211,  -1, 212,  -1, 213,
    214,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 215, 216,
     -1,  -1, 217,  -1,  -1, 218,  -1,  -1,  -1, 219,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 220,  -1,
     -1, 221,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1, 222,  -1,  -1,  -1, 223, 224,  -1,  -1,
     -1, 225,  -1,  -1,  -1, 226, 227,  -1, 228,  -1,
     -1,  -1, 229,  -1,  -1, 230, 231,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1, 232,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1, 233,  -1, 234,  -1, 235, 236,
    237,  -1,  -1, 238,  -1,  -1, 239,  -1, 240,  -1,
     -1,  -1,  -1, 241,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1, 242,  -1,  -1,  -1,  -1,
     -1,  -1, 243,  -1,  -1,  -1,  -1, 244,  -1,  -1,
     -1,  -1,  -1, 245,  -1, 246,  -1,  -1, 247,  -1,
     -1,  -1, 248,  -1,  -1,  -1,  -1,  -1, 249,  -1,
     -1,  -1, 250,  -1, 251,  -1,  -1, 252, 253,  -1,
    254,  -1,  -1,  -1, 255,  -1,  -1,  -1,  -1, 256,
     -1, 257,  -1, 258,  -1, 259, 260, 261,  -1,  -1,
     -1,  -1,  -1,  -1,  -1, 262,  -1,  -1,  -1,  -1,
     -1,  -1,  -1, 263,  -1, 264,  -1, 265,  -1,  -1,
     -1, 266, 267,  -1,  -1, 268, 269,  -1,  -1,  -1,
     -1, 270,  -1, 271,  -1,  -1, 272,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 273,  -1,
     -1, 274, 275,  -1,  -1,  -1,  -1,  -1,  -1, 276,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 277,  -1,
     -1,  -1, 278,  -1, 279,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1, 280, 281, 282,  -1,  -1,  -1,
    283,  -1, 284,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1, 285,  -1,  -1,  -1,  -1, 286,  -1,  -1,
     -1,  -1,  -1, 287,  -1,  -1,  -1,  -1,  -1, 288,
     -1,  -1,  -1,  -1,  -1,  -1,  -1, 289,  -1,  -1,
     -1,  -1, 290,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    291,  -1,  -1, 292,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1, 293,  -1, 294,  -1,
    295, 296, 297,  -1,  -1,  -1,  -1,  -1, 298,  -1,
     -1,  -1,  -1, 299,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1, 300, 301,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1, 302,  -1,  -1,  -1,  -1, 303,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1, 304,  -1, 305,  -1,  -1,  -1, 306,  -1,
     -1,  -1,  -1,  -1,  -1, 307,  -1,  -1,  -1,  -1,
    308,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1, 309,  -1, 310,  -1,  -1,  -1,  -1,
     -1,  -1,  -1, 311,  -1,  -1, 312,  -1,  -1,  -1,
     -1, 313,  -1,  -1,  -1,  -1,  -1, 314,  -1, 315,
     -1,  -1,  -1,  -1, 316,  -1,  -1,  -1,  -1,  -1,
    317,  -1,  -1, 318,  -1, 319, 320,  -1,  -1, 321,
    322, 323,  -1, 324,  -1,  -1,  -1,  -1,  -1, 325,
     -1,  -1,  -1,  -1, 326,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1, 327,  -1,  -1,  -1, 328,  -1,  -1,
    329,  -1,  -1,  -1,  -1, 330,  -1,  -1, 331,  -1,
     -1,  -1,  -1, 332, 333,  -1,  -1,  -1,  -1, 334,
    335, 336,  -1, 337,  -1,  -1,  -1, 338,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1, 339,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1, 340,  -1,  -1,  -1,  -1,
     -1,  -1,  -1, 341,  -1,  -1,  -1, 342, 343,  -1,
     -1,  -1,  -1,  -1, 344, 345,  -1,  -1,  -1,  -1,
    346,  -1,  -1, 347,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1, 348,  -1,  -1,  -1,  -1,  -1,  -1, 349,
    350, 351,  -1,  -1,  -1,  -1, 352,  -1, 353,  -1,
     -1,  -1, 354,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1, 355,  -1, 356, 357, 358,  -1,  -1,
     -1, 359,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1, 360,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 361,
     -1,  -1,  -1,  -1,  -1, 362,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1, 363,  -1,  -1,
     -1, 364, 365,  -1,  -1, 366,  -1,  -1,  -1, 367,
    368, 369,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1, 370,  -1,  -1,  -1,  -1, 371,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1, 372,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1, 373, 374,  -1,  -1,
    375,  -1,  -1, 376,  -1,  -1,  -1,  -1,  -1,  -1,
     -1, 377,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1, 378,  -1,  -1,  -1,  -1,
     -1,  -1, 379,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    380,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 381,
    382,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1, 383,  -1,  -1, 384,
     -1,  -1, 385,  -1,  -1,  -1,  -1, 386,  -1,  -1,
     -1,  -1, 387,  -1,  -1,  -1,  -1,  -1, 388,  -1,
     -1,  -1,  -1, 389,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1, 390,  -1,  -1,  -1,  -1, 391, 392,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    393, 394,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 395,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 396,  -1,
     -1,  -1,  -1, 397,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1, 398,  -1,  -1,
     -1, 399,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 400,
     -1, 401,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1, 402,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1, 403,  -1, 404,  -1,  -1,  -1,  -1, 405,
     -1,  -1,  -1, 406,  -1,  -1,  -1,  -1,  -1, 407,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1, 408,  -1,  -1,  -1, 409,  -1,  -1,
     -1, 410,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 411,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    412,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1, 413, 414,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1, 415,  -1,  -1,
     -1,  -1,  -1, 416,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1, 417,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 418,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1, 419,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1, 420,  -1,  -1,  -1,  -1,  -1,  -1, 421,
    422,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1, 423,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 424,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    425,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1, 426,  -1, 427,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1, 428, 429,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1, 430,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1, 431,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1, 432,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1, 433,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1, 434,  -1,  -1,  -1,
    435,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1, 436,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1, 437,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1, 438,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1, 439,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 440,
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
     -1,  -1,  -1, 441,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1, 442,  -1,  -1,  -1,  -1,  -1,  -1,
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
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 443
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
